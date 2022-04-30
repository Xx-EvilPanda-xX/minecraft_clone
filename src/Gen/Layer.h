#ifndef LAYER_H
#define LAYER_H

#include "../World/Block.h"

class Layer
{
private:
	BlockType m_Type;
	int m_Top;
	bool m_Relative;

public:
	Layer(BlockType type, int top, bool relative);
	~Layer();

	void setType(BlockType type);

	void setTop(int top);

	void setRelative(bool relative);

	BlockType getType() const;

	int getTop() const;

	bool isRelative() const;
};


#endif
