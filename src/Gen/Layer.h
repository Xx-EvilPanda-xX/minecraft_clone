#ifndef LAYER_H
#define LAYER_H

#include "../World/Block.h"

class Layer
{
private:
	Block m_Block;
	int m_Top;
	int m_VerticalLimit;
	bool m_Relative;

public:
	//vertical limit means it doesn't generate below that y value (ex. 0: always generates, 255: never generates)
	Layer(Block block, int top, int verticalLimit, bool relative);
	~Layer();

	void setBlock(Block block);

	void setTop(int top);

	void setVerticalLimit(int verticalLimit);

	void setRelative(bool relative);

	Block getBlock() const;

	int getTop() const;

	int getVerticalLimit() const;

	bool isRelative() const;
};


#endif
