#pragma once
#include "Bindable.h"
#include <string>
namespace Bind {
	class VertexShader : public Bindable
	{
	public:
		VertexShader(Graphics& gfx, const std::string& path);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<VertexShader> Resolve(Graphics& gfx, const std::string& path);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID() const noexcept override;
		ID3DBlob* GetBytecode() const noexcept;
	protected:
		Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		std::string path;
	};
}
