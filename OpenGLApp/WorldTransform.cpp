#include "WorldTransform.h"

void WorldTransform::SetScale(float scale) {
	m_scale = scale;
}


void WorldTransform::SetRotation(float x, float y, float z) {
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}
void WorldTransform::SetPosition(float x, float y, float z) {
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}

void WorldTransform::Rotate(float x, float y, float z) {
	m_rotation.x += x;
	m_rotation.y += y;
	m_rotation.z += z;
}

Matrix4f WorldTransform::GetMatrix() {
	Matrix4f scaleMat;
	scaleMat.InitScaleTransform(m_scale, m_scale, m_scale);

	Matrix4f rotMat;
	rotMat.InitRotateTransform(m_rotation.x, m_rotation.y, m_rotation.z);

	Matrix4f translateMat;
	translateMat.InitTranslationTransform(m_pos.x, m_pos.y, m_pos.z);

	return translateMat * rotMat * scaleMat;
}