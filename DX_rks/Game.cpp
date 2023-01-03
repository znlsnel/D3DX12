#include "pch.h"
#include "Game.h"
#include "Engine.h"

shared_ptr<Mesh> mesh = make_shared<Mesh>();
shared_ptr<Shader> shader = make_shared<Shader>();
shared_ptr<Texture> texture = make_shared<Texture>();

void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

	// �簢�� ����� ( �ﰢ�� 2���� )

	
#pragma region �簢�� ( �ﰢ�� 2�� )
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

	// ���Ľ�
	// ���� �ո� �ǿ��� ���� ��ĥ�ϴ°�
	// Stencil�̶�� �ϸ� ���� ���� ���ٽ� ���� �������༭
	// ���ٽ� ���� ���� �ֵ鳢�� ��� ��ĥ�ϴ°�

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

	texture->Init(L"..\\Resources\\Texture\\test.jpg");

	GEngine->GetCmdQueue()->WaitSync();
}

void Game::Update()
{
	GEngine->RenderBegin();

	shader->Update();


	{
		Transform t;
		t.offset = Vec4(0.25f, 0.25f, 0.2f, 0.f);
		mesh->SetTransform(t);

		mesh->SetTexture(texture);

		mesh->Render();
	}

	{
		Transform t;
		t.offset = Vec4(0.f, 0.f, 0.3f, 0.f);
		mesh->SetTransform(t);

		mesh->SetTexture(texture);

		mesh->Render();
	}






	GEngine->RenderEnd();


}
