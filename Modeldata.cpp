#include "Modeldata.h"

Model_Data::Model_Data()
{
	


	Vertices = {
		{ { 0.25f, 0.25f, 0.0f }, {0.0f,0.0f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { -0.25f, 0.25f, 0.0f },{0.0f,0.0f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.25f, -0.25f, 0.0f },{0.0f,0.0f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f, 0.0f },{0.0f,0.0f,0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}, { 0.0f, 0.0f, 1.0f, 1.0f } } };
	Index = {
		2,1,0,
		1,2,3
	};
	VertexBufferSize = Vertices.size() * sizeof(Vertex);
	IndexBufferSize = Index.size() * sizeof(UINT);
}

Model_Data::~Model_Data()
{
}

void Model_Data::CopyVertices(Vertex* pVertexDataBegin)
{
	std::copy(std::begin(Vertices), std::end(Vertices), pVertexDataBegin);
	return;
}

void Model_Data::CopyIndex(UINT* pIndexDataBegin)
{
	std::copy(std::begin(Index), std::end(Index), pIndexDataBegin);
	return;
}

ModelLoader::ModelLoader() 
{
}

ModelLoader::~ModelLoader() 
{
}