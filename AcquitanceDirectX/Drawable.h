#pragma once
#include "Graphics.hpp"
#include <DirectXMath.h>
#include <memory>

namespace Bind
{
	class Bindable;
	class IndexBuffer;
}

class Drawable
{
	
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noxnd;
	virtual ~Drawable() = default;
	
	template <class T>
	T* QueryBindables()
	{
		for (auto& b : binds)
		{
			if (auto ptr = dynamic_cast<T*>(b.get()))
			{
				return ptr;
			}
		}
		return nullptr;
	}
protected:
	void AddBind(std::shared_ptr<Bind::Bindable> bind) noxnd;


private:
	const Bind::IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bind::Bindable>> binds;
};
