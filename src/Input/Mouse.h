#ifndef MOUSE_H
#define MOUSE_H

class Mouse
{
private:
	float m_xOffset;
	float m_yOffset;

	bool usedX;
	bool usedY;

public:

	Mouse();

	~Mouse();

	float getXOffset();

	float getYOffset();

	void setXOffset(float x);

	void setYOffset(float y);
};

#endif
