#include "TextRenderer.h"
#include "Texture.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

TextRenderer::TextRenderer(std::string text, double x, double y, double fontSize, double charSpacing, Shader& shader)
	: m_Text{ text },
	m_xPos{ x },
	m_yPos{ y },
	m_FontSize{ fontSize },
	m_CharSpacing{ charSpacing },
	m_Data{}
{
	m_Data.shader = &shader;
	create();
}

TextRenderer::TextRenderer() = default;

std::map<char, unsigned int> TextRenderer::m_Chars{};

void TextRenderer::create()
{
	float vertices[]{ 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f };
	float tex[]{ 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0, 0.0f };
	int indices[]{ 0, 1, 2, 3, 1, 2 };

	unsigned int vbo, tbo, ebo, vao;
	glCreateVertexArrays(1, &vao);
	glCreateBuffers(1, &vbo);
	glCreateBuffers(1, &tbo);
	glCreateBuffers(1, &ebo);

	m_Data.vao = vao;
	m_Data.vbo = vbo;
	m_Data.tbo = tbo;
	m_Data.ebo = ebo;

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex), tex, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	m_Data.indexCount = sizeof(indices) / sizeof(indices[0]);
}

void TextRenderer::prepare(int itr, Window& window)
{
	glm::dmat4 model(1.0);
	model = glm::translate(model, glm::dvec3{ m_xPos + (itr * (m_FontSize * m_CharSpacing)), m_yPos, 0.0});
	model = glm::scale(model, glm::dvec3{ m_FontSize, m_FontSize, m_FontSize });

	int width, height;
	glfwGetWindowSize(window.getGlfwWindow(), &width, &height);
	double aspect{ static_cast<double>(width) / static_cast<double>(height) };
	//this line took a while phew
	glm::dmat4 proj{ glm::ortho(-1.0, 1.0, 1.0 - (1.0 / (aspect / 2.0)), 1.0, -1.0, 1.0)};

	m_Data.shader->bind();
	m_Data.shader->setMat4("model", model);
	m_Data.shader->setMat4("proj", proj);
}

void TextRenderer::render(EventHandler& handler, Window& window)
{
	if (!handler.m_IsWireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);

	for (int i{}; i < m_Text.length(); ++i)
	{
		if (m_Text.at(i) != ' ')
		{
			prepare(i, window);
			glBindTexture(GL_TEXTURE_2D, m_Chars.at(m_Text.at(i)));

			glBindVertexArray(m_Data.vao);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Data.ebo);
			glDrawElements(GL_TRIANGLES, m_Data.indexCount, GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glBindVertexArray(0);

			glBindTexture(GL_TEXTURE_2D, 0);
			m_Data.shader->unbind();
		}
	}

	if (!handler.m_IsWireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
}

void TextRenderer::update(std::string text, double x, double y, double fontSize, double charSpacing)
{
	m_Text = text;
	m_xPos = x;
	m_yPos = y;
	m_FontSize = fontSize;
	m_CharSpacing = charSpacing;
}

void TextRenderer::createChars()
{
	for (int i{}; i < 128; ++i)
	{
		std::string strPath{ "assets/fonts/default/char_" };
		strPath.append(std::to_string(i)).append(".png");
		namespace fs = std::filesystem;
		fs::path path{ strPath };
		if (fs::exists(path))
		{
			m_Chars.emplace(static_cast<char>(i), getCharTex(strPath.c_str()));
		}
	}
}

unsigned int TextRenderer::getCharTex(const char* p)
{
	Texture tex{ p, true };
	return tex.getId();
}