#pragma once
#include "Component.h"

class Transform;
class MeshRenderer;
class MonoBehaviour;

// 자기 자신에 대한 weak_ptr를 하나 들고있는 상태를 뜻함
class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	virtual ~GameObject();

	void Init();

	void Awake();
	void Start();
	void Update();
	void LateUpdate();

	shared_ptr<Transform> GetTransform();

	void AddComponent(shared_ptr<Component> component);




private:
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
	vector<shared_ptr<MonoBehaviour>> _scripts;
};

