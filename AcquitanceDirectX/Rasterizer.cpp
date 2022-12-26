#include "Rasterizer.h"
#include "GraphicsThrowHeader.h"
#include "BindableCodex.h"
#include <memory>
namespace Bind
{
	Rasterizer::Rasterizer(Graphics& gfx, bool twoSided)
		:
		twoSided(twoSided)
	{
		INFOMAN(gfx);

		D3D11_RASTERIZER_DESC rastDesc = { CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{}) };
		rastDesc.CullMode = twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;
		GFX_THROW_INFO(GetDevice(gfx)->CreateRasterizerState(&rastDesc, &pRasterizer));
	}
	void Rasterizer::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->RSSetState(pRasterizer.Get());
	}
	std::shared_ptr<Rasterizer> Rasterizer::Resolve(Graphics& gfx, bool twoSided)
	{
		return Codex::Resolve<Rasterizer>(gfx, twoSided);
	}
	std::string Rasterizer::GenerateUID(const bool twoSided)
	{
		using namespace std::string_literals;
		return typeid(Rasterizer).name() + "#"s + (twoSided ? "twoSided" : "NotTwoSided");
	}
	std::string Rasterizer::GetUID() const noexcept
	{
		return GenerateUID(twoSided);
	}
}