#pragma once
#include "Bindable.h"
namespace Bind {
	class Rasterizer :public Bindable
	{
	public:
		Rasterizer(Graphics& gfx, bool twoSided);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<Rasterizer> Resolve(Graphics& gfx, bool  twoSided);
		static std::string GenerateUID(const bool twoSided);
		std::string GetUID() const noexcept override;

	private:
		bool twoSided;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
	};
}