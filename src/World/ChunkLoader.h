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

	static Chunk* decode(uint8_t* buffer, Vector2i chunkLoc, Shader& chunkShader, std::pair<std::mutex&, std::vector<unsigned int>&> bufferDestroyQueue);

	static uint8_t* readFromFile(std::string fileName, std::string worldName, size_t* outFileSize);

	static std::string calculateName(Vector2i loc);

public:
	static Chunk* loadChunk(Vector2i loc, std::string worldName, Shader& chunkShader, std::pair<std::mutex&, std::vector<unsigned int>&> bufferDestroyQueue);

	static void setLoadDir(std::filesystem::path newPath);
};

#endif
