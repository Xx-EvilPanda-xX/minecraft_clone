#ifndef FOLIAGE_H
#define FOLIAGE_H

class Foliage
{
public:
	enum class FoliageType
	{
		OAK_TREE,
		PALM_TREE,
		BUSH,
		CACTUS,
	};

private:
	FoliageType m_Type;
	double m_SpawnRate;
	int m_SpawnRangeBottom;
	int m_SpawnRangeTop;

public:
	Foliage(FoliageType type, double spawnRate, int spawnRangeBottom, int spawnRangeTop);
	~Foliage();

	FoliageType getType() const;

	double getSpawnRate() const;

	int getSpawnRangeBottom() const;

	int getSpawnRangeTop() const;
};

#endif