#pragma once

#include "BindableCodex.h"
#include "Bindable.h"

namespace Bind
{
	class NullPixelShader : public Bind::Bindable
	{
	public:
		NullPixelShader(Graphics& gfx)
		{
		}
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->PSSetShader(nullptr, nullptr, 0u);
		}
		static std::shared_ptr<NullPixelShader> Resolve(Graphics& gfx)
		{
			return Codex::Resolve<NullPixelShader>(gfx);
		}
		static std::string GenerateUID() 
		{
			return typeid(NullPixelShader).name();
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID();
		}
	};
}