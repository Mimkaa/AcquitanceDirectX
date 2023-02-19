#include "Sampler.h"
#include "GraphicsThrowHeader.h"
#include "BindableCodex.h"
namespace Bind {
	Sampler::Sampler(Graphics& gfx, bool anisotropic, bool reflect)
		:
		EnableAnisotropic(anisotropic),
		reflectionMode(reflect)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC sam_des = {};
		sam_des.Filter = EnableAnisotropic? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_POINT;
		sam_des.AddressU = reflectionMode ? D3D11_TEXTURE_ADDRESS_WRAP: D3D11_TEXTURE_ADDRESS_MIRROR;
		sam_des.AddressV = reflectionMode ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_MIRROR;
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

	std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx, bool anisotropic , bool reflect)
	{
		return Codex::Resolve<Sampler>(gfx, anisotropic, reflect);

	}

	std::string Sampler::GenerateUID(bool anisotropic, bool reflect)
	{
		using namespace std::string_literals;
		return typeid(Sampler).name() + (reflect? "R"s:"noe"s) + (anisotropic? "A"s:"noe"s);
	}
	std::string Sampler::GetUID() const  noexcept
	{

		return GenerateUID(EnableAnisotropic, reflectionMode);
	}
}