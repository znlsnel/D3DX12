#include "pch.h"
#include "Game.h"
#include "Engine.h"

shared_ptr<Mesh> mesh = make_shared<Mesh>();
shared_ptr<Shader> shader = make_shared<Shader>();

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
	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vec[3].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 1.f, 0.f, 1.f);

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

	shader->Init(L"..\\Resources\\Shader\\default.hlsli");

	GEngine->GetCmdQueue()->WaitSync();
}

void Game::Update()
{
	GEngine->RenderBegin();

	shader->Update();

	{
		Transform t;
		t.offset = Vec4(0.f, 0.f, 0.f, 0.f);
		mesh->SetTransform(t);

		mesh->Render();
	}

	/* {
		Transform t;
		t.offset = Vec4(0.f, 0.75f, 0.0f, 0.f);
		mesh->SetTransform(t);

		mesh->Render();
	}*/


	GEngine->RenderEnd();


}
