#pragma once

#include <ogldev_math_3d.h>

class Camera
{
public:
	Camera();
	void SetPosition(float x, float y, float z);
	void OnKeyBoard(unsigned char key);
	Matrix4f GetMatrix();

private:
	Vector3f m_pos;
	Vector3f m_target;
	Vector3f m_up;
	float m_speed = 1.0f;
};

