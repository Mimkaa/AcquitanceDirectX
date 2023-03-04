#pragma once
class Graphics;
class BufferResource
{
public:
	virtual void Clear(Graphics& gfx)const noexcept = 0;

};