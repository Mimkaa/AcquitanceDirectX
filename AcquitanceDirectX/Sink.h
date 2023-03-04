#pragma once
#include <string>
#include <memory>
#include <sstream>
#include <type_traits>


class Source;

class Sink
{
public:
	Sink(std::string regiseredName);
		
	std::string GetRegisteredName() const
	{
		return registeredName;
	}
	std::string GetSourceName() const
	{
		return sourceName;
	}
	std::string GetPassName() const
	{
		return passName;
	}
	void SetPass(std::string pass)
	{
		passName = std::move(pass);
	}
	void SetSource(std::string source)
	{
		sourceName = std::move(source);
	}
	virtual void PostLinkValidation() const
	{}
	virtual void Bind(std::unique_ptr<Source>& source ) = 0;

private:
	std::string passName;
	std::string sourceName;
	std::string registeredName;
	
};


template<class T>
class DirectPassSink : public Sink
{
public:
	static std::unique_ptr<DirectPassSink<T>> Make (std::string registeredName, std::shared_ptr<T>& target_in)
	{
		return std::make_unique<DirectPassSink<T>>(std::move(registeredName), target_in);
	}
	DirectPassSink(std::string registeredName, std::shared_ptr<T>& target_in)
		:
		Sink{std::move(registeredName)}
	{
		target = target_in;
		linked = false;
	}
	void PostLinkValidation() const override
	{
		if (!linked)
		{
			throw std::runtime_error("one of the sinks is not linked");
		}
	}
	void Bind(std::unique_ptr<Source>& source);
	
private:
	bool linked;
	std::shared_ptr<T> target;

};


template<class T>
class BindPassSink : public Sink
{
public:
	static std::unique_ptr< BindPassSink<T>> Make(std::string registeredName, std::shared_ptr<T>& target_in)
	{
		return std::make_unique< BindPassSink<T>>(std::move(registeredName), target_in);
	}
	BindPassSink(std::string registeredName, std::shared_ptr<T>& target_in)
		:
		Sink{ std::move(registeredName) }
	{
		target = target_in;
		linked = false;
	}
	void Bind(std::unique_ptr<Source>& source);
private:
	bool linked;
	std::shared_ptr<T> target;

};