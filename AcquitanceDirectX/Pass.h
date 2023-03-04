#pragma once
#include <string>
#include <vector>
#include "Sink.h"
#include"Source.h"
#include "Vertex.h"
#include <memory>
#include "ChilliStringUtils.h"


class Graphics;

namespace Bind
{
	class Bindable;
};

class Pass
{
public:
	Pass(std::string name)
		:
		unique_name(std::move(name))
	{}

	virtual void Execute(Graphics& gfx) = 0;
	virtual void Reset()
	{
		
	}
	std::string GetName() const
	{
		return unique_name;
	}
protected:
	void RegisterSink(std::unique_ptr<Sink> sink);


	void RegisterSource(std::unique_ptr<Source> source);
	

	

public:

	void SetPassLinkage(const std::string& sinkName, const std::string& pass_source_names);

	
	std::vector<std::unique_ptr<Sink>>& GetSinks() 
	{
		return sinks;
	}

	std::vector<std::unique_ptr<Source>>& GetSources() 
	{
		return sources;
	}

protected:
	std::string unique_name;
	std::vector<std::unique_ptr<Sink>> sinks;
	std::vector<std::unique_ptr<Source>> sources;
};















