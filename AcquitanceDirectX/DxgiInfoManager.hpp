
#pragma once
#include "MyWin.hpp"
#include <vector>
#include <wrl.h>
#include <dxgidebug.h>
class DxgiInfoManager
{
public:
	DxgiInfoManager();
	~DxgiInfoManager() = default;
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long next = 0u;
	// struct pointer
	// we can do something like that:
	/*
	(normal)
	struct date
	{
		short int dd, mm, yy ;
	} ;
	date ∗ dt_ptr ;

	(what we have done below)
	struct date
	{
		short int dd, mm, yy ;
	} ∗ dt_ptr ;
	*/
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue ;
};
