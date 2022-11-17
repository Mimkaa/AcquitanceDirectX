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
		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx, const Dvtx::VertexLayout& VLayout,
			ID3DBlob* pVertexShaderBytecode);
		template <typename ... Ignore>
		static std::string GenerateUID(const Dvtx::VertexLayout& layout_in, Ignore&&... ignore)
		{
			return GenerateUID_(layout_in);
		}
		std::string GetUID() const noexcept override;
		void Bind(Graphics& gfx) noexcept override;
	private:
		static std::string GenerateUID_(const Dvtx::VertexLayout& layout_in);
	protected:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
		Dvtx::VertexLayout layout;
	};
}