#ifndef CHUNK_LOADER_H
#define CHUNK_LOADER_H

#include <filesystem>
#include "Chunk.h"

class ChunkLoader
{
private:
	static std::filesystem::path s_LoadDir;

	ChunkLoader() = default;
	~ChunkLoader() = default;

	static Chunk* decode(uint8_t* buffer, size_t bufLen, Vector2i chunkLoc,
						Shader& chunkShader,
						std::pair<std::mutex&, std::vector<unsigned int>&> bufferDestroyQueue,
						std::vector<QueueBlock>& globalBlockQueue);

	static uint8_t* readFromFile(std::string fileName, std::string worldName, size_t* outFileSize);

	static std::string calculateName(Vector2i loc);

	template <typename T>
	static T get(uint8_t* buffer, size_t bufLen, size_t& index);

public:
	static Chunk* loadChunk(Vector2i loc, std::string worldName,
						Shader& chunkShader,
						std::pair<std::mutex&, std::vector<unsigned int>&> bufferDestroyQueue,
						std::vector<QueueBlock>& globalBlockQueue);

	static void setLoadDir(std::filesystem::path newPath);
};

#endif
