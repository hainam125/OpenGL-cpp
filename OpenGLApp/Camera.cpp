#include "Camera.h"
#include <freeglut.h>

static int MARGIN = 10;
static float EDGE_STEP = 0.02f;

Camera::Camera(int windowWidth, int windowHeight) {
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
	m_pos = Vector3f(0.0f, 0.0f, 0.0f);
	m_target = Vector3f(0.0f, 0.0f, 1.0f);
	m_up = Vector3f(0.0f, 1.0f, 0.0f);

	Init();
}

Camera::Camera(int windowWidth, int windowHeight, const Vector3f& pos, const Vector3f& target, const Vector3f& up) {
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
	m_pos = pos;
	m_target = target;
	m_target.Normalize();
	m_up = up;
	m_up.Normalize();

	Init();
}

void Camera::Init() {
	Vector3f HTarget(m_target.x, 0.0, m_target.z);
	HTarget.Normalize();

	float Angle = ToDegree(asin(abs(HTarget.z)));

	if (HTarget.z >= 0.0f)
	{
		if (HTarget.x >= 0.0f)
		{
			m_AngleH = 360.0f - Angle;//[270;360]
		}
		else
		{
			m_AngleH = 180.0f + Angle;//[180;270]
		}
	}
	else
	{
		if (HTarget.x >= 0.0f)
		{
			m_AngleH = Angle;//[0;90]
		}
		else
		{
			m_AngleH = 180.0f - Angle;//[90;180]
		}
	}

	m_AngleV = -ToDegree(asin(m_target.y));//[-90;90]

	m_OnUpperEdge = false;
	m_OnLowerEdge = false;
	m_OnLeftEdge = false;
	m_OnRightEdge = false;
	m_mousePos.x = m_windowWidth / 2;
	m_mousePos.y = m_windowHeight / 2;
}

void Camera::Update()
{
	Vector3f Yaxis(0.0f, 1.0f, 0.0f);

	// Rotate the view vector by the horizontal angle around the vertical axis
	Vector3f View(1.0f, 0.0f, 0.0f);
	View.Rotate(m_AngleH, Yaxis);
	View.Normalize();

	// Rotate the view vector by the vertical angle around the horizontal axis
	Vector3f U = Yaxis.Cross(View);
	U.Normalize();
	View.Rotate(m_AngleV, U);

	m_target = View;
	m_target.Normalize();

	m_up = m_target.Cross(U);
	m_up.Normalize();
}

void Camera::SetPosition(float x, float y, float z) {
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}

void Camera::OnKeyBoard(unsigned char key) {
	switch (key)
	{
	case GLUT_KEY_UP:
		m_pos += (m_target * m_speed);
		break;
	case GLUT_KEY_DOWN:
		m_pos -= (m_target * m_speed);
		break;
	case GLUT_KEY_LEFT:
	{
		Vector3f left = m_target.Cross(m_up);
		left.Normalize();
		m_pos += left;
	}
	break;
	case GLUT_KEY_RIGHT:
	{
		Vector3f right = m_up.Cross(m_target);
		right.Normalize();
		m_pos += right;
	}
	break;
	case GLUT_KEY_PAGE_UP:
		m_pos.y += m_speed;
		break;
	case GLUT_KEY_PAGE_DOWN:
		m_pos.y -= m_speed;
		break;
	case '+':
		m_speed += 0.1f;
		printf("Speed changed to %f\n", m_speed);
		break;
	case '-':
		m_speed -= 0.1f;
		if (m_speed < 0.1f) {
			m_speed = 0.1f;
		}
		printf("Speed changed to %f\n", m_speed);
		break;
	}
}

void Camera::OnMouse(int x, int y) {
	int DeltaX = x - m_mousePos.x;
	int DeltaY = y - m_mousePos.y;

	m_mousePos.x = x;
	m_mousePos.y = y;

	m_AngleH += (float)DeltaX / 20.0f;
	m_AngleV += (float)DeltaY / 50.0f;

	if (DeltaX == 0) {
		if (x <= MARGIN) {
			m_OnLeftEdge = true;
		}
		else if (x >= (m_windowWidth - MARGIN)) {
			m_OnRightEdge = true;
		}
	}
	else {
		m_OnLeftEdge = false;
		m_OnRightEdge = false;
	}

	if (DeltaY == 0) {
		if (y <= MARGIN) {
			m_OnUpperEdge = true;
		}
		else if (y >= (m_windowHeight - MARGIN)) {
			m_OnLowerEdge = true;
		}
	}
	else {
		m_OnUpperEdge = false;
		m_OnLowerEdge = false;
	}

	Update();
}

void Camera::OnRender() {
	bool ShouldUpdate = false;

	if (m_OnLeftEdge) {
		m_AngleH -= EDGE_STEP;
		ShouldUpdate = true;
	}
	else if (m_OnRightEdge) {
		m_AngleH += EDGE_STEP;
		ShouldUpdate = true;
	}

	if (m_OnUpperEdge) {
		if (m_AngleV > -90.0f) {
			m_AngleV -= EDGE_STEP;
			ShouldUpdate = true;
		}
	}
	else if (m_OnLowerEdge) {
		if (m_AngleV < 90.0f) {
			m_AngleV += EDGE_STEP;
			ShouldUpdate = true;
		}
	}

	if (ShouldUpdate) {
		Update();
	}
}

Matrix4f Camera::GetMatrix() {
	Matrix4f CameraTransformation;
	CameraTransformation.InitCameraTransform(m_pos, m_target, m_up);
	return CameraTransformation;
}