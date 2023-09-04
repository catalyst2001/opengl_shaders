#include <Windows.h>
#include <stdio.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void appendTextToEdit(HWND hEdit, LPCSTR newText)
{
	int TextLen = SendMessageA(hEdit, WM_GETTEXTLENGTH, 0, 0);
	SendMessageA(hEdit, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
	SendMessageA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)newText);
}

int main()
{
	HINSTANCE hInstance = GetModuleHandleW(0);

	// Регистрация класса окна
	const char* className = "MyWindowClass";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = className;
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

	RegisterClass(&wc);

	int width = 600;
	int height = 600;
	int xpos = (GetSystemMetrics(SM_CXSCREEN) >> 1) - (width >> 1);
	int ypos = (GetSystemMetrics(SM_CYSCREEN) >> 1) - (height >> 1);

	// Создание окна
	HWND hwnd = CreateWindowEx(
		0,
		className,
		"React Engine    Log Console",
		WS_OVERLAPPEDWINDOW,
		xpos, ypos, width, height,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	// Создание Edit Control
	
	RECT rect;
	GetClientRect(hwnd, &rect);
	int padding = 5;
	HWND hEdit = CreateWindowEx(
		0,
		"EDIT",
		NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE,
		rect.left + padding, rect.top + padding, rect.right - padding*2, rect.bottom - padding * 2,
		hwnd,
		NULL,
		hInstance,
		NULL
	);

	if (hEdit == NULL)
	{
		return 0;
	}

	char buf[1024];
	for (size_t i = 0; i < 1000; i++) {
		sprintf_s(buf, sizeof(buf), "Line %d\r\n", i);
		appendTextToEdit(hEdit, buf);
	}

	// Установка желтого текста
	//HFONT hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
	//	CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	//SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

	// Отображение окна
	ShowWindow(hwnd, SW_SHOW);

	// Цикл обработки сообщений
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CTLCOLOREDIT: {
		HDC hdc = (HDC)wParam;
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255, 0, 0));  // yourColor is a WORD and it's format is 0x00BBGGRR
		SetDCBrushColor(hdc, RGB(0, 0, 0));
		return (LRESULT)GetStockObject(DC_BRUSH); // return a DC brush.
	}

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}