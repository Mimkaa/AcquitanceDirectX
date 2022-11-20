#include "TransformCbufDoubleBoi.h"
namespace Bind {
	TransformCbufDoubleBoi::TransformCbufDoubleBoi(Graphics& gfx, const Drawable& parent, UINT slot , UINT slotp )
		:
		TransformCbuf(gfx, parent, slot)
	{
		pPcbuf = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotp);
	}
	void TransformCbufDoubleBoi::UpdateAndBind(Graphics& gfx) noexcept
	{
		TransformCbuf::UpdateAndBind(gfx);
		pPcbuf->Update(gfx, GenerateTransform(gfx));
		pPcbuf->Bind(gfx);
	}
	std::unique_ptr<PixelConstantBuffer<TransformCbufDoubleBoi::Transforms>> TransformCbufDoubleBoi::pPcbuf;
}