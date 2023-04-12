#pragma once
#include "Bindable.h"
#include "GraphicsThrowHeader.h"
#include "Vertex.h"
namespace Bind {
	class VertexBuffer : public Bindable
	{
	public:

		
		VertexBuffer(Graphics& gfx, const Dvtx::VertexBuffer& vb);
		VertexBuffer(Graphics& gfx, std::string tag, const Dvtx::VertexBuffer& vb);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<VertexBuffer> Resolve(Graphics& gfx, const std::string& tag_in, const Dvtx::VertexBuffer& vb);
		template<typename ...Ignore>
		static std::string GenerateUID(const std::string& tag_in, Ignore&&... ignore)
		{
			return GenerateUID_(tag_in);
		}
		std::string GetUID() const noexcept override;
		const Dvtx::VertexLayout& GetLayout() const
		{
			return layout;
		}

	private:
		static std::string GenerateUID_(const std::string& tag_in);
	protected:
		Dvtx::VertexLayout layout;
		UINT stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		std::string tag;
	};
}