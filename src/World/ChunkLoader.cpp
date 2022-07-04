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

Chunk* ChunkLoader::decode(uint8_t* buffer, size_t bufLen, Vector2i chunkLoc,
						Shader& chunkShader,
						std::pair<std::mutex&, std::vector<unsigned int>&> bufferDestroyQueue,
						std::vector<QueueBlock>& globalBlockQueue)
{
	Chunk* chunk{ new Chunk{ chunkLoc, chunkShader, bufferDestroyQueue, true } };
	size_t index{ 16 };

	try
	{
		uint32_t numBlocksInQueue{ get<uint32_t>(buffer, bufLen, index) };

		for (int i{}; i < g_ChunkCap; ++i)
		{
			ChunkSection* section{ new ChunkSection{} };

			if (!buffer[i])
			{
				section->m_OpaqueBlocks = get<int>(buffer, bufLen, index);
				section->m_AirBlocks = get<int>(buffer, bufLen, index);
				section->m_Empty = false;

				if (section->m_OpaqueBlocks == g_ChunkSectionCapacity)
					section->m_Full = true;

				for (int j{}; j < g_ChunkSectionCapacity; ++j)
				{
					Block block{};

					block.m_Type = get<char>(buffer, bufLen, index);
					uint8_t flags{ get<uint8_t>(buffer, bufLen, index) };

					block.m_Transparent = flags & 1;
					block.m_Surface = (flags >> 1) & 1;
					block.m_FoliageMesh = (flags >> 2) & 1;

					section->m_Blocks[j] = block;
				}
			}

			chunk->addSection(section);
		}

		for (uint32_t i{}; i < numBlocksInQueue; ++i)
		{
			globalBlockQueue.push_back(get<QueueBlock>(buffer, bufLen, index));
		}
	}
	catch (std::string e)
	{
		std::cout << e;
		return nullptr;
	}

	return chunk;
}

template <typename T>
T ChunkLoader::get(uint8_t* buffer, size_t bufLen, size_t& index)
{
	if (index >= bufLen)
		throw std::string{ "Buffer access exception: index was greater than size\n" };

	T val{ *((T*)(buffer + index)) };
	index += sizeof(T);
	return val;
}

Chunk* ChunkLoader::loadChunk(Vector2i loc, std::string worldName,
							Shader& chunkShader,
							std::pair<std::mutex&, std::vector<unsigned int>&> bufferDestroyQueue,
							std::vector<QueueBlock>& globalBlockQueue)
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

	Chunk* chunk{ decode((uint8_t*)uncompressed, uncompressedSize, loc, chunkShader, bufferDestroyQueue, globalBlockQueue) };

	if (!chunk)
		std::cout << "Chunk at " << loc.x << ", " << loc.y << " may be corrupt and will not be loaded\n";

	delete[] buffer;
	delete[] uncompressed;
	return chunk;
}

void ChunkLoader::setLoadDir(std::filesystem::path newPath)
{
	newPath.remove_filename();
	s_LoadDir = newPath;
}