#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>
#include <memory>

namespace Bind {
	class TransformCbuf : public CloningBindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX modle;
			DirectX::XMMATRIX modelViewProj;
		};
	public:
		virtual Transforms GenerateTransform(Graphics& gfx) noexcept;
		virtual void UpdateAndBind(Graphics& gfx, const Transforms& tf) noexcept;

		TransformCbuf(Graphics& gfx, UINT slot = 0);
		virtual void Bind(Graphics& gfx) noexcept override;

		std::unique_ptr<CloningBindable> Clone() const noexcept override
		{
			return std::make_unique<TransformCbuf>(*this);
		}

		void InitializeParentReference(const Drawable& drawable) noexcept override;
		
		Transforms GetTransforms(Graphics& gfx) noexcept
		{
			assert(parent != nullptr);
			DirectX::XMMATRIX modelView = parent->GetTransformXM() * gfx.GetCamera();
			return {
				DirectX::XMMatrixTranspose(modelView),
				DirectX::XMMatrixTranspose(
					modelView *
					gfx.GetProjection()
				)
			};
		}
		
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		const Drawable* parent;
	};
}
