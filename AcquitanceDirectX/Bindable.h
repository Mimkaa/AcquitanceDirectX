#pragma once
#include "Graphics.hpp"

class Drawable;
class TechniqueProbe;
// if the base class is a friend class, children are not
namespace Bind {
	class Bindable
	{
	public:
		virtual void Bind(Graphics& gfx) noexcept = 0;
		virtual ~Bindable() = default;
		virtual std::string GetUID() const noexcept
		{
			assert(false);
			return " ";
		}
		virtual void InitializeParentReference(const Drawable& drawable) noexcept
		{}
		virtual void Accept(TechniqueProbe& probe)
		{}
	protected:// only available to children and friends

		// we create these to access members of Graphics 
		// using Bindable and at the same time all the children of 
		// Bindable will have these functions inherited 
		// hence we can access everything from the graphics in them too
		static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
		static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
		static DxgiInfoManager& GetInfoManager(Graphics& gfx) noxnd;
	};
}