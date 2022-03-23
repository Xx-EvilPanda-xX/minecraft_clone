#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <string>
#include <map>
#include <filesystem>
#include "RenderData.h"
#include "Shader.h"
#include "../Input/EventHandler.h"
#include "../Window.h"

class TextRenderer
{
private:
	std::string m_Text;
	double m_xPos;
	double m_yPos;
	double m_FontSize;
	double m_CharSpacing;
	static std::map<char, unsigned int> m_Chars;

	RenderData m_Data;

	void prepare(int itr, Window& window);

	void create();

	static unsigned int getCharTex(const char* p);

public:
	TextRenderer(std::string text, double x, double y, double fontSize, double charSpacing, Shader& shader);

	TextRenderer();

	void render(EventHandler& handler, Window& window);

	void update(std::string text, double x, double y, double fontSize, double charSpacing);

	static void createChars();
};

#endif
