#include "ChunkSaver.h"
#include "../../lib/snappy/snappy.h"	

std::filesystem::path ChunkSaver::s_SaveDir{ };

Chunk* ChunkSaver::makeDeepCopy(const Chunk* chunk)
{
	//these are just placeholders since the constructor requires them
	//and are never used so its okay if the references to them are invalid.
	//the only thing we care about here are the chunk sections
	Shader shader{};
	std::mutex m{};
	std::vector<unsigned int> vec{};
	std::pair<std::mutex&, std::vector<unsigned int>&> pair{ m, vec };

	Chunk* newChunk{ new Chunk{ chunk->m_Location, shader, pair, false } }; 
	for (int i{}; i < g_ChunkCap; ++i)
	{
		ChunkSection* newSection{ new ChunkSection{} };
		ChunkSection* oldSection{ chunk->m_Sections[i] };
		for (int j{}; j < g_ChunkSectionCapacity; ++j)
		{
			newSection->m_Blocks[j] = oldSection->m_Blocks[j];
		}
		newSection->m_OpaqueBlocks = oldSection->m_OpaqueBlocks;
		newSection->m_AirBlocks = oldSection->m_AirBlocks;
		newSection->m_Empty = oldSection->m_Empty;
		newSection->m_Full = oldSection->m_Empty;
		newChunk->m_Sections[i] = newSection;
	}
	
	return newChunk;
}

size_t ChunkSaver::calculateFileSize(const Chunk* chunk)
{
	//header
	size_t size{ 16 };

	//payload
	for (int i{}; i < g_ChunkCap; ++i)
	{
		//no data stored for empty sections
		if (chunk->m_Sections[i]->m_Empty)
			continue;
		else
			size += 8200; //8192 (4092 * 2 bytes per block) plus two 4 byte ints
	}

	return size;
}

void ChunkSaver::encode(uint8_t* buffer, const Chunk* chunk)
{
	size_t index{};

	for (int i{}; i < g_ChunkCap; ++i)
	{
		buffer[index] = (uint8_t)chunk->m_Sections[i]->m_Empty;
		++index;
	}
	
	for (int i{}; i < g_ChunkCap; ++i)
	{
		ChunkSection* section{ chunk->m_Sections[i] };
		if (section->m_Empty)
			continue;

		((int*)buffer)[index / 4] = section->m_OpaqueBlocks;
		((int*)buffer)[(index / 4) + 1] = section->m_AirBlocks;
		index += 8;

		for (int j{}; j < g_ChunkSectionCapacity; ++j)
		{
			Block block{ section->m_Blocks[j] };
			uint8_t id{ (uint8_t)block.m_Type };

			uint8_t bit1{ (uint8_t)block.m_Transparent };
			uint8_t bit2{ (uint8_t)(((uint8_t)block.m_Surface) << 1) };
			uint8_t bit3{ (uint8_t)(((uint8_t)block.m_FoliageMesh) << 2) };

			buffer[index] = id;
			buffer[index + 1] = bit1 | bit2 | bit3;
			index += 2;
		}
	}
}

void ChunkSaver::writeToFile(uint8_t* buffer, size_t size, std::string fileName, std::string worldName)
{
	char* compressed{ new char[snappy::MaxCompressedLength(size)] };
	size_t compressedLen{};
	snappy::RawCompress((const char*) buffer, size, compressed, &compressedLen);

	std::ofstream outStream{};
	std::string filePath{ s_SaveDir.generic_string() + worldName + "/" + fileName};
	std::string worldPath{ s_SaveDir.generic_string() + worldName + "/" };

	if (!std::filesystem::exists(s_SaveDir))
		std::filesystem::create_directory(s_SaveDir);

	if (!std::filesystem::exists(worldPath))
		std::filesystem::create_directory(worldPath);

	outStream.open(filePath, std::fstream::out | std::fstream::binary);
	outStream.write((const char*)compressed, compressedLen);
	outStream.close();
	delete[] compressed;
}

std::string ChunkSaver::calculateName(Vector2i loc)
{
	std::string name{};
	name.append(std::to_string(loc.x));
	name.append(",");
	name.append(std::to_string(loc.y));
	name.append(".chunk");
	return name;
}

std::thread ChunkSaver::saveChunk(const Chunk* chunk, std::string worldName)
{
	Chunk* newChunk{ makeDeepCopy(chunk)};

	std::thread saver{ [newChunk, worldName] {
		size_t bufferLen{ calculateFileSize(newChunk) };
		uint8_t* buffer{ new uint8_t[bufferLen] };

		encode(buffer, newChunk);
		std::string fileName{ calculateName(newChunk->m_Location) };
		writeToFile(buffer, bufferLen, fileName, worldName);

		delete[] buffer;
		delete newChunk;
	} };

	return saver;
}

void ChunkSaver::setSaveDir(std::filesystem::path newPath)
{
	newPath.remove_filename();
	s_SaveDir = newPath;
}