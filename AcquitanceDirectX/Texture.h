#pragma once
#include "Bindable.h"
#include "Surface.h"
namespace Bind {
	class Texture :public Bindable
	{
	public:
		Texture(Graphics& gfx, const Surface& surf, unsigned int slot = 0);
		void Bind(Graphics& gfx) noexcept override;
	private:
		unsigned int slot;
	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	};
}