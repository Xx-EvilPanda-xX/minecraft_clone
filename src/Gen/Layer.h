#ifndef LAYER_H
#define LAYER_H

#include "../World/Block.h"

class Layer
{
private:
	BlockType m_Type;
	int m_Top;

public:
	Layer(BlockType type, int top);
	~Layer();

	void setType(BlockType type);

	void setTop(int top);

	BlockType getType() const;

	int getTop() const;

};


#endif
