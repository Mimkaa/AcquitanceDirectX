#include "Testing.h"
#include "DynamicConstantBuffer.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "Vertex.h"
#include "Material.h"
#include "Mesh.h"

void TestDynamicVertexBuff()
{
	using namespace std::string_literals;
	using namespace Dvtx;
	Assimp::Importer importer;
	const auto scene = importer.ReadFile("Models\\gobber\\GoblinX.obj"s.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	VertexLayout lay{};
	lay.Append(VertexLayout::Position2D)
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Tangent)
		.Append(VertexLayout::Bytangent)
		.Append(VertexLayout::Texture2D);

	
	VertexBuffer bf{ std::move(lay),scene->mNumMeshes};

	for (int end = bf.Size(), i = 0; i < end; i++)
	{
		auto a = bf[i].Attr<VertexLayout::Position2D>();
		auto b = bf[i].Attr<VertexLayout::Position3D>();
		auto c = bf[i].Attr<VertexLayout::Normal>();
		auto d = bf[i].Attr<VertexLayout::Tangent>();
		auto e = bf[i].Attr<VertexLayout::Bytangent>();
		auto f = bf[i].Attr<VertexLayout::Texture2D>();

	}
	
	

}

void TestDynamicConstantBuff()
{
	using namespace Dcb;
	RawLayout row{};
	row.Add<Type::Float>("test");
	row.Add<Type::Array>("arr");
	row.Add<Type::Array>("arr2");

	row["arr"].Set<Type::Struct>(10);
	row["arr"].T().Add<Type::Float>("arrTest1");

	row["arr2"].Set<Type::Array>(1);
	row["arr2"].T().Set<Type::Struct>(30);
	row["arr2"].T().T().Add<Type::Float3>("ultimate");
	


	Buffer buf(std::move(row));
	auto exp = DirectX::XMFLOAT3{ 69.0f,0.0f,0.0f };
	buf["test"] = 5.5f;
	buf["arr"][0]["arrTest1"] = 6.9f;
	buf["arr"][9]["arrTest1"] = 99.9f;
	buf["arr2"][0][29]["ultimate"] = exp;
	
	float res1 = buf["test"];
	float res2 = buf["arr"][0]["arrTest1"];
	float res3 = buf["arr"][9]["arrTest1"];
	dx::XMFLOAT3 res4 = buf["arr2"][0][29]["ultimate"];
	
	int a = 18;
	



}


void TestMaterialSystemLoading(Graphics& gfx)
{
	std::string path = "Models\\brick_wall\\brick_wall.obj";
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);
	Material mat{ gfx,*pScene->mMaterials[1],path };
	Mesh mesh{ gfx,mat,*pScene->mMeshes[0] };
}