#pragma once
class Graphics;
class BufferResource
{
public:
	BufferResource() = default;
	virtual void Clear(Graphics& gfx) const noexcept = 0;
	                         
};