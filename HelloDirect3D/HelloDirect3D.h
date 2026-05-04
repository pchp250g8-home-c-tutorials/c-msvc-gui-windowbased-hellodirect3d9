#pragma once

#include "resource.h"
#include <d3d9.h>
#include <d3dx9.h>

#if defined _DEBUG
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib, "d3dx9d.lib")
#else
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#endif