#include "Layer.h"

Layer::Layer(Block block, int top, bool relative)
	: m_Block{ block },
	m_Top{},
	m_Relative{ relative }
{
	setTop(top);
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

bool Layer::isRelative() const
{
	return m_Relative;
}