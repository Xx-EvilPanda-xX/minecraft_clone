#ifndef LAYER_H
#define LAYER_H

#include "../World/Block.h"

class Layer
{
private:
	Block m_Block;
	int m_Top;
	bool m_Relative;

public:
	Layer(Block block, int top, bool relative);
	~Layer();

	void setBlock(Block block);

	void setTop(int top);

	void setRelative(bool relative);

	Block getBlock() const;

	int getTop() const;

	bool isRelative() const;
};


#endif
