#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>
#include <filesystem>
#include <utility>

enum class Token {
	Equals,
	Semicolon,
	Identifier,
	Invalid
};

class Config
{
private:
	Config(std::filesystem::path configFile);

	~Config() = default;

	std::map<std::string, std::string> m_ConfigMap;
	
	void parse(std::string_view unparsed);

	Token processToken(std::string_view tok);

	template <typename T>
	std::optional<T> getValue(std::string key);

public:
	static void loadAllConfigurations(std::filesystem::path path);
};

#endif
