#include "Sink.h"
#include "Source.h"

Sink::Sink(std::string regiseredName)
	:
	registeredName{ registeredName }
{}

template<class T>
void DirectPassSink<T>::Bind(std::unique_ptr<Source>& source)
{
	auto p = std::dynamic_pointer_cast<T>(source->YeildBuffer());
	if (!p)
	{
		std::ostringstream oss;
		oss << "you fuched up the class:" << typeid(this) << " with registered name:" << GetRegisteredName() <<
			"pointing to the source:" << GetSourceName() << "attached to :" << GetPassName() << "the types are not compatible";

		throw std::runtime_error(oss.str());
	}
	traget = std::move(p);
	linked = true;
}

template<class T>
void BindPassSink<T>::Bind(std::unique_ptr<Source>& source)
{
	auto p = std::dynamic_pointer_cast<T>(source->YeildBuffer());
	if (!p)
	{
		std::ostringstream oss;
		oss << "you fuched up the class:" << typeid(this) << " with registered name:" << GetRegisteredName() <<
			"pointing to the source:" << GetSourceName() << "attached to :" << GetPassName() << "the types are not compatible";

		throw std::runtime_error(oss.str());
	}
	traget = std::move(p);
	linked = true;
}