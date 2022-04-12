#include "OakTree.h"

OakTree::OakTree()
	: Tree{ new Vector3i[13]{ { 1, 3, 0 }, { 0, 3, 1 }, { -1, 3, 0 }, { 0, 3, -1 }, { 1, 2, 0 },
							  { 0, 2, 1 }, { -1, 2, 0 }, { 0, 2, -1 }, { 1, 2, 1 }, { -1, 2, 1 },
							  { 1, 2, -1 }, { -1, 2, -1 }, { 0, 4, 0} },
							  4,
							  13,
							  BlockType::Leaves,
							  BlockType::Wood }
{
}

OakTree::~OakTree() = default;

const Vector3i* OakTree::getLeaves()
{
	return m_Leaves;
}

int OakTree::getTrunkHeight()
{
	return m_TrunkHeight;
}

int OakTree::getNumLeaves()
{
	return m_NumLeaves;
}

BlockType OakTree::getLeavesType()
{
	return m_LeavesType;
}

BlockType OakTree::getTrunkType()
{
	return m_TrunkType;
}