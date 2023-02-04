//#include "TransformCbufDoubleBoi.h"
//namespace Bind {
//	TransformCbufDoubleBoi::TransformCbufDoubleBoi(Graphics& gfx, const Drawable& parent, UINT slot , UINT slotp )
//		:
//		TransformCbuf(gfx, parent, slot)
//	{
//		pPcbuf = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotp);
//	}
//	
//	void TransformCbufDoubleBoi::UpdateAndBind(Graphics& gfx, const Transforms& tf) noexcept
//	{
//		
//		pPcbuf->Update(gfx, tf);
//		pPcbuf->Bind(gfx);
//	}
//	void TransformCbufDoubleBoi::Bind(Graphics& gfx) noexcept
//	{
//		const auto tf = GenerateTransform(gfx);
//		TransformCbuf::UpdateAndBind(gfx, tf);
//		UpdateAndBind(gfx, tf);
//	}
//	std::unique_ptr<PixelConstantBuffer<TransformCbufDoubleBoi::Transforms>> TransformCbufDoubleBoi::pPcbuf;
//}