#include "TransformCbuf.h"

namespace Bind {
	TransformCbuf::TransformCbuf(Graphics& gfx,  UINT slot)
	{
		if (!pVcbuf)
		{
			pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}
	
	TransformCbuf::Transforms TransformCbuf::GenerateTransform(Graphics& gfx) noexcept
	{
		auto modelView = parent->GetTransformXM() * gfx.GetCamera();
		return { DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(modelView *
				gfx.GetProjection()),
			DirectX::XMMatrixTranspose(parent->GetTransformXM())
		};

	}
	void TransformCbuf::InitializeParentReference(const Drawable& drawable) noexcept
	{
		parent = &drawable;
	}




	void TransformCbuf::UpdateAndBind(Graphics& gfx, const Transforms& tf) noexcept
	{
		pVcbuf->Update(gfx, tf);
		pVcbuf->Bind(gfx);
	}
	
	void TransformCbuf::Bind(Graphics& gfx) noexcept
	{
		UpdateAndBind(gfx, GenerateTransform(gfx));
	}
	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;
}