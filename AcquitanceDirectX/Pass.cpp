#include "Pass.h"


void Pass::SetPassLinkage(const std::string& sinkName, const std::string& pass_source_names)
{
	using namespace std::string_literals;
	for (auto& s : sinks)
	{
		if (s->GetRegisteredName() == sinkName)
		{
			auto strVec = SplitString(pass_source_names, "."s);
			// for now no name validity chacks (TODO)
			s->SetPass(std::move(std::string(strVec[0])));
			s->SetSource(std::move(strVec[1]));
		}
	}
}

void Pass::RegisterSource(std::unique_ptr<Source> source)
{
	sources.push_back(std::move(source));
}

void Pass::RegisterSink(std::unique_ptr<Sink> sink)
{
	sinks.push_back(std::move(sink));
}