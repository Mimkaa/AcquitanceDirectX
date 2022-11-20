#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>
namespace Bind {
	class TransformCbuf : public Bindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX modelViewProj;
			DirectX::XMMATRIX modle;
		};
	public:
		virtual Transforms GenerateTransform(Graphics& gfx) noexcept;
		virtual void UpdateAndBind(Graphics& gfx) noexcept;

		TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot = 0);
		void Bind(Graphics& gfx) noexcept override;
		
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		const Drawable& parent;
	};
}
