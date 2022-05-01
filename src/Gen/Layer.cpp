#include "Layer.h"

Layer::Layer(Block block, int top, int verticalLimit, bool relative)
	: m_Block{ block },
	m_Top{},
	m_VerticalLimit{},
	m_Relative{ relative }
{
	setTop(top);
	setVerticalLimit(verticalLimit);
}

Layer::~Layer() = default;

void Layer::setBlock(Block block)
{
	m_Block = block;
}

void Layer::setTop(int top)
{
	if (top > 255) return;
	m_Top = top;
}

void Layer::setVerticalLimit(int verticalLimit)
{
	if (verticalLimit > 255 || verticalLimit < 0) return;
	m_VerticalLimit = verticalLimit;
}

void Layer::setRelative(bool relative)
{
	m_Relative = relative;
}

Block Layer::getBlock() const
{
	return m_Block;
}

int Layer::getTop() const
{
	return m_Top;
}

int Layer::getVerticalLimit() const
{
	return m_VerticalLimit;
}

bool Layer::isRelative() const
{
	return m_Relative;
}