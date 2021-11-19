#ifndef TEXTURE_H
#define TEXTURE_H

class Texture
{
private:
	unsigned int m_Id;
	const char* m_Path;

	void load();

public:
	Texture(const char* path);

	Texture();

	unsigned int getId() const;
};

#endif
