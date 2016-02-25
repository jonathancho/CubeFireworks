#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <gl/gl.h>
//#include <stddef.h>
//#include <mmsystem.h>

const int clientWidth = 800;
const int clientHeight = 600;

float  g_fElapsedTime = 0.0f;
float  g_dCurrentTime = 0.0f;
float  g_dLastTime = 0.0f;

HWND Hwnd = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	//SetupConsole();

	WNDCLASSEX winClass;
	MSG        uMsg;

	memset(&uMsg, 0, sizeof(uMsg));

	winClass.lpszClassName = L"CubeFireworks";
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	winClass.lpfnWndProc = WindowProc;
	winClass.hInstance = hInstance;
	winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName = NULL;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;

	if (!RegisterClassEx(&winClass))
		return E_FAIL;


	Hwnd = CreateWindowEx(NULL, L"CubeFireworks", L"Cube Fireworks",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, //WS_POPUPWINDOW | WS_VISIBLE,WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, clientWidth, clientHeight, NULL, NULL, hInstance, NULL);//					     
	if (Hwnd == NULL)
		return E_FAIL;

	ShowWindow(Hwnd, nCmdShow);
	UpdateWindow(Hwnd);

	//Init();

	while (uMsg.message != WM_QUIT)
	{
		if (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&uMsg);
			DispatchMessage(&uMsg);
		}
		else
		{
			g_dCurrentTime = (double)timeGetTime();
			g_fElapsedTime = (float)((g_dCurrentTime - g_dLastTime) * 0.001);
			g_dLastTime = g_dCurrentTime;
			//UpdateScene(g_fElapsedTime);
			//Render();
		}
	}

	//ShutDown();


	UnregisterClass(L"ModelViewerClass", winClass.hInstance);

	return uMsg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
	}
	return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
