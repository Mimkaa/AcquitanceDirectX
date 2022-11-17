#include "InputLayout.h"
#include "GraphicsThrowHeader.h"
#include "BindableCodex.h"
namespace Bind {
	InputLayout::InputLayout(Graphics& gfx,
		const Dvtx::VertexLayout& VLayout,
		ID3DBlob* pVertexShaderBytecode)
		:
		layout(VLayout)
	{
		INFOMAN(gfx);
		auto layout = VLayout.GetD3DLayout();
		GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
			layout.data(), (UINT)layout.size(),
			pVertexShaderBytecode->GetBufferPointer(),
			pVertexShaderBytecode->GetBufferSize(),
			&pInputLayout
		));
	}
	std::shared_ptr<Bindable> InputLayout::Resolve(Graphics& gfx)
	{
		return Codex::Resolve<InputLayout>(gfx);
	}
	std::string InputLayout::GenerateUID(const Dvtx::VertexLayout& layout_in)
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout_in.GetCode();
	}
	std::string InputLayout::GetUID() const
	{
		return GenerateUID(layout);
	}

	void InputLayout::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
	}
}