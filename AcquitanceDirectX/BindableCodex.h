#pragma once

#include "Bindable.h"
#include "BindableCodex.h"
#include <memory>
#include <unordered_map>

namespace Bind
{
	class Codex
	{
	public:
		static std::shared_ptr<Bindable> Resolve(const std::string& bindID)
		{
			return Get().Resolve_(bindID);
		}

		static void Store(std::shared_ptr<Bindable> bind)
		{
			Get().Store_(std::move(bind));
		}
	private:
		std::shared_ptr<Bindable> Resolve_(const std::string& bindID)
		{
			auto i = binds.find(bindID);
			if (i == binds.end())
			{
				return {};
			}
			else
			{
				return i->second;
			}
		}
		void Store_(std::shared_ptr<Bindable> bind_in)
		{
			binds[bind_in->GetUID()] = std::move(bind_in);
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