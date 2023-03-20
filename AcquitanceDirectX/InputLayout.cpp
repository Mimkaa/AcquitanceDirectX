#include "InputLayout.h"
#include "GraphicsThrowHeader.h"
#include "BindableCodex.h"
#include "VertexShader.h"

namespace Bind {
	InputLayout::InputLayout(Graphics& gfx,
		const Dvtx::VertexLayout& VLayout,
		const VertexShader& vs)
		:
		layout(VLayout),
		gfx_ref(gfx),
		shaderUID(vs.GetUID())

	{
		INFOMAN(gfx);
		auto layout = VLayout.GetD3DLayout();
		GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
			layout.data(), (UINT)layout.size(),
			vs.GetBytecode()->GetBufferPointer(),
			vs.GetBytecode()->GetBufferSize(),
			&pInputLayout
		));
	}
	std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx,const Dvtx::VertexLayout& VLayout,
		const VertexShader& vs)
	{
		return Codex::Resolve<InputLayout>(gfx, VLayout, vs);
	}
	std::string InputLayout::GenerateUID_(const Dvtx::VertexLayout& layout_in, const VertexShader& vs)
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout_in.GetCode() + "#"s + vs.GetUID();
	}
	std::string InputLayout::GetUID() const noexcept
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode() + "#"s + shaderUID;
	}

	void InputLayout::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
	}
}