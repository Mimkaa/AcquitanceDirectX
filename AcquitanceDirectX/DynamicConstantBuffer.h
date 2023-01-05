#pragma once

namespace Dcbf
{
	// X-Macros
#define LEAFTYPES\
	X(Float)\
	X(Float3)\
	X(Float2)\
	X(Bool)\
	X(INT)

	enum Type
	{
	#define X(el) el,
		LEAFTYPES
	#undef X
	Array,
	Srtuct
	};

#undef LEAFTYPES
}

int a = Dcbf::Type::Float;