#include "pch.h"
#include "Transform.h"

Transform::Transform() : Component(COMPONENT_TYPE::TRANSFORM)
{

}

Transform::~Transform()
{

}

void Transform::FinalUpdate()
{
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);
	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);

	_matLocal = matScale * matRotation * matTranslation;
	_matWorld = _matLocal;

	shared_ptr<Transform> parent = GetParent().lock();
	if (parent != nullptr)
	{
		// ºÎ¸ð´ÔÀÇ worldMatrix¸¦ °öÇØÁÜ
		_matWorld *= parent->GetLocalToWorldMatrix();
	}
}


// CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushData(&_transform, sizeof(_transform));

