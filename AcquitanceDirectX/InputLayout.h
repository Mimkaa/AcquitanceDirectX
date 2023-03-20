#pragma once
#include "Bindable.h"
#include "Vertex.h"



namespace Bind {
	class VertexShader;
	class InputLayout : public Bindable
	{
	public:
		InputLayout(Graphics& gfx,
			const Dvtx::VertexLayout& VLayout,
			const VertexShader& vs);
		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx, const Dvtx::VertexLayout& VLayout,
			const VertexShader& vs);
		template <typename ... Ignore>
		static std::string GenerateUID(const Dvtx::VertexLayout& layout_in, const VertexShader& vs, Ignore&&... ignore)
		{
			return GenerateUID_(layout_in, vs);
		}
		std::string GetUID() const noexcept override;
		void Bind(Graphics& gfx) noexcept override;
	private:
		static std::string GenerateUID_(const Dvtx::VertexLayout& layout_in, const VertexShader& vs);
	protected:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
		Dvtx::VertexLayout layout;
		Graphics& gfx_ref;
		std::string shaderUID;

	};
}