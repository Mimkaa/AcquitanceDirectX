#include "TransformCbuf.h"
namespace Bind {
	TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot)
		:
		parent(parent)
	{
		if (!pVcbuf)
		{
			pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}
	
	TransformCbuf::Transforms TransformCbuf::GenerateTransform(Graphics& gfx) noexcept
	{
		auto modelView = parent.GetTransformXM() * gfx.GetCamera();
		return { DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(modelView *
				gfx.GetProjection())
		};

	}


	void TransformCbuf::UpdateAndBind(Graphics& gfx) noexcept
	{
		pVcbuf->Update(gfx, GenerateTransform(gfx));
		pVcbuf->Bind(gfx);
	}
	
	void TransformCbuf::Bind(Graphics& gfx) noexcept
	{
		UpdateAndBind(gfx);
	}
	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;
}