#pragma once
#include <string>
#include "Bindable.h"

namespace Bind {
	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
		IndexBuffer(Graphics& gfx, const std::string& tag,
			const std::vector<unsigned short>& indices);
		void Bind(Graphics& gfx) noexcept override;
		UINT GetCount() const noexcept;
		static std::shared_ptr<IndexBuffer> Resolve(Graphics& gfx, const std::string& tag_in, const std::vector<unsigned short>& indices);
		template<typename ...Ignore>
		static std::string GenerateUID(const std::string& tag_in, Ignore&&... ignore)
		{
			return GenerateUID_(tag_in);
		}
		std::string GetUID() const noexcept override;
	private:
		static std::string GenerateUID_(const std::string& tag_in);
	protected:
		UINT count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
		std::string tag;
	};
}