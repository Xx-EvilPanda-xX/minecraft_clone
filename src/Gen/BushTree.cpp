#include "BushTree.h"

BushTree::BushTree()
	: Tree{ new Vector3i[13]{ { 0, 0, 0},  { 0, 1, 0 }, { 1, 0, 0 }, { 0, 0, 1 }, { -1, 0, 0 }, { 0, 0, -1 } },
							  0,
							  6,
							  BlockType::Flowers,
							  BlockType::Planks }
{
}

BushTree::~BushTree() = default;

const Vector3i* BushTree::getLeaves() const
{
	return m_Leaves;
}

int BushTree::getTrunkHeight() const
{
	return m_TrunkHeight;
}

int BushTree::getNumLeaves() const
{
	return m_NumLeaves;
}

BlockType BushTree::getLeavesType() const
{
	return m_LeavesType;
}

BlockType BushTree::getTrunkType() const
{
	return m_TrunkType;
}