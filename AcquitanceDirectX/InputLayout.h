#pragma once
#include "Bindable.h"
#include "Vertex.h"
namespace Bind {
	class InputLayout : public Bindable
	{
	public:
		InputLayout(Graphics& gfx,
			const Dvtx::VertexLayout& VLayout,
			ID3DBlob* pVertexShaderBytecode);
		static std::shared_ptr<Bindable> Resolve(Graphics& gfx);
		static std::string GenerateUID(const Dvtx::VertexLayout& layout_in);
		std::string GetUID() const noexcept override;
		void Bind(Graphics& gfx) noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
		Dvtx::VertexLayout layout;
	};
}