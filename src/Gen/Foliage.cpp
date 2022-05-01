#include "Foliage.h"

Foliage::Foliage(Foliage::FoliageType type, double spawnRate, int spawnRangeBottom, int spawnRangeTop)
	: m_Type{ type },
	m_SpawnRate{ spawnRate },
	m_SpawnRangeBottom{ spawnRangeBottom },
	m_SpawnRangeTop{ spawnRangeTop }
{
	if (m_SpawnRate > 1.0)
		m_SpawnRate = 1.0;

	if (m_SpawnRate < 0.0)
		m_SpawnRate = 0.0;

	if (m_SpawnRangeBottom > 255)
		m_SpawnRangeBottom = 255;

	if (m_SpawnRangeTop < 0)
		m_SpawnRangeTop = 0;
}

Foliage::~Foliage() = default;

Foliage::FoliageType Foliage::getType() const
{
	return m_Type;
}

double Foliage::getSpawnRate() const
{
	return m_SpawnRate;
}

int Foliage::getSpawnRangeBottom() const
{
	return m_SpawnRangeBottom;
}

int Foliage::getSpawnRangeTop() const
{
	return m_SpawnRangeTop;
}