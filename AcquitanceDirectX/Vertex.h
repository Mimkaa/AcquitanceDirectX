#pragma once
#include <DirectXMath.h>
#include <type_traits>
#include <vector>
#include "Color.h"
#include <dxgiformat.h>
#include <d3d11.h>
#include "ConditionalNoexcept.h"
#include <string>
namespace Dvtx {

	class VertexLayout
	{
		
	public:
		enum ElementType
		{
			Position2D,
			Position3D,
			Texture2D,
			Normal,
			Float3Color,
			Float4Color,
			BGRAColor,
			Count
		};
		template<ElementType> struct Map;
		template<> struct Map<Position2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P2";
		};
		template<> struct Map<Position3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P3";
		};
		template<> struct Map<Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
			static constexpr const char* code = "T2";
		};
		template<> struct Map<Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "N3";
		};
		template<> struct Map<Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C3";
		};
		template<> struct Map<Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C4";
		};
		template<> struct Map<BGRAColor>
		{
			using SysType = ::BGRAColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "BGR4";
		};
		class Element
		{
		public:
			Element(ElementType type_in, size_t offset_in);
			ElementType GetType() const;
			size_t Size() const noxnd;
			size_t GetOffset() const;
			size_t GetOffsetAfter() const noxnd;
			static size_t SizeOf(ElementType type_in) noexcept;
			D3D11_INPUT_ELEMENT_DESC GetDesc() const noxnd;
			const char* GetCode() const noxnd;

			
		private:
			template<ElementType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset) noxnd
			{
				return { Map<type>::semantic,0,Map<type>::dxgiFormat,0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0 };
			}

		private:
			ElementType type;
			size_t offset;
		};

	public:
		template<ElementType Type>
		const Element& Resolve() const noxnd
		{
			for (auto& e : elements)
			{
				if (e.GetType() == Type)
				{
					return e;
				}

			}
			assert("No such element" && false);
			return elements.front();
		}
		const Element& ResolveByIndex(size_t i) const noxnd;
		VertexLayout& Append(ElementType type) noxnd;
		size_t Size() const noxnd;
		size_t GetElementCount() const noexcept;
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noxnd;
		std::string GetCode() const noxnd;

	private:
		std::vector<Element> elements;
	};

	class Vertex
	{
		friend class VertexBuffer;
	public:


		template<VertexLayout::ElementType Type>
		auto& Attr() const noxnd
		{
			auto pAttribute = pData + layout.Resolve<Type>().GetOffset();
			return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
		}
		template<typename T>
		void SetAttributeByIndex(size_t index, T&& val)
		{
			using namespace DirectX;
			const auto& element = layout.ResolveByIndex(index);
			auto pAttribute = pData + element.GetOffset();
			switch (element.GetType())
			{
			case (VertexLayout::Position2D):
			{
				SetAttribute<VertexLayout::Position2D>(pAttribute, std::forward<T>(val));
				break;
			}
			case VertexLayout::Position3D:
				SetAttribute<VertexLayout::Position3D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Texture2D:
				SetAttribute<VertexLayout::Texture2D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Normal:
				SetAttribute<VertexLayout::Normal>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Float3Color:
				SetAttribute<VertexLayout::Float3Color>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Float4Color:
				SetAttribute<VertexLayout::Float4Color>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::BGRAColor:
				SetAttribute<VertexLayout::BGRAColor>(pAttribute, std::forward<T>(val));
				break;
			default:
				assert("Bad element type" && false);
			}
		}
	protected:

		Vertex(char* pData, const VertexLayout& layout) noxnd;
		
	private:

		template<typename First, typename ...Rest>
		void SetAttributeByIndex(size_t index, First&& first, Rest&&... rest)
		{
			SetAttributeByIndex(index, std::forward<First>(first));
			SetAttributeByIndex(index + 1, std::forward<Rest>(rest)...);
		}

		template<VertexLayout::ElementType DestLayoutType, typename SrcType>
		void SetAttribute(char* attr, SrcType&& source)
		{
			using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
			if constexpr (std::is_assignable<Dest, SrcType>::value)
			{
				*reinterpret_cast<Dest*>(attr) = source;
			}
			else
			{
				assert("Dest and Source are not compatible" && false);
			}
		}
	private:
		const VertexLayout& layout;
		char* pData = nullptr;
	};


	class ConstVertex
	{
	public:
		ConstVertex(const Vertex& v) noxnd;
			
		template<VertexLayout::ElementType Type>
		const auto& Attr() const noxnd
		{
			// int Attr this is const so we make it not const
			return const_cast<Vertex&>(vertex).Attr<Type>();
		}
	private:
		Vertex vertex;
	};


	class VertexBuffer
	{
	public:
		VertexBuffer(VertexLayout layout_in);
		const VertexLayout& GetLayout() const noexcept;
		size_t Size() const noxnd;
		size_t SizeBytes() const noxnd;
		Vertex Back()  noxnd;
		Vertex Front() noxnd;
		Vertex operator [](size_t i) noxnd;
		ConstVertex Back() const noxnd;
		ConstVertex Front() const noxnd;
		ConstVertex operator[](size_t i) const noxnd;
		const char* GetData()const noxnd;

		template <typename ...Params >
		void EmplaceBack(Params&&... params)
		{
			assert(sizeof...(params) == layout.GetElementCount() && "Number of Parameters does not match the number of elements in the layout");
			buffer.resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}
	private:
		std::vector<char> buffer;
		VertexLayout layout;
	};
}