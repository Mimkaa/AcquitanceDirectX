#pragma once
#include "Pass.h"


class ClearBufferPass :public Pass
{
public:
	ClearBufferPass(const std::string& name) :
		Pass(std::move(name))
	{
		RegisterSink(DirectPassSink<BufferResource>::Make("buffer", buffer));
		RegisterSource(DirectPassSource<BufferResource>::Make("buffer", buffer));

	}
	void Execute(Graphics& gfx)
	{
		buffer->Clear(gfx);
	}
private:
	std::shared_ptr<BufferResource> buffer;

};