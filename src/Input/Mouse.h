#ifndef MOUSE_H
#define MOUSE_H

class Mouse
{
private:
	float m_xOffset;
	float m_yOffset;

	float m_xPos;
	float m_yPos;

	float m_LastxPos;
	float m_LastyPos;

public:

	Mouse();

	~Mouse();

	void update();

	float getXOffset();

	float getYOffset();

	void setX(float x);

	void setY(float y);
};

#endif
