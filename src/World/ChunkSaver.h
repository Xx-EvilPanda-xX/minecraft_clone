#ifndef CHUNK_SAVER_H
#define CHUNK_SAVER_H

#include <filesystem>
#include "Chunk.h"

class ChunkSaver
{
private:
	static std::filesystem::path s_SaveDir;

	ChunkSaver() = default;
	~ChunkSaver() = default;

	static Chunk* makeDeepCopy(const Chunk* chunk);

	static size_t calculateFileSize(const Chunk* chunk);

	static void encode(uint8_t* buffer, const Chunk* chunk);

	static void writeToFile(uint8_t* buffer, size_t size, std::string fileName, std::string worldName);

	static std::string calculateName(Vector2i loc);
public:
	static std::thread saveChunk(const Chunk* chunk, std::string worldName);

	static void setSaveDir(std::filesystem::path newPath);
};

#endif