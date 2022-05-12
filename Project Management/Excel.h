#pragma once

#include "framework.h"

class cExcel
{
public:
	void Import(const wchar_t* filename);
	void Export_Ex(const wchar_t* filename);
};

extern cExcel* Excel;