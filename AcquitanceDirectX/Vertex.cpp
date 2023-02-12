#include"Vertex.h"
namespace Dvtx {
	// vertex element
	VertexLayout::Element::Element(ElementType type_in, size_t offset_in)
		:
		type(type_in),
		offset(offset_in)
	{}

	VertexLayout::ElementType VertexLayout::Element::GetType() const
	{
		return type;
	}

	size_t VertexLayout::Element::Size() const noxnd
	{
		return SizeOf(type);
	}
	size_t VertexLayout::Element::GetOffset() const
	{
		return offset;
	}
	size_t VertexLayout::Element::GetOffsetAfter() const noxnd
	{
		return offset + Size();
	}

	template<VertexLayout::ElementType type>
	struct SysSizeLookup
	{
		static constexpr auto Exec() noexcept
		{
			return sizeof(VertexLayout::Map<type>::SysType);
		}
	};
	constexpr size_t VertexLayout::Element::SizeOf(ElementType type) noexcept
	{
		return Bridge<SysSizeLookup>(type);
	}

	template<VertexLayout::ElementType type>
	struct CodeLookup
	{
		static constexpr auto Exec() noexcept
		{
			return VertexLayout::Map<type>::code;
		}
	};
	const char* Dvtx::VertexLayout::Element::GetCode() const noexcept
	{
		return Bridge<CodeLookup>(type);
	}

	template<VertexLayout::ElementType type> struct DescGenerate {
		static constexpr D3D11_INPUT_ELEMENT_DESC Exec(size_t offset) noexcept {
			return {
				VertexLayout::Map<type>::semantic,0,
				VertexLayout::Map<type>::dxgiFormat,
				0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0
			};
		}
	};
	D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const noxnd
	{
		return Bridge<DescGenerate>(type, GetOffset());
	}

	// VertexLayout

	const VertexLayout::Element& VertexLayout::ResolveByIndex(size_t i) const noxnd
	{
		
			return elements[i];
		
	}

	VertexLayout& VertexLayout::Append(ElementType type) noxnd
	{
		if (!Has(type))
		{
			elements.emplace_back(type, Size());
		}
		return *this;
	}

	size_t VertexLayout::Size() const noxnd
	{
		return elements.empty() ? 0u : elements.back().GetOffsetAfter();
	}

	size_t VertexLayout::GetElementCount() const noexcept
	{
		return elements.size();
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::GetD3DLayout() const noxnd
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
		desc.reserve(GetElementCount());
		for (const auto& e : elements)
		{
			desc.push_back(e.GetDesc());
		}
		return desc;
	}

	std::string VertexLayout::GetCode() const noxnd
	{
		std::string result;
		for (auto& e : elements)
		{
			result += e.GetCode();
		}
		return result;
	}

	bool VertexLayout::Has(VertexLayout::ElementType Type) const noexcept 
	{
		for (auto& e : elements)
		{
			if (e.GetType() == Type)
			{
				return true;
			}
		}
		return false;
	}

	// Vertex

	
	Vertex::Vertex(char* pData, const VertexLayout& layout) noxnd
		:
		pData(pData),
		layout(layout)
	{
		assert(pData != nullptr);
	}

	ConstVertex::ConstVertex(const Vertex& v) noxnd
		:
		vertex(v)
	{}

	// VertexBuffer


	VertexBuffer::VertexBuffer(VertexLayout layout_in)
		:
		layout(std::move(layout_in))
	{}
	VertexBuffer::VertexBuffer(VertexLayout layout_in, size_t size)
		:
		layout(std::move(layout_in))
	{
		Reserve(size);
	}

	template<VertexLayout::ElementType type>
	struct SetVertaxAttribute
	{
		static constexpr auto Exec(VertexBuffer* pBuff, const aiMesh& mesh_in) noexcept
		{
			for (int end = mesh_in.mNumVertices, i = 0; i < end; i++)
			{
				(*pBuff)[i].Attr<type>() = VertexLayout::Map<type>::Extract(mesh_in, i);
			}
		}
	};

	VertexBuffer::VertexBuffer(VertexLayout lay_in, const aiMesh& mesh_in)
		:
		layout(std::move(lay_in))
	{
		Reserve(mesh_in.mNumVertices);
		for (int end = layout.GetElementCount(), i = 0; i < end; i++)
		{
			VertexLayout::Bridge<SetVertaxAttribute>(layout.ResolveByIndex(i).GetType(), this, mesh_in);
		}
	}
	



	const VertexLayout& VertexBuffer::GetLayout() const noexcept
	{
		return layout;
	}

	size_t VertexBuffer::Size() const noxnd
	{
		return buffer.size() / layout.Size();
	}

	size_t VertexBuffer::SizeBytes() const noxnd
	{
		return buffer.size();
	}

	Vertex VertexBuffer::Back() noxnd
	{
		return Vertex{ buffer.data() + buffer.size() - layout.Size(), layout };
	}

	Vertex VertexBuffer::Front() noxnd
	{
		return Vertex{ buffer.data(), layout };
	}

	Vertex VertexBuffer::operator[](size_t i) noxnd
	{
		assert(i < Size());
		return Vertex{ buffer.data() + layout.Size() * i, layout };
	}

	ConstVertex VertexBuffer::Back() const noxnd
	{
		// const_cast removes constness
		return const_cast<VertexBuffer*>(this)->Back();
	}

	ConstVertex VertexBuffer::Front() const noxnd
	{
		return const_cast<VertexBuffer*>(this)->Front();
	}

	ConstVertex VertexBuffer::operator[](size_t i) const noxnd
	{
		return const_cast<VertexBuffer&>(*this)[i];
	}

	const char* VertexBuffer::GetData() const noxnd
	{
		return buffer.data();
	}

}