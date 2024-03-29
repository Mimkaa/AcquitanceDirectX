#define DCB_IMPL_SOURCE
#include "DynamicConstantBuffer.h"
namespace Dcb
{
	struct ExtraData
	{


		struct StructData : public Dcb::LayoutElement::ExtraDataBase
		{
			std::vector<std::pair<std::string, Dcb::LayoutElement>> data;
		};

		struct ArrayData : public LayoutElement::ExtraDataBase
		{
			std::optional<Dcb::LayoutElement> type;
			size_t size;
			size_t element_size;
		};
	};

	LayoutElement::LayoutElement(Type type_in)
		:
		type(type_in)
	{
		// not a compile time cause by the time it is called the program is converted to the machine code
		assert(type != Empty, "type cannot be empty");
		if (type_in == Struct)
		{
			extraData = std::unique_ptr<ExtraData::StructData>{ new ExtraData::StructData{} };
		}
		else if (type_in == Array)
		{
			extraData = std::unique_ptr<ExtraData::ArrayData>{ new ExtraData::ArrayData{} };
		}

	}

	bool LayoutElement::Exists() const
	{
		return type != Empty;
	}
	

	std::string LayoutElement::GetSignature() const
	{
		switch (type)
		{
			#define X(el) case(el):return Map<el>:: code;
				LEAFTYPES
			#undef X

			case(Struct):
				{
					return GetSignatureFromStruct();
				}
			case(Array):
			{
				return GetSignatureFromArray();
			}
		}

	}

	std::string LayoutElement::GetSignatureFromStruct() const
	{
		using namespace std::string_literals;
		assert(type == Struct && "the type is not Struct");
		auto data = static_cast<ExtraData::StructData*>(extraData.get());
		std::string beg = "St{"s;
		for (auto& i : data->data)
		{
			beg += " "s + i.first + " "s + ":"s + " "s + i.second.GetSignature() + " ;";
		}
		beg += "}"s;
		return beg;
	}


	std::string LayoutElement::GetSignatureFromArray() const
	{
		using namespace std::string_literals;
		assert(type == Array, "the type is not Array");
		auto data = static_cast<ExtraData::ArrayData*>(extraData.get());
		std::string res = "["s + " "s + data->type->GetSignature() + ":"s + std::to_string(data->size) + "]"s;
		return res;

	}

	LayoutElement& LayoutElement::operator[](const std::string& key) const
	{
		assert(type == Struct && "the type is not Struct");
		auto data = static_cast<ExtraData::StructData*>(extraData.get());
		for (auto& el : (*data).data)
		{
			if (el.first == key)
			{
				return el.second;
			}
		}
		return GetEmptyElement();
	}

	LayoutElement& LayoutElement::T()
	{
		assert(type == Array && "the type is not Array");
		auto data = static_cast<ExtraData::ArrayData*>(extraData.get());
		return *(data -> type);
	}

	size_t LayoutElement::GetOffsetBegin() const
	{
		return *offset;
	}

	size_t LayoutElement::AdvanceToBoudary(size_t val_in) const
	{
		return val_in + (16u - val_in % 16u) % 16u;
	}

	bool LayoutElement::CrossesBoundary(size_t offset, size_t val_in)
	{
		const auto end = offset + val_in;
		const auto pageStart = offset / 16u;
		const auto pageEnd = end / 16u;
		return (pageStart != pageEnd && end % 16 != 0u) || val_in > 16u;
	}

	size_t LayoutElement::GetOffsetEnd() const
	{

		switch (type)
		{
		#define X(el) case(el):return *offset + Map<el>::size;
			LEAFTYPES
		#undef X
		case(Struct):
			{
				auto data = static_cast<ExtraData::StructData*>(extraData.get());
				return *offset + AdvanceToBoudary(data->data.back().second.GetOffsetEnd());
			}
		case(Array):
			{
				auto data = static_cast<ExtraData::ArrayData*>(extraData.get());
				//assert(data->type->GetSizeBytes() > 0, "Size of array data must not be 0");
				return *offset + AdvanceToBoudary(data->type->GetSizeBytes()) * data->size;
			}
		}

	}

	size_t LayoutElement::GetSizeBytes() const
	{
		return GetOffsetEnd() - GetOffsetBegin();
	}

	LayoutElement& LayoutElement::Add(Type type_in, std::string str_in)
	{
		assert(type == Struct && "the type is not Struct");
		assert(ValidateSymbols(str_in), "invalid Name");
		auto data = static_cast<ExtraData::StructData*>(extraData.get());
		for (auto& i : data->data)
		{
			if (str_in == i.first)
			{
				assert("you already have that name in me" && false);
			}
		}
		data->data.emplace_back(std::move(str_in), LayoutElement(type_in));
		return *this;
	}

	LayoutElement& LayoutElement::Set(Type type_in, size_t size)
	{
		assert(type == Array && "the type is not Array");
		auto data = static_cast<ExtraData::ArrayData*>(extraData.get());
		data->size = size;
		data->type = { type_in };
		return *this;
	}
	size_t LayoutElement::FinalizeForStruct(size_t offsetIn)
	{
		auto& data = static_cast<ExtraData::StructData&>(*extraData);
		assert(data.data.size() != 0u);
		offset = AdvanceToBoudary(offsetIn);
		auto offsetNext = *offset;
		for (auto& el : data.data)
		{
			offsetNext = el.second.Finalize(offsetNext);
		}
		return offsetNext;
	}

	size_t LayoutElement::FinalizeForArray(size_t offsetIn)
	{
		auto& data = static_cast<ExtraData::ArrayData&>(*extraData);
		assert(data.size != 0u);
		offset = AdvanceToBoudary(offsetIn);
		data.type->Finalize(*offset);
		data.element_size = LayoutElement::AdvanceToBoudary(data.type->GetSizeBytes());
		return GetOffsetEnd();
	}
	 
	size_t LayoutElement::Finalize(size_t offset_in)
	{
		switch (type)
		{
			#define X(el) case el: offset = AdvanceIfCrossesBoundary(offset_in, Map<el>::size); return *offset + Map<el>::size ;
				LEAFTYPES
			#undef X
			case(Struct):
				{
					return FinalizeForStruct(offset_in);
				}
			case(Array):
				{
					return FinalizeForArray(offset_in);
				}
		}
	}

	std::pair<size_t, const LayoutElement*> LayoutElement::CalculateIndexingOffset(size_t offset, size_t index) const
	{
		assert("Indexing into non-array" && type == Array);
		const auto& data = static_cast<ExtraData::ArrayData&>(*extraData);
		//size_t offset_el = *((*(data.type)).offset);
		assert(index < data.size);
		return { offset + data.element_size * index,&*data.type };
	}

}