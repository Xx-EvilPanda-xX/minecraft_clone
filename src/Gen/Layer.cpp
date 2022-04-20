#include "Layer.h"

Layer::Layer(BlockType type, int top)
	: m_Type{ BlockType::Air },
	m_Top{}
{
	m_Type = type;
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

BlockType Layer::getType() const
{
	return m_Type;
}

int Layer::getTop() const
{
	return m_Top;
}