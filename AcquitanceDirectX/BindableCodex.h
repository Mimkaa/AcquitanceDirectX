#pragma once

#include "Bindable.h"

#include <memory>
#include <unordered_map>
#include  <type_traits>

namespace Bind
{
	class Codex
	{
	public:
		template<class T, typename ...Params>
		// parameter pack
		static std::shared_ptr<Bindable> Resolve(Graphics& gfx, Params&& ...p)
		{
			assert(std::is_base_of<Bindable, T> && "only children of Bindable are acceptable here");
			return Get().Resolve_<T>(gfx, std::forward<Params>(p)...);
		}

		
	private:
		template<class T, typename ...Params>
		std::shared_ptr<Bindable> Resolve_(Graphics& gfx, Params&& ...p)
		{
			const auto key = T::GenerateUID(std::forward<Params>(p)...);
			const auto bind = binds.find(key);
			if (bind == binds.end())
			{
				// this on will be created but then destroyed
				auto new_s_ptr = std::make_shared<T>(gfx, std::forward<Params>(p)...);
				// but then we copy it to the hash and this way it gets stored (so now we have 2 references to the object)
				binds[key] = new_s_ptr;
				// and then we return it
				return new_s_ptr;
				// supposingly there new_s_ptr is deleted (1 reference) but as we return it in the end whatever will own returned value will make num of refs 2
			}
			else
			{
				return bind->second;
			}
		}
		

	private:
		// this creates a singleton
		static Codex& Get()
		{
			static Codex codex;
			return codex;
		}
		std::unordered_map<std::string, std::shared_ptr<Bindable>> binds;
	};
}