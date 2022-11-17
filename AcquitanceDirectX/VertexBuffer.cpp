#include "VertexBuffer.h"
#include "BindableCodex.h"
namespace Bind {
	VertexBuffer::VertexBuffer(Graphics& gfx, const Dvtx::VertexBuffer& vb)
		:
		VertexBuffer(gfx,"?",vb)
	{}

	VertexBuffer::VertexBuffer(Graphics& gfx, std::string tag, const Dvtx::VertexBuffer& vb)
		:
		stride(vb.GetLayout().Size()),
		tag(tag)
	{
		INFOMAN(gfx);
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(vb.SizeBytes());
		bd.StructureByteStride = stride;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vb.GetData();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
	}

	void VertexBuffer::Bind(Graphics& gfx) noexcept
	{
		const UINT offset = 0u;
		GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	}
	std::shared_ptr<Bindable> VertexBuffer::Resolve(Graphics& gfx, const std::string& tag_in, const Dvtx::VertexBuffer& vb)
	{
		return Codex::Resolve<VertexBuffer>(gfx, tag_in, vb);
	}
	std::string VertexBuffer::GenerateUID_(const std::string& tag_in)
	{
		using namespace std::string_literals;
		return typeid(VertexBuffer).name() + "#"s + tag_in;
	}

	std::string VertexBuffer::GetUID() const noexcept
	{
		return GenerateUID(tag);
	}

}