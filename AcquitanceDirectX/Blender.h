#pragma once
#include "Bindable.h"
#include <optional>
#include <array>
namespace Bind {
	class Blender :public Bindable
	{
	public:
		Blender(Graphics& gfx, bool hasAlpha, std::optional<float> factors_in = {});
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<Blender> Resolve(Graphics& gfx, bool hasAlpha, std::optional<float> factors_in = {});
		static std::string GenerateUID(const bool hasAlpha, std::optional<float> factors_in = {});
		std::string GetUID() const noexcept override;
		void SetFactor(float factor) noexcept;
		float GetFactor()noexcept;

	private:
		bool hasAlpha;
		Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
		std::optional<std::array<float, 4>> factors;
	};
}
