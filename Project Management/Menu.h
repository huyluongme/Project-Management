#pragma once

#include "framework.h"

class cMenu
{
public:
	std::string utf8_encode(const std::wstring& wstr);
	std::wstring utf8_decode(const std::string& str);
	void DrawMenu(LPDIRECT3DDEVICE9 pDevice);
	void InitializeImGuiStartFrame(LPDIRECT3DDEVICE9 pDevice);
	void InitializeImGuiEndFrame(LPDIRECT3DDEVICE9 pDevice);
	void InitializeImGui(LPDIRECT3DDEVICE9 pDevice, HWND hwnd);
};

extern cMenu* Menu;