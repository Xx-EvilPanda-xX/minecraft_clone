#ifndef TREE_H
#define TREE_H

#include "../Math/Vector3i.h"
#include "../World/Block.h"

class Tree
{
public:
	virtual const Vector3i* getLeaves() const = 0;
	virtual int getTrunkHeight() const = 0;
	virtual int getNumLeaves() const = 0;
	virtual BlockType getLeavesType() const = 0;
	virtual BlockType getTrunkType() const = 0;
	virtual ~Tree();

protected:
	Tree(const Vector3i* leaves, const int height, const int numLeaves, const BlockType leavesType, const BlockType trunkType);

	const Vector3i* const m_Leaves;
	const int m_TrunkHeight;
	const int m_NumLeaves;
	const BlockType m_LeavesType;
	const BlockType m_TrunkType;
};

#endif

