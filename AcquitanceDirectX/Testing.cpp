#include "Testing.h"
#include "DynamicConstantBuffer.h"
void TestDynamicConstantBuff()
{
	using namespace Dcbf;
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