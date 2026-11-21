#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>

// extern "C" stops C++ name mangling, so the exported name is exactly "Init".
extern "C" __declspec(dllexport) void init() {
        printf("[dll] init() ran\n");
        fflush(stdout);
}

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) {
        return TRUE;
}
