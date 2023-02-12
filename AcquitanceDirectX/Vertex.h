#pragma once
#include <DirectXMath.h>
#include <type_traits>
#include <vector>
#include "Color.h"
#include <dxgiformat.h>
#include <d3d11.h>
#include "ConditionalNoexcept.h"
#include <string>

#include <assimp/scene.h>           // Output data structure





namespace Dvtx {
	// it will work beacause when we dereference the const pointer we will get (cont "type"&), which will then be passed to the constructor of the object(SysType) 
	#define DVTX_ELEMENT_AI_EXTRACTOR(member) static SysType Extract( const aiMesh& mesh,size_t i ) noexcept {return *reinterpret_cast<const SysType*>(&mesh.member[i]);}

	#define LAYOUT_ELEMENT_TYPES \
		X(Position2D) \
		X(Position3D) \
		X(Texture2D) \
		X(Normal)\
		X(Float3Color)\
		X(Float4Color)\
		X(BGRAColor)\
		X(Tangent)\
		X(Bytangent)\
		X(Count)\

	class VertexLayout
	{
		
	public:
		enum ElementType
		{
			#define X(el) el,
				LAYOUT_ELEMENT_TYPES
			#undef X;
		};
		template<ElementType> struct Map;
		template<> struct Map<Position2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P2";
			DVTX_ELEMENT_AI_EXTRACTOR(mVertices);
		};
		template<> struct Map<Position3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P3";
			DVTX_ELEMENT_AI_EXTRACTOR(mVertices);
		};
		template<> struct Map<Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
			static constexpr const char* code = "T2";
			DVTX_ELEMENT_AI_EXTRACTOR(mTextureCoords[0]);
		};
		template<> struct Map<Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "N3";
			DVTX_ELEMENT_AI_EXTRACTOR(mNormals);
		};
		template<> struct Map<Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C3";
			DVTX_ELEMENT_AI_EXTRACTOR(mNormals);
		};
		template<> struct Map<Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C4";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0]);
		};
		template<> struct Map<BGRAColor>
		{
			using SysType = ::BGRAColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "BGR4";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0]);
		};
		template<> struct Map<Tangent>
		{
			using SysType = ::DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Tangent";
			static constexpr const char* code = "T3";
			DVTX_ELEMENT_AI_EXTRACTOR(mTangents);
		};
		template<> struct Map<Bytangent>
		{
			using SysType = ::DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Bytangent";
			static constexpr const char* code = "B3";
			DVTX_ELEMENT_AI_EXTRACTOR(mBitangents);
		};

		template<> struct Map<Count>
		{
			using SysType = long double;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
			static constexpr const char* semantic = "INVALID";
			static constexpr const char* code = "INVALID";
			DVTX_ELEMENT_AI_EXTRACTOR(mFaces);
		};

		template<template<VertexLayout::ElementType> class F, typename ...Args>
		static constexpr auto Bridge(VertexLayout::ElementType type, Args&&... args)
		{
			switch (type)
			{
			#define X(el) case VertexLayout::el: return F<VertexLayout::el>::Exec(std::forward<Args>(args)...);
				LAYOUT_ELEMENT_TYPES
			#undef X;
			}
			assert("Invalid element type" && false);
			return F<VertexLayout::Count>::Exec(std::forward<Args>(args)...);
		}

		class Element
		{
		public:
			Element(ElementType type_in, size_t offset_in);
			ElementType GetType() const;
			size_t Size() const noxnd;
			size_t GetOffset() const;
			size_t GetOffsetAfter() const noxnd;
			static constexpr size_t SizeOf(ElementType type) noexcept;
			D3D11_INPUT_ELEMENT_DESC GetDesc() const noxnd;
			const char* GetCode() const noexcept;

			
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

		bool Has(ElementType Type) const noexcept;
		

	private:
		std::vector<Element> elements;
	};

	class Vertex
	{
		friend class VertexBuffer;

	private:
		template<VertexLayout::ElementType T>
		struct AttirbuteSetter
		{
			template<typename TT>
			static auto constexpr Exec(Vertex* pVertex, char* pAttribute, TT&& val)
			{
				return pVertex->SetAttribute<T>(pAttribute, std::forward<TT>(val));
			}
		};
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
			VertexLayout::Bridge<AttirbuteSetter>(element.GetType(), this, pAttribute, std::forward<T>(val));
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
		VertexBuffer(VertexLayout layout_in, size_t size);
		VertexBuffer(VertexLayout layout_in, const aiMesh& mesh_in);
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
		void Reserve(size_t num)
		{
			const int size = buffer.size();
			if (buffer.size() < num)
			{
				buffer.resize(layout.Size() * num - size);
			}
		
		
		}

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

#undef DVTX_ELEMENT_AI_EXTRACTOR
#ifndef DVTX_SOURCE_FILE
#undef LAYOUT_ELEMENT_TYPES
#endif