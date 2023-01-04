#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "Material.h"

shared_ptr<Mesh> mesh = make_shared<Mesh>();


void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

	// 사각형 만들기 ( 삼각형 2개로 )

	
#pragma region 사각형 ( 삼각형 2개 )
	//vector<Vertex> vec(6);
		//{
		//	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
		//	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f); // R - 100

		//	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
		//	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f); // G - 100

		//	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
		//	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f); // B - 100
		//}
		//{
		//	vec[3].pos = Vec3(0.5f, -0.5f, 0.5f);
		//	vec[3].color = Vec4(0.f, 0.f, 1.f, 1.f); // B - 100

		//	vec[4].pos = Vec3(-0.5f, -0.5f, 0.5f);
		//	vec[4].color = Vec4(0.f, 1.f, 0.f, 1.f); // G - 100

		//	vec[5].pos = Vec3(-0.5f, 0.5f, 0.5f);
		//	vec[5].color = Vec4(1.f, 0.f, 0.f, 1.f); // R - 100
		//}

#pragma endregion

	vector<Vertex> vec(4);
	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[0].uv = Vec2(0.f, 0.f);

	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[1].uv = Vec2(1.f, 0.f);

	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vec[2].uv = Vec2(1.f, 1.f);

	vec[3].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[3].uv = Vec2(0.f, 1.f);

	// 스탠실
	// 구멍 뚫린 판에다 데고 색칠하는거
	// Stencil이라고 하면 색상에 따라 스텐실 값을 지정해줘서
	// 스텐실 값이 같은 애들끼리 묶어서 색칠하는거

	vector<uint32> indexVec;
	{
		indexVec.push_back(0);
		indexVec.push_back(1);
		indexVec.push_back(2);
	}
	{
		indexVec.push_back(0);
		indexVec.push_back(2);
		indexVec.push_back(3);
	}
	mesh->Init(vec, indexVec);

	shared_ptr<Shader> shader = make_shared<Shader>();
	shared_ptr<Texture> texture = make_shared<Texture>();

	shader->Init(L"..\\Resources\\Shader\\default.hlsli");

	texture->Init(L"..\\Resources\\Texture\\test.jpg");

	shared_ptr<Material> material = make_shared<Material>();
	material->SetShader(shader);
	material->SetFloat(0, 0.3f);
	material->SetFloat(1, 0.4f);
	material->SetFloat(2, 0.3f);
	material->SetTexture(0, texture);
	mesh->SetMaterial(material);

	GEngine->GetCmdQueue()->WaitSync();
}

void Game::Update()
{
	GEngine->Update();
	GEngine->RenderBegin();


	{
		static Transform t = {};

		if (INPUT->GetButton(KEY_TYPE::W))
			t.offset.y += 1.f * DELTA_TIME;
		if (INPUT->GetButton(KEY_TYPE::S))
			t.offset.y -= 1.f * DELTA_TIME;
		if (INPUT->GetButton(KEY_TYPE::A))
			t.offset.x -= 1.f * DELTA_TIME;
		if (INPUT->GetButton(KEY_TYPE::D))
			t.offset.x += 1.f * DELTA_TIME;

		mesh->SetTransform(t);


		mesh->Render();
	}


	GEngine->RenderEnd();


}
