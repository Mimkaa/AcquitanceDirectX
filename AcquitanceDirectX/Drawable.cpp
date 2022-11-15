#include "Drawable.h"
#include "GraphicsThrowHeader.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>
using namespace Bind;

void Drawable::Draw(Graphics& gfx) const noxnd
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::shared_ptr<Bindable> bind) noxnd
{
	
	if (typeid(*bind) == typeid(IndexBuffer))
	{
		assert("*Must* use only one index buffer" && pIndexBuffer == nullptr);
		pIndexBuffer = &static_cast<IndexBuffer&>(*bind);
	}
	binds.push_back(std::move(bind));
}

