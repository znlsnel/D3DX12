#pragma once
#include "Component.h"

enum class PROJECTION_TYPE
{
	PERSPECTIVE, // 원근 투영
	ORTHOGRAPHIC, // 직교 투영
};

class Camera : public Component
{
public:
	Camera();
	virtual ~Camera();

	virtual void FinalUpdate() override;
	void Render();


public:
	PROJECTION_TYPE _type = PROJECTION_TYPE::PERSPECTIVE;

	float _near = 1.f;
	float _far = 1000.f;
	float _fov = XM_PI / 4.f; // 카메라가 찍는 각도 ( 필드 오브 뷰 )
	float _scale = 1.f;

	Matrix _matView = {};
	Matrix _matProjection = {};

public:
	// TEMP
	static Matrix S_MatView;
	static Matrix S_MatProjection;
};

