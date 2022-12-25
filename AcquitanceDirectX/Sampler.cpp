#include "Sampler.h"
#include "GraphicsThrowHeader.h"
#include "BindableCodex.h"
namespace Bind {
	Sampler::Sampler(Graphics& gfx)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC sam_des = {};
		sam_des.Filter = D3D11_FILTER_ANISOTROPIC;
		sam_des.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sam_des.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sam_des.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sam_des.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		sam_des.MipLODBias = 0.0f;
		sam_des.MinLOD = 0.0f;
		sam_des.MaxLOD = D3D11_FLOAT32_MAX;

		GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&sam_des, &pSamplerState));

	}

	void Sampler::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetSamplers(0u, 1u, pSamplerState.GetAddressOf());
	}

	std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx)
	{
		return Codex::Resolve<Sampler>(gfx);

	}

	std::string Sampler::GenerateUID()
	{
		return typeid(Sampler).name();
	}
	std::string Sampler::GetUID() const  noexcept
	{

		return GenerateUID();
	}
}