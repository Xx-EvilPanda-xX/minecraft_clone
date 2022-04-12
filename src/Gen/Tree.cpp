#include "Tree.h"

Tree::Tree(const Vector3i* leaves, const int height, const int numLeaves, const BlockType leavesType, const BlockType trunkType)
	: m_Leaves{ leaves },
	m_TrunkHeight{ height },
	m_NumLeaves{ numLeaves },
	m_LeavesType{ leavesType },
	m_TrunkType{ trunkType }
{
}

Tree::~Tree()
{
	delete[] m_Leaves;
}