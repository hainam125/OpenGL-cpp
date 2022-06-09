#pragma once

#include <ogldev_math_3d.h>

class Camera
{
public:
	Camera(int windowWidth, int windowHeight);
	Camera(int windowWidth, int windowHeight, const Vector3f& pos, const Vector3f& target, const Vector3f& up);
	void SetPosition(float x, float y, float z);
	void OnKeyBoard(unsigned char key);
	Matrix4f GetMatrix();
	void OnMouse(int x, int y);
	void OnRender();

private:
	void Init();
	void Update();

	Vector3f m_pos;
	Vector3f m_target;
	Vector3f m_up;
	float m_speed = 1.0f;

	int m_windowWidth;
	int m_windowHeight;

	float m_AngleH;
	float m_AngleV;

	bool m_OnUpperEdge;
	bool m_OnLowerEdge;
	bool m_OnLeftEdge;
	bool m_OnRightEdge;

	Vector2i m_mousePos;
};

