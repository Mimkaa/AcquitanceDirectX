#pragma once
#include "Bindable.h"
#include "Surface.h"
namespace Bind {
	class Texture :public Bindable
	{
	public:
		Texture(Graphics& gfx, const std::string& path, UINT slot = 0);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<Texture> Resolve(Graphics& gfx, const std::string& path, UINT slot = 0 );
		static std::string GenerateUID(const std::string& path, UINT slot = 0);
		std::string GetUID() const noexcept override;
		bool HasAlpha() const noexcept;
		
	private:
		unsigned int slot;
		std::string path;
	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		bool hasAlphaSet;
	};
}