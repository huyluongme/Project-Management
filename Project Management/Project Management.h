#pragma once

#include "resource.h"

class cPM
{
public:
	cPM() {
		nWidth = 1280;
		nHeight = 800;
		g_pD3D = NULL;
		g_pd3dDevice = NULL;
		g_d3dpp = {};
	}

	int nWidth, nHeight;

	LPDIRECT3D9              g_pD3D;
	LPDIRECT3DDEVICE9        g_pd3dDevice;
	D3DPRESENT_PARAMETERS    g_d3dpp;

	ATOM MyRegisterClass(HINSTANCE hInstance);
	void  GetDesktopResolution(int& horizontal, int& vertical);
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void ResetDevice();
};

extern cPM* PN;