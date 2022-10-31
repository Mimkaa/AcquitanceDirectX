#pragma once
#include "Graphics.hpp"
#include <DirectXMath.h>
#include <memory>

class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	virtual void Update(float dt) noexcept {};
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
	void AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;
	
private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};
