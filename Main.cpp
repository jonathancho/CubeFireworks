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

// Function Declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC);
void ShutDownOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
void UpdateScene(float dt);
void Render(float& theta, HDC hDC);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	//SetupConsole();

	WNDCLASSEX winClass;
	MSG        uMsg;
	HDC		   hDC;
	HGLRC	   hRC;
	float theta = 0.0f;

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

	InitOpenGL(Hwnd, &hDC, &hRC);

	while (uMsg.message != WM_QUIT)
	{
		if (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&uMsg);
			DispatchMessage(&uMsg);
		}
		else
		{
			g_dCurrentTime = (float)timeGetTime();
			g_fElapsedTime = (float)((g_dCurrentTime - g_dLastTime) * 0.001);
			g_dLastTime = g_dCurrentTime;
			UpdateScene(g_fElapsedTime);
			Render(theta, hDC);
		}
	}

	ShutDownOpenGL(Hwnd, hDC, hRC);


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

void InitOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;

	// get the device context (DC)
	*hDC = GetDC(hWnd);

	// set the pixel format for the DC
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat(*hDC, &pfd);
	SetPixelFormat(*hDC, format, &pfd);

	// create and enable the render context (RC)
	*hRC = wglCreateContext(*hDC);
	wglMakeCurrent(*hDC, *hRC);
}

void ShutDownOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hWnd, hDC);
}

void UpdateScene(float dt)
{
	// Update scene based on user input
	return;
}

void Render(float& theta, HDC hDC)
{
	// OpenGL animation code goes here

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glRotatef(theta, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(0.0f, 1.0f);
	glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(0.87f, -0.5f);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(-0.87f, -0.5f);
	glEnd();
	glPopMatrix();

	SwapBuffers(hDC);

	theta += 1.0f;
}