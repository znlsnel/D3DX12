#pragma once
#include "MonoBehaviour.h"

class TestCamera : public MonoBehaviour
{
public:
	TestCamera();
	virtual ~TestCamera();

	virtual void LateUpdate() override;

private:
	float		_speed = 100.f;
};

