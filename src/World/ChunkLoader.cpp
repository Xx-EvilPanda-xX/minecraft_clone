#include "ChunkLoader.h"
#include "../../lib/snappy/snappy.h"

std::filesystem::path ChunkLoader::s_LoadDir{};

uint8_t* ChunkLoader::readFromFile(std::string fileName, std::string worldName, size_t* outFileSize)
{
	std::string filePath{ s_LoadDir.generic_string() + worldName + "/" + fileName };

	if (!std::filesystem::exists(filePath))
		return nullptr;

	std::ifstream inStream{};
	inStream.open(filePath, std::fstream::in | std::fstream::binary);
	size_t fileSize{ std::filesystem::file_size(filePath) };
	char* buffer{ new char[fileSize]{} };
	inStream.read(buffer, fileSize);
	inStream.close();

	*outFileSize = fileSize;
	return (uint8_t*)buffer;
}

std::string ChunkLoader::calculateName(Vector2i loc)
{
	std::string name{};
	name.append(std::to_string(loc.x));
	name.append(",");
	name.append(std::to_string(loc.y));
	name.append(".chunk");
	return name;
}

Chunk* ChunkLoader::decode(uint8_t* buffer, Vector2i chunkLoc, Shader& chunkShader, std::pair<std::mutex&, std::vector<unsigned int>&> bufferDestroyQueue)
{
	Chunk* chunk{ new Chunk{ chunkLoc, chunkShader, bufferDestroyQueue, true } };

	size_t index{ 16 };
	for (int i{}; i < g_ChunkCap; ++i)
	{
		ChunkSection* section{ new ChunkSection{} };

		if (!buffer[i])
		{
			section->m_OpaqueBlocks = ((int*)buffer)[index / 4];
			section->m_AirBlocks = ((int*)buffer)[(index / 4) + 1];
			section->m_Empty = false;

			if (section->m_OpaqueBlocks == g_ChunkSectionCapacity)
				section->m_Full = true;	
			

			index += 8;

			for (int j{}; j < g_ChunkSectionCapacity; ++j)
			{
				Block block{};

				block.m_Type = (char)buffer[index];
				uint8_t flags{ buffer[index + 1] };

				block.m_Transparent = flags & 1;
				block.m_Surface = (flags >> 1) & 1;
				block.m_FoliageMesh = (flags >> 2) & 1;

				section->m_Blocks[j] = block;
				index += 2;
			}
		}

		chunk->addSection(section);
	}

	return chunk;
}

Chunk* ChunkLoader::loadChunk(Vector2i loc, std::string worldName, Shader& chunkShader, std::pair<std::mutex&, std::vector<unsigned int>&> bufferDestroyQueue)
{
	std::string name{ calculateName(loc) };
	size_t fileSize{};
	uint8_t* buffer{ readFromFile(name, worldName, &fileSize) };

	if (!buffer)
		return nullptr;

	size_t uncompressedSize{};
	snappy::GetUncompressedLength((const char*)buffer, fileSize, &uncompressedSize);
	
	char* uncompressed{ new char[uncompressedSize]{} };
	bool result{ snappy::RawUncompress((const char*)buffer, fileSize, uncompressed) };

	if (!result)
		std::cout << "Couldn't decompress\n";

	Chunk* chunk{ decode((uint8_t*)uncompressed, loc, chunkShader, bufferDestroyQueue) };
	delete[] buffer;
	return chunk;
}

void ChunkLoader::setLoadDir(std::filesystem::path newPath)
{
	newPath.remove_filename();
	s_LoadDir = newPath;
}