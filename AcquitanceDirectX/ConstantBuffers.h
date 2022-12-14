#pragma once
#include "Bindable.h"
#include "GraphicsThrowHeader.h"
#include "BindableCodex.h"

namespace Bind {
	template<typename C>
	class ConstantBuffer : public Bindable
	{
	public:
		void Update(Graphics& gfx, const C& consts)
		{
			INFOMAN(gfx);

			D3D11_MAPPED_SUBRESOURCE msr;
			GFX_THROW_INFO(GetContext(gfx)->Map(
				pConstantBuffer.Get(), 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			));
			memcpy(msr.pData, &consts, sizeof(consts));
			GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
		}
		ConstantBuffer(Graphics& gfx, const C& consts, UINT slot_in = 0)
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(consts);
			cbd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &consts;
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
			slot = slot_in;
		}
		ConstantBuffer(Graphics& gfx, UINT slot_in = 0)
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(C);
			cbd.StructureByteStride = 0u;
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
			slot = slot_in;
		}
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		UINT slot;
	};

	// class template inheritance

	template<typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using Bindable::GetContext;
		using ConstantBuffer<C>::slot;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}
		static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx,const C& consts, UINT slot_in = 0)
		{
			return Codex::Resolve<VertexConstantBuffer>(gfx, consts, slot_in );
		}
		static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx, UINT slot_in = 0)
		{
			return Codex::Resolve<VertexConstantBuffer>(gfx,  slot_in );
		}
		static std::string GenerateUID(const C& consts, UINT slot_in = 0)
		{
			return GenerateUID(slot);
		}
		static std::string GenerateUID(UINT slot_in = 0)
		{
			return typeid(VertexConstantBuffer).name() + std::to_string(slot_in);
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID(slot);
		}
	};

	template<typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using Bindable::GetContext;
		using ConstantBuffer<C>::slot;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}

		static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, const C& consts, UINT slot_in = 0)
		{
			return Codex::Resolve<PixelConstantBuffer>(gfx,  consts,  slot_in);
		}
		static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, UINT slot_in = 0)
		{
			return Codex::Resolve<PixelConstantBuffer>(gfx,  slot_in );
		}
		static std::string GenerateUID(const C& consts, UINT slot_in = 0)
		{
			return GenerateUID(slot_in);
		}
		static std::string GenerateUID(UINT slot_in = 0)
		{
			return typeid(PixelConstantBuffer).name() + std::to_string(slot_in);
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID(slot);
		}
	};
}