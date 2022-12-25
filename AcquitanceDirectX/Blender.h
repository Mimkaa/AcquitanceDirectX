#pragma once
#include "Bindable.h"
namespace Bind {
	class Blender :public Bindable
	{
	public:
		Blender(Graphics& gfx, bool hasAlpha);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<Blender> Resolve(Graphics& gfx, bool hasAlpha);
		static std::string GenerateUID(const bool hasAlpha);
		std::string GetUID() const noexcept override;

	private:
		bool hasAlpha;
		Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
	};
}
