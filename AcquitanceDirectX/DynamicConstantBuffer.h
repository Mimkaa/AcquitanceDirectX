#pragma once
#include <DirectXMath.h>
#include "optional"
#include <string>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <type_traits>
#include <unordered_map>

namespace Dcb
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
	X(Float4)\

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
	template<> struct Map<Float4>
	{
		using SysType = dx::XMFLOAT4;
		static constexpr size_t size = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "FL4";
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
		using SysType = bool;
		static constexpr size_t size = 4u;
		static constexpr bool valid = true;
		static constexpr const char* code = "BL";
	};
	template<> struct Map<Int>
	{
		using SysType =  int;
		static constexpr size_t size = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "INT";
	};
	template<> struct Map<Matrix>
	{
		using SysType = dx::XMFLOAT4X4;
		static constexpr size_t size = sizeof(SysType);
		static constexpr bool valid = true;
		static constexpr const char* code = "MT";
	};

	// check weather or not we have added all cases
	#define X(el) static_assert(Map<el>::valid, "you forgot to add a description in the map");
		LEAFTYPES
	#undef X

	// generate a reverse map
	template<typename T> struct ReverseMap
	{
		static constexpr bool valid = false;

	};
	// "\" continuation of the line
	#define X(el) \
	template<> struct ReverseMap<typename Map<el>::SysType> \
	{ \
		static constexpr Type EnumType = el; \
		static constexpr bool valid = true; \
	};

	LEAFTYPES
	#undef X

	



	class LayoutElement
	{
		

		friend class ExtraData;
		
		friend class RawLayout;

		friend class Buffer;

		friend class ElementRef;


		struct ExtraDataBase
		{
			virtual ~ExtraDataBase() = default;
			
		};
		friend struct ExtraData;

		

			

	public:

		LayoutElement(Type type_in);


		// construct an empty layout element
		LayoutElement() noexcept = default;
		
		bool Exists() const;
	

		std::string GetSignature() const;
		

		std::string GetSignatureFromStruct() const;
		

		std::string GetSignatureFromArray() const;
		

		static LayoutElement& GetEmptyElement() 
		{
			// this will be called only once for a satic object
			static LayoutElement empty{};
			return empty;
		}

		LayoutElement& operator[](const std::string& key) const;
		

		LayoutElement& T();
		

		const LayoutElement& T() const
		{
			return const_cast<LayoutElement*>(this)->T();
		}

		size_t GetOffsetBegin() const;
		

		size_t AdvanceToBoudary(size_t val_in) const;
		

		bool CrossesBoundary(size_t offset, size_t val_in);
	



		size_t GetOffsetEnd() const;
		
		size_t GetSizeBytes() const ;
		

		LayoutElement& Add(Type type_in, std::string str_in);
		

		template <Type typen>
		LayoutElement& Add(std::string str_in)
		{
			return  Add(typen, std::move(str_in));
		}

		bool ValidateSymbols(std::string& str_in)
		{
			using namespace std::string_literals;
			return !std::isdigit(str_in[0]) && !str_in.empty() && 
				std::all_of(str_in.begin(), str_in.end(), [](const char& c) -> bool { return std::isalnum(c) || c == '_'; });
		}

		LayoutElement& Set(Type type_in, size_t size);
	

		template <Type typen>
		LayoutElement& Set(size_t size)
		{
			return Set(typen, size);
		}

		
		
		template<typename T>
		size_t Resolve() const 
		{
			
			switch (type)
			{
			#define X(el) case el: assert(typeid(Map<el>::SysType) == typeid(T)); return *offset;
				LEAFTYPES
			#undef X
			default:
				assert("Tried to resolve non-leaf element" && false);
				return 0u;
			}
		}
		
		size_t AdvanceIfCrossesBoundary(size_t offset_in, size_t size_in)
		{
			return CrossesBoundary(offset_in, size_in) ? AdvanceToBoudary(offset_in) : offset_in;
		}
		

		size_t FinalizeForStruct(size_t offset_in);
		
		size_t FinalizeForArray(size_t offset_in);
		
		std::pair<size_t, const LayoutElement*> CalculateIndexingOffset(size_t offset, size_t index) const;
		

		size_t Finalize(size_t offset_in);
		

	private:
		 
		std::optional<size_t> offset;
		std::unique_ptr<ExtraDataBase> extraData;
		Type type = Empty;
	};

	class Layout
	{
	friend class Buffer;
	friend class LayoutCodex;
	
	protected:
		Layout(std::shared_ptr<LayoutElement> layIn)
			:pRoot(layIn)
		{}
		std::shared_ptr<LayoutElement> pRoot;
	public:
		std::string GetSignature()
		{
			return pRoot->GetSignature();
		}
		size_t GetSizeInBytes()
		{
			return pRoot->GetSizeBytes();
		}
	};

	class RawLayout :public Layout
	{
	public:
		RawLayout()
			:Layout(std::move(std::make_shared<LayoutElement>(Struct)))
		{}

		

		LayoutElement& operator[](const std::string& string_in) const
		{
			return (*pRoot)[string_in];
		}

		std::shared_ptr<LayoutElement>DeliverRoot()
		{
			auto pCopy = std::move(pRoot);
			pCopy->Finalize(0);
			*this = RawLayout();
			return std::move(pCopy);
		}

		template<Type T>
		LayoutElement& Add(const std::string& name)
		{
			return pRoot->Add<T>(name);
		}

		void ClearRoot()
		{
			*this = RawLayout();
		}

	};

	class  CookedLayout :public Layout
	{
		friend class Buffer;
	public:
		CookedLayout(std::shared_ptr<LayoutElement> pRoot_in)
			:Layout(std::move(pRoot_in))
		{}

		std::shared_ptr<LayoutElement> RelinquishRoot() const 
		{
			return std::move(pRoot);
		}

		std::shared_ptr<LayoutElement> ShareRoot() const
		{
			return pRoot;
		}
		const LayoutElement& operator[](const std::string& string_in) const
		{
			return (*pRoot)[string_in];
		}
	


	};

	class ConstElementRef
	{
	public:
		class Ptr
		{
			friend ConstElementRef;
		public:
			template<typename T>
			// the way to overload casting 
			operator const T* ()const
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsuported SysType");
				return &static_cast<const T&>(*ref);
			}
		private:
			Ptr(const ConstElementRef* ref_in)
				:ref(ref_in)
			{}
			const ConstElementRef* ref;
		};
	public:
		ConstElementRef(const LayoutElement* lay_in, const char* bytes_in, const size_t offset_in)
			:
			offset(offset_in),
			element(lay_in),
			bytes(bytes_in)
		{}
		// the way to overload casting 
		template <typename T>
		operator const T& ()const
		{
			static_assert (ReverseMap<std::remove_const_t<T>>::valid, "Unsuported SysType");
			return *reinterpret_cast<const T*>(bytes + offset + element->Resolve<T>());
		}
		
		bool Exists() const
		{
			return element->Exists();
		}

		Ptr operator &() const
		{
			return Ptr{ this };
		}

		ConstElementRef operator[](const std::string& name) const
		{
			return { &(*element)[name], bytes, offset };
		}

		ConstElementRef operator[](const size_t index) const
		{
			const auto IndexingData = element->CalculateIndexingOffset(offset, index);
			return { IndexingData.second, bytes, IndexingData.first };
		}




	private:
		const size_t offset;
		const LayoutElement* element;
		const char* bytes;

	};

	class ElementRef
	{
	public:
		class Ptr
		{
			friend ElementRef;
		public:
			template<typename T>
			// the way to overload casting 
			operator T* ()const
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsuported SysType");
				return &static_cast<T&>(*ref);
			}
		private:
			Ptr(ElementRef* ref_in)
				:ref(ref_in)
			{}
			ElementRef* ref;
		};
	public:
		ElementRef(LayoutElement* lay_in, char* bytes_in, size_t offset_in)
			:
			offset(offset_in),
			element(lay_in),
			bytes(bytes_in)
		{}

		operator ConstElementRef() const noexcept
		{
			return { element,bytes,offset };
		}

		// the way to overload casting 
		template <typename T>
		operator T& ()const
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in refernce conversion");
			return *reinterpret_cast<T*>(bytes + offset + element->Resolve<T>());
		}

		template<typename T>
		T& operator = (const T& rhs) const // rhs - right hand side  
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in refrence conversion");
			//static_assert(std::is_same<Map<element->type>::SysType, T>::value, "cannot assign this value to the element");
			return static_cast<T&>(*this) = rhs;
		}

		bool Exists() const
		{
			return element->Exists();
		}
	
		template <typename S>
		bool SetIfExists(const S& rhs)
		{
			if (Exists())
			{
				*this = rhs;
				return true;
			}
			return false;
		}

		Ptr operator &() const
		{
			return Ptr{ const_cast<ElementRef*>(this) };
		}
		
		ElementRef operator[](const std::string& name)  const
		{
			return {&(*element)[name], bytes, offset };
		}

		ElementRef operator[](const size_t index) const
		{
			const auto IndexingData = element->CalculateIndexingOffset((*element).GetOffsetBegin(), index);
			return {const_cast<LayoutElement*>(IndexingData.second), bytes, IndexingData.first};
		}

		
	

	private:
		size_t offset;
		LayoutElement* element;
		char* bytes;

	};

	class LayoutCodex
	{
	public:
		static CookedLayout Resolve(Dcb::RawLayout&& raw)
		{
			auto& map = Get_().map;
			auto sign = raw.GetSignature();
			auto it = map.find(sign);

			if (it != map.end())
			{
				raw.ClearRoot();
				return it->second;
			}
			auto iter = map.insert({ sign, raw.DeliverRoot() });
			return { iter.first->second };
		}
	private:

		static LayoutCodex& Get_()
		{
			static LayoutCodex codex;
			return codex;
		}
		std::unordered_map<std::string, std::shared_ptr<LayoutElement>> map;
	};




	class Buffer
	{
	public:
		Buffer(RawLayout&& raw)
			:
			Buffer(LayoutCodex::Resolve(std::move(raw)))
		{}

		Buffer(const CookedLayout& cookedIn)
			:
			pLayout(cookedIn.ShareRoot()),
			buffer(pLayout->GetOffsetEnd())
		{}
		Buffer(CookedLayout&& cookedIn)
			:
			pLayout{ cookedIn.RelinquishRoot() },
			buffer(pLayout->GetOffsetEnd())
		
		{}

		Buffer(const Buffer& buf_in)
			:
			pLayout(buf_in.pLayout),
			buffer(buf_in.buffer)
		{}
		
		Buffer(Buffer&& buf_in) noexcept
			:
			pLayout(std::move(buf_in.pLayout)),
			buffer(std::move(buf_in.buffer))
		{}

		ElementRef operator[](const std::string& key) 
		{
			return { &(*pLayout)[key], buffer.data(), 0 };
		}

		ConstElementRef operator[](const std::string& key) const
		{
			return (*const_cast<Buffer*>(this))[key];
		}
		const char* GetData() const noexcept
		{
			return buffer.data();
		}

		size_t GetSizeInBytes() const noexcept
		{
			return buffer.size();
		}

		std::shared_ptr<LayoutElement> ShareLayoutRoot() const
		{
			return pLayout;
		}

		void CopyFromBuffer(const Buffer& buf_in)
		{
			assert(&GetRootLayoutElement() == &buf_in.GetRootLayoutElement());
			std::copy(buf_in.buffer.begin(), buf_in.buffer.end(), buffer.begin());
		}

		const LayoutElement& GetRootLayoutElement() const
		{
			return *pLayout;
		}

	private:
		std::shared_ptr<LayoutElement> pLayout;
		std::vector<char> buffer;

	};



#ifndef DCB_IMPL_SOURCE
#undef LEAFTYPES
#endif
}


