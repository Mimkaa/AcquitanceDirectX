#include "Technique.h"
#include "FrameCommander.h"
#include "Drawable.h"

void Technique::Submit(FrameComander& frame, const Drawable& drawable, size_t FilterChennel)
{
	if (active && ((chennel & FilterChennel)!=0))
	{
		for (auto& s : steps)
		{
			s.Submit(frame, drawable);
		}
	}
}

void Technique::InitializeParentReference(const Drawable& drawable) noexcept
{
	for (auto& s : steps)
	{
		s.InitializeParentReference(drawable);
	}
}