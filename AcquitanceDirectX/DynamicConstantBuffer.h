#pragma once
#include <DirectXMath.h>
#include "optional"
#include <string>
#include <vector>
#include <memory>
#include <string>

namespace Dcbf
{
	namespace dx = DirectX;

	// X-Macros
	#define LEAFTYPES\
	X(Float)\
	X(Float3)\
	X(Float2)\
	X(Bool)\
	X(Int)\
	X(Matrix)\

	enum Type
	{
	#define X(el) el,
		LEAFTYPES
	#undef X
	Array,
	Struct,
	Empty
	};

	template<Type> struct Map
	{
		static constexpr bool valid = false;

	};
	template<> struct Map<Float>
	{
		using SysType = float;
		static constexpr size_t size = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "FL";
	};
	template<> struct Map<Float2>
	{
		using SysType = dx::XMFLOAT2;
		static constexpr size_t size = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "FL2";
	};
	template<> struct Map<Float3>
	{
		using SysType = dx::XMFLOAT3;
		static constexpr size_t size = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "FL3";
	};
	template<> struct Map<Bool>
	{
		using SysType = int;
		static constexpr size_t size = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "BL";
	};
	template<> struct Map<Int>
	{
		using SysType = int;
		static constexpr size_t size = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "INT";
	};
	template<> struct Map<Matrix>
	{
		using SysType = dx::XMMATRIX;
		static constexpr size_t size = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "MT";
	};

	// check weather or not we have added all cases
	#define X(el) static_assert(Map<el>::valid, "you forgot to add a description int the map");
		LEAFTYPES
	#undef X

	// generate a reverse map
	template<typename T> 
	struct ReverseMap
	{
		static constexpr bool valid = false;

	};
	// "\" continuation of the line
	#define X(el) template<> struct ReverseMap<Map<el>::SysType>\
		{\
			Type EnumType = el;\
			static constexpr bool valid = true;\
		};\
		LEAFTYPES
	#undef X


	struct ExtraData 
	{
		struct StructData : public LayoutElement::ExtraDataBase
		{
			std::vector<std::pair<std::string, LayoutElement>> data;
		};
		
		struct ArrayData : public LayoutElement::ExtraDataBase
		{
			std::optional<LayoutElement> type;
			size_t size;
			size_t element_size;
		};
	};


	class LayoutElement
	{
		friend struct ExtraData;
		

		struct ExtraDataBase
		{
			virtual ~ExtraDataBase() = default;
			
		};
	private:
		LayoutElement(Type type_in)
			:
			type(type_in)
		{
			// not a compile time cause by the time it is called the program is converted to machine code
			assert(type != Empty, "type cannot be empty");
			if (type_in == Struct)
			{
				extraData = std::unique_ptr<ExtraData::StructData>{ new ExtraData::StructData() };
			}
			else if (type_in == Array)
			{
				extraData = std::unique_ptr<ExtraData::ArrayData>{ new ExtraData::ArrayData() };
			}
			
		}

		// construct an empty layout element
		LayoutElement() noexcept = default;
	public:

		bool Exists()
		{
			return type != Empty;
		}

		std::string GetSignature() const
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

		std::string GetSignatureFromStruct() const
		{
			using namespace std::string_literals;
			assert(type == Struct, "the type is not Struct");
			auto data = static_cast<ExtraData::StructData*>(extraData.get());
			std::string beg = "St{"s;
			for (auto& i : data->data)
			{
				beg += " "s + i.first + " "s + ":"s + " "s + i.second.GetSignature();
			}
			beg += "}"s;
			return beg;
		}

		std::string GetSignatureFromArray() const
		{
			using namespace std::string_literals;
			assert(type == Array, "the type is not Array");
			auto data = static_cast<ExtraData::ArrayData*>(extraData.get());
			std::string res = "["s + " "s + data->type->GetSignature() + ":"s + std::to_string(data->size) + "]"s;
			return res;

		}

		LayoutElement& GetEmptyElement() const
		{
			// this will be called only once for a satic object
			static LayoutElement empty{};
			return empty;
		}

		LayoutElement& operator[](std::string& key) const
		{
			assert(type == Struct, "the type is not Struct");
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
	private:
		 
		std::optional<size_t> offset;
		std::unique_ptr<ExtraDataBase> extraData;
		Type type = Empty;
	};


#undef LEAFTYPES
}

