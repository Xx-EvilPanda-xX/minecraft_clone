#ifndef TEXTURE_H
#define TEXTURE_H

class Texture
{
private:
	unsigned int m_Id;
	const char* m_Path;

	void load(bool flip);

public:
	Texture(const char* path, bool flip);

	Texture();

	unsigned int getId() const;
};

#endif
