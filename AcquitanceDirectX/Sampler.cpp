#include "Sampler.h"
#include "GraphicsThrowHeader.h"
namespace Bind {
	Sampler::Sampler(Graphics& gfx)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC sam_des = {};
		sam_des.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sam_des.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sam_des.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sam_des.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&sam_des, &pSamplerState));

	}

	void Sampler::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetSamplers(0u, 1u, pSamplerState.GetAddressOf());
	}
}