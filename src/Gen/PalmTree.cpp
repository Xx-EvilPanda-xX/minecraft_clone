#include "PalmTree.h"

PalmTree::PalmTree()
	: Tree{ new Vector3i[17]{ { 1, 5, 0 }, { 2, 5, 0 }, { 3, 5, 0 }, { 4, 4, 0 }, { -1, 5, 0 },
							  { -2, 5, 0 }, { -3, 5, 0 }, { -4, 4, 0 }, { 0, 5, 1 }, { 0, 5, 2 },
							  { 0, 5, 3 }, { 0, 4, 4 }, { 0, 5, -1 }, { 0, 5, -2 }, { 0, 5, -3 },
							  { 0, 4, -4 }, { 0, 6, 0 } },
							  6,
							  17,
							  BlockType::PalmLeaves,
							  BlockType::Wood }
{
}

PalmTree::~PalmTree() = default;

const Vector3i* PalmTree::getLeaves() const
{
	return m_Leaves;
}

int PalmTree::getTrunkHeight() const
{
	return m_TrunkHeight;
}

int PalmTree::getNumLeaves() const
{
	return m_NumLeaves;
}

BlockType PalmTree::getLeavesType() const
{
	return m_LeavesType;
}

BlockType PalmTree::getTrunkType() const
{
	return m_TrunkType;
}