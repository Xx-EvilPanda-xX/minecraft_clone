#include "Layer.h"

Layer::Layer(BlockType type, int top, bool relative)
	: m_Type{ type },
	m_Top{},
	m_Relative{ relative }
{
	setTop(top);
}

Layer::~Layer() = default;

void Layer::setType(BlockType type)
{
	m_Type = type;
}

void Layer::setTop(int top)
{
	if (top > 255) return;
	m_Top = top;
}

void Layer::setRelative(bool relative)
{
	m_Relative = relative;
}

BlockType Layer::getType() const
{
	return m_Type;
}

int Layer::getTop() const
{
	return m_Top;
}

bool Layer::isRelative() const
{
	return m_Relative;
}