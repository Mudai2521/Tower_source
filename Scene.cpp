#include "Scene.h"
#include "Win32Application.h"
#include "D3D12Application.h"
#include <d3dx12.h>
#include "VertexBuffer.h"

Scene* g_Scene;

using namespace DirectX;

VertexBuffer* vertexbuffer;

bool Scene::Init()
{
	Model_Data modeldata(g_D3DApp->GetaspectRatio());

	const UINT vertexBufferSize = modeldata.GetVertexBufferSize();

	vertexbuffer = new VertexBuffer(vertexBufferSize, sizeof(Vertex), vertices);

	if (!vertexBuffer->IsValid())
	{
		
		return false;
	}
}

void Scene::Update()
{
}

void Scene::Draw()
{
}
