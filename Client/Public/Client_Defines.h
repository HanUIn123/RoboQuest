#pragma once
#include "imgui.h"
#include "Impl/imgui_impl_win32.h"
#include "Impl/imgui_impl_dx11.h"

#include "Windows.h"
#include "commdlg.h"
#include <process.h>

namespace Client 
{
    const unsigned int      g_iWinSizeX = 1280;
    const unsigned int      g_iWinSizeY = 720;

    enum LEVELID {LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_BOSS, LEVEL_MAPTOOL, LEVEL_END };
}

using namespace Client;

extern HINSTANCE    g_hInst;
extern HWND         g_hWnd;