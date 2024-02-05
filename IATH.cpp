#include <Windows.h>
#include <iostream>

typedef int (WINAPI* MessageBoxAType)(HWND, LPCSTR, LPCSTR, UINT);

int WINAPI MyMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    std::cout << "Custom MessageBoxA called with text: " << lpText << std::endl;
    return 0;
}

int main()
{
    HMODULE hModule = GetModuleHandleA("user32.dll");
    if (hModule == NULL)
    {
        std::cerr << "Failed to get handle to user32.dll" << std::endl;
        return 1;
    }

    MessageBoxAType pOriginalMessageBoxA = reinterpret_cast<MessageBoxAType>(
        GetProcAddress(hModule, "MessageBoxA"));

    if (pOriginalMessageBoxA == NULL)
    {
        std::cerr << "Failed to get address of MessageBoxA function" << std::endl;
        return 1;
    }

    DWORD oldProtect;
    if (!VirtualProtect(&pOriginalMessageBoxA, sizeof(pOriginalMessageBoxA), PAGE_READWRITE, &oldProtect))
    {
        std::cerr << "Failed to change memory protection" << std::endl;
        return 1;
    }

    pOriginalMessageBoxA = MyMessageBoxA;

    if (!VirtualProtect(&pOriginalMessageBoxA, sizeof(pOriginalMessageBoxA), oldProtect, &oldProtect))
    {
        std::cerr << "Failed to restore memory protection" << std::endl;
        return 1;
    }

    MessageBoxA(NULL, "This message will be intercepted!", "Intercepted Message", MB_OK);

    return 0;
}
