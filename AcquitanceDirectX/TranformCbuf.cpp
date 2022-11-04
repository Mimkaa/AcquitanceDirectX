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

	void TransformCbuf::Bind(Graphics& gfx) noexcept
	{
		auto modelView = parent.GetTransformXM() * gfx.GetCamera();
		Transforms tf =
		{ DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(modelView *
				gfx.GetProjection())
		};
		pVcbuf->Update(gfx, tf);
		pVcbuf->Bind(gfx);
	}
	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;
}