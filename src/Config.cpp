#include "Config.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <charconv>
#include "Constants.h"

Config::Config(std::filesystem::path configFile)
{
    std::string unparsed{};
    std::ifstream configFileStream{};

    configFileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        configFileStream.open(configFile);
        std::stringstream inStream{};
        inStream << configFileStream.rdbuf();
        configFileStream.close();
        unparsed = inStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "Error: Failed to read config file because: " << e.what() << std::endl;
    }

    parse(unparsed);
}

void Config::parse(std::string_view unparsed)
{
    std::string preprocessed{};
    for (int i{}; i < unparsed.size(); ++i)
    {
        char c = unparsed.at(i);

        if (c == ' ' || c == '\n')
            continue;

        if (c == '#')
        {
            while (i != unparsed.size() - 1 && unparsed.at(i) != '\n')
            {
                ++i;
            }
            continue;
        }

        preprocessed.append(&unparsed.at(i), 1);
    }

    std::string currentToken{};
    std::optional<std::string> tempKey{ std::nullopt };
    bool error{ false };

    for (int i{}; i < preprocessed.size() - 1; ++i)
    {
        currentToken.append(&preprocessed.at(i), 1);

        switch (processToken(std::string{ preprocessed.at(i + 1) }))
        {
            case Token::Equals:
            {
                switch (processToken(currentToken))
                {
                case Token::Identifier:
                    if (tempKey != std::nullopt)
                    {
                        std::cerr << "Config syntax error: There must be exactly one value per key.\n";
                        error = true;
                    }

                    tempKey = std::optional{ currentToken };
                    currentToken.clear();
                    ++i;
                    break;
                default:
                    std::cerr << "Config syntax error: Expected indentifier but got some other token.\n";
                    error = true;
                }
                break;
            }
            case Token::Semicolon:
            {
                switch (processToken(currentToken))
                {
                case Token::Identifier:
                    if (tempKey == std::nullopt)
                    {
                        std::cerr << "Config syntax error: No key provided.\n";
                        error = true;
                    }

                    m_ConfigMap.emplace(tempKey.value(), currentToken);
                    tempKey = std::nullopt;
                    currentToken.clear();
                    ++i;
                    break;
                default:
                    std::cerr << "Config syntax error: Expected indentifier but got some other token.\n";
                    error = true;
                }
                break;
            }
            case Token::Identifier:
                continue;
            case Token::Invalid:
                std::cerr << "Config syntax error: Expected valid character.\n";
                error = true;
                break;
        }
    }

    if (error)
        std::exit(-1);
}

Token Config::processToken(std::string_view tok)
{
    if (tok.size() == 1)
    {
        if (tok == "=")
            return Token::Equals;
        if (tok == ";")
            return Token::Semicolon;
    }

    for (int i{}; i < tok.size(); ++i)
    {
        if (tok.at(i) < '!' || tok.at(i) > '~')
            return Token::Invalid;
    }

    return Token::Identifier;
}

template <typename T>
std::optional<T> Config::getValue(std::string key)
{
    try
    {
        std::string value = m_ConfigMap.at(key);

        T val{};
        auto [p, ec] = std::from_chars(value.c_str(), value.c_str() + value.length(), val);

        if (ec == std::errc{} && *p == '\0')
            return std::optional<T>(val);
        else
        {
            std::cerr << "Failed to parse config `" << key << "` (" << value << "). Default hardcoded value will be used.\n";
            return std::nullopt;
        }
    }
    catch (std::out_of_range& e)
    {
        std::cerr << "No config definition for `" << key << "`. Default hardcoded value will be used.\n";
        return std::nullopt;
    }
}

void Config::loadAllConfigurations(std::filesystem::path path)
{
    Config config{ path };

    std::optional window_width = config.getValue<int>("window_width");
    std::optional window_height = config.getValue<int>("window_height");
    std::optional fullscreen_width = config.getValue<int>("fullscreen_width");
    std::optional fullscreen_height = config.getValue<int>("fullscreen_height");
    std::optional fullscr = config.getValue<int>("enable_fullscreen");
    std::optional mouse_sensitivity = config.getValue<double>("mouse_sensitivity");
    std::optional flat_world = config.getValue<int>("generate_flat_world");
    std::optional flat_world_height = config.getValue<int>("flat_world_height");
    std::optional load_distance = config.getValue<int>("load_distance");
    std::optional render_distance = config.getValue<int>("render_distance");
    std::optional player_reach = config.getValue<double>("player_reach");
    std::optional block_break_cooldown = config.getValue<double>("block_break_cooldown");
    std::optional block_place_cooldown = config.getValue<double>("block_place_cooldown");
    std::optional use_translucent_water = config.getValue<int>("render_translucent_water");
    std::optional water_level = config.getValue<int>("water_level");
    std::optional beach_size = config.getValue<int>("beach_size");
    std::optional walk_speed = config.getValue<double>("walk_speed");
    std::optional sprint_speed = config.getValue<double>("sprint_speed");
    std::optional player_drift = config.getValue<double>("player_drift");
    std::optional player_size = config.getValue<double>("player_size");
    std::optional camera_fov = config.getValue<double>("camera_fov");
    std::optional grav = config.getValue<double>("gravity");
    std::optional jump_height = config.getValue<double>("jump_height");

    using namespace constants;
    using std::nullopt;

    if (window_width != nullopt) windowWidth = window_width.value();
    if (window_height != nullopt) windowHeight = window_height.value();
    if (fullscreen_width != nullopt) fullscreenWidth = fullscreen_width.value();
    if (fullscreen_height != nullopt) fullscreenHeight = fullscreen_height.value();
    if (fullscr != nullopt) fullscreen = fullscr.value();
    if (mouse_sensitivity != nullopt) mouseSensitivity = mouse_sensitivity.value();
    if (flat_world != nullopt) flatWorld = flat_world.value();
    if (flat_world_height != nullopt) flatWorldHeight = flat_world_height.value();
    if (load_distance != nullopt) loadDistance = load_distance.value();
    if (render_distance != nullopt) renderDistance = render_distance.value();
    if (player_reach != nullopt) playerReach = player_reach.value();
    if (block_break_cooldown != nullopt) blockBreakCooldown = block_break_cooldown.value();
    if (block_place_cooldown != nullopt) blockPlaceCooldown = block_place_cooldown.value();
    if (use_translucent_water != nullopt) useTranslucentWater = use_translucent_water.value();
    if (water_level != nullopt) waterLevel = water_level.value();
    if (beach_size != nullopt) beachSize = beach_size.value();
    if (walk_speed != nullopt) walkSpeed = walk_speed.value();
    if (sprint_speed != nullopt) sprintSpeed = sprint_speed.value();
    if (player_drift != nullopt) playerDrift = player_drift.value();
    if (player_size != nullopt) playerSize = player_size.value();
    if (camera_fov != nullopt) cameraFov = camera_fov.value();
    if (grav != nullopt) gravity = grav.value();
    if (jump_height != nullopt) jumpHeight = jump_height.value();
}