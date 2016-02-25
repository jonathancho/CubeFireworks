#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <iostream>
#include <gl/gl.h>
#include "./glext.h"
//#include <stddef.h>
//#include <mmsystem.h>

const int clientWidth = 800;
const int clientHeight = 600;

const float CAMERA_DISTANCE = 5.0f;

float  g_fElapsedTime = 0.0f;
float  g_dCurrentTime = 0.0f;
float  g_dLastTime = 0.0f;

bool vboSupported;
GLuint vboID = 0;

HWND Hwnd = NULL;

// Function Declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC);
void ShutDownOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
void UpdateScene(float dt);
void Render(float& theta, HDC hDC);

GLfloat vertices[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,      // v0-v1-v2 (front)
					   -1,-1, 1,   1,-1, 1,   1, 1, 1,      // v2-v3-v0

					   1, 1, 1,   1,-1, 1,   1,-1,-1,      // v0-v3-v4 (right)
					   1,-1,-1,   1, 1,-1,   1, 1, 1,      // v4-v5-v0

					   1, 1, 1,   1, 1,-1,  -1, 1,-1,      // v0-v5-v6 (top)
					   -1, 1,-1,  -1, 1, 1,   1, 1, 1,      // v6-v1-v0

					   -1, 1, 1,  -1, 1,-1,  -1,-1,-1,      // v1-v6-v7 (left)
					   -1,-1,-1,  -1,-1, 1,  -1, 1, 1,      // v7-v2-v1

					   -1,-1,-1,   1,-1,-1,   1,-1, 1,      // v7-v4-v3 (bottom)
					   1,-1, 1,  -1,-1, 1,  -1,-1,-1,      // v3-v2-v7

					   1,-1,-1,  -1,-1,-1,  -1, 1,-1,      // v4-v7-v6 (back)
					   -1, 1,-1,   1, 1,-1,   1,-1,-1 };    // v6-v5-v4

															// normal array
GLfloat normals[] = { 0, 0, 1,   0, 0, 1,   0, 0, 1,      // v0-v1-v2 (front)
					  0, 0, 1,   0, 0, 1,   0, 0, 1,      // v2-v3-v0

					  1, 0, 0,   1, 0, 0,   1, 0, 0,      // v0-v3-v4 (right)
					  1, 0, 0,   1, 0, 0,   1, 0, 0,      // v4-v5-v0

					  0, 1, 0,   0, 1, 0,   0, 1, 0,      // v0-v5-v6 (top)
					  0, 1, 0,   0, 1, 0,   0, 1, 0,      // v6-v1-v0

					  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,      // v1-v6-v7 (left)
					  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,      // v7-v2-v1

					  0,-1, 0,   0,-1, 0,   0,-1, 0,      // v7-v4-v3 (bottom)
					  0,-1, 0,   0,-1, 0,   0,-1, 0,      // v3-v2-v7

					  0, 0,-1,   0, 0,-1,   0, 0,-1,      // v4-v7-v6 (back)
					  0, 0,-1,   0, 0,-1,   0, 0,-1 };    // v6-v5-v4

// color array
GLfloat colors[] = { 1, 1, 1,   1, 1, 0,   1, 0, 0,      // v0-v1-v2 (front)
					 1, 0, 0,   1, 0, 1,   1, 1, 1,      // v2-v3-v0

					 1, 1, 1,   1, 0, 1,   0, 0, 1,      // v0-v3-v4 (right)
					 0, 0, 1,   0, 1, 1,   1, 1, 1,      // v4-v5-v0

					 1, 1, 1,   0, 1, 1,   0, 1, 0,      // v0-v5-v6 (top)
					 0, 1, 0,   1, 1, 0,   1, 1, 1,      // v6-v1-v0

					 1, 1, 0,   0, 1, 0,   0, 0, 0,      // v1-v6-v7 (left)
					 0, 0, 0,   1, 0, 0,   1, 1, 0,      // v7-v2-v1

					 0, 0, 0,   0, 0, 1,   1, 0, 1,      // v7-v4-v3 (bottom)
					 1, 0, 1,   1, 0, 0,   0, 0, 0,      // v3-v2-v7

					 0, 0, 1,   0, 0, 0,   0, 1, 0,      // v4-v7-v6 (back)
					 0, 1, 0,   0, 1, 1,   0, 0, 1 };    // v6-v5-v4

// function pointers for VBO Extension
// Windows needs to get function pointers from ICD OpenGL drivers,
// because opengl32.dll does not support extensions higher than v1.1.
#ifdef _WIN32
PFNGLGENBUFFERSARBPROC            pglGenBuffersARB = 0;             // VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC            pglBindBufferARB = 0;             // VBO Bind Procedure
PFNGLBUFFERDATAARBPROC            pglBufferDataARB = 0;             // VBO Data Loading Procedure
PFNGLBUFFERSUBDATAARBPROC         pglBufferSubDataARB = 0;          // VBO Sub Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC         pglDeleteBuffersARB = 0;          // VBO Deletion Procedure
PFNGLGETBUFFERPARAMETERIVARBPROC  pglGetBufferParameterivARB = 0;   // return various parameters of VBO
PFNGLMAPBUFFERARBPROC             pglMapBufferARB = 0;              // map VBO procedure
PFNGLUNMAPBUFFERARBPROC           pglUnmapBufferARB = 0;            // unmap VBO procedure

#define glGenBuffersARB           pglGenBuffersARB
#define glBindBufferARB           pglBindBufferARB
#define glBufferDataARB           pglBufferDataARB
#define glBufferSubDataARB        pglBufferSubDataARB
#define glDeleteBuffersARB        pglDeleteBuffersARB
#define glGetBufferParameterivARB pglGetBufferParameterivARB
#define glMapBufferARB            pglMapBufferARB
#define glUnmapBufferARB          pglUnmapBufferARB

#endif

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

	// get pointers to GL functions
	glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
	glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
	glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
	glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
	glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
	glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
	glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
	glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");

	// check if VBO extension is supported
	if (glGenBuffersARB && glBindBufferARB && glBufferDataARB && glBufferSubDataARB &&
		glMapBufferARB && glUnmapBufferARB && glDeleteBuffersARB && glGetBufferParameterivARB)
	{
		vboSupported = true;
		std::cout << "Video card supports GL_ARB_vertex_buffer_object." << std::endl;
	}
	else
	{
		vboSupported = false;
		std::cout << "Video card does NOT support GL_ARB_vertex_buffer_object." << std::endl;
	}

	if (vboSupported)
	{
		int bufferSize;
		glGenBuffersARB(1, &vboID);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboID);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(normals) + sizeof(colors), 0, GL_STATIC_DRAW_ARB);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), vertices);                             // copy vertices starting from 0 offest
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(normals), normals);                // copy normals after vertices
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(normals), sizeof(colors), colors);  // copy colours after normals
	}
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
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glPushMatrix();

	glTranslatef(0, 0, -CAMERA_DISTANCE);
	glRotatef(0, 1, 0, 0);   // pitch
	glRotatef(0, 0, 1, 0);   // heading

	if (vboSupported)
	{
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboID);

		// enable vertex arrays
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);

		// before draw, specify vertex and index arrays with their offsets
		glNormalPointer(GL_FLOAT, 0, (void*)sizeof(vertices));
		glColorPointer(3, GL_FLOAT, 0, (void*)(sizeof(vertices) + sizeof(normals)));
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		// it is good idea to release VBOs with ID 0 after use.
		// Once bound with 0, all pointers in gl*Pointer() behave as real
		// pointer, so, normal vertex array operations are re-activated
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}

	glPopMatrix();

	SwapBuffers(hDC);

	//theta += 1.0f;
}