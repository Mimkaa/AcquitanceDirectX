#include "Blender.h"
#include "GraphicsThrowHeader.h"
#include "BindableCodex.h"
#include <string> 
namespace Bind
{
	Blender::Blender(Graphics& gfx, bool hasAlpha, std::optional<float> factors_in)
		:
		hasAlpha(hasAlpha)
	{
		INFOMAN(gfx);
		// by factors I mean that one factor will be replicated by fill function
		if (factors_in)
		{
			factors.emplace();
			factors->fill(*factors_in);
		}

		
		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
		auto& brt = blendDesc.RenderTarget[0];
		if (hasAlpha)
		{
			brt.BlendEnable = TRUE;
			if (factors_in)
			{
				brt.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
				brt.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
			}
			else
			{
				brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			}
			
		}
		
		GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&blendDesc, &pBlendState));
	}
	void Blender::SetFactor(float factor) noexcept
	{
		assert(factors);
		factors->fill(factor);
	}

	float Blender::GetFactor()noexcept
	{
		assert(factors);
		return factors->front();
	}


	void Blender::Bind(Graphics& gfx) noexcept
	{
		//extract data from our factors
		const float* data = factors ? factors->data() : nullptr;
		GetContext(gfx)->OMSetBlendState(pBlendState.Get(), data, 0xFFFFFFFFu);
		
	}
	std::shared_ptr<Blender> Blender::Resolve(Graphics& gfx, bool hasAlpha, std::optional<float> factors_in)
	{
		return Codex::Resolve<Blender>(gfx, hasAlpha, factors_in);
	}
	std::string Blender::GenerateUID(const bool hasAlpha, std::optional<float> factors_in)
	{
		using namespace std::string_literals;
		return typeid(Blender).name() + "#"s + (hasAlpha? "hasAlpha" : "hasNotAlpha" + "#"s + (!factors_in?"nof":std::to_string(*factors_in)));
	}
	std::string Blender::GetUID() const noexcept
	{
		return GenerateUID(hasAlpha, factors ? factors->front() : std::optional<float>{});
	}
}