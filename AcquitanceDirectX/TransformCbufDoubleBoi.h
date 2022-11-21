#include "Bindable.h"
#include "TransformCbuf.h"
#pragma once
namespace Bind {
	class TransformCbufDoubleBoi : public TransformCbuf
	{
	public:
		TransformCbufDoubleBoi(Graphics& gfx, const Drawable& parent, UINT slot = 0, UINT slotp = 0);
		
		void UpdateAndBind(Graphics& gfx, const Transforms& tf) noexcept;
		void Bind(Graphics& gfx) noexcept override;
	private:
		static std::unique_ptr<PixelConstantBuffer<Transforms>> pPcbuf;
	};

}