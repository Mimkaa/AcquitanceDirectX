#include "ShadowSampler.h"

#include "ShadowSampler.h"

#include "BindableCodex.h"

namespace Bind
{
	ShadowSampler::ShadowSampler(Graphics& gfx)
	{

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };

		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;

		GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSampler);
	}

	void ShadowSampler::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetSamplers(1, 1, pSampler.GetAddressOf());
	}
}