#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include "./glext.h"
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

GLuint vbo_cube_vertices; 
GLuint vbo_cube_colors;
GLuint ibo_cube_elements;

GLint attribute_coord3d;
GLint attribute_v_color;
GLint uniform_mvp;

GLuint shaderProgram;

HWND Hwnd = NULL;

// Function Declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC);
void ShutDownOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
void UpdateScene(float dt);
void Render(float& theta, HDC hDC);
std::string loadFile(const char *fname);

// function pointers for VBO Extension
// Windows needs to get function pointers from ICD OpenGL drivers,
// because opengl32.dll does not support extensions higher than v1.1.
//#ifdef _WIN32
//PFNGLGENBUFFERSARBPROC            pglGenBuffersARB = 0;             // VBO Name Generation Procedure
//PFNGLBINDBUFFERARBPROC            pglBindBufferARB = 0;             // VBO Bind Procedure
//PFNGLBUFFERDATAARBPROC            pglBufferDataARB = 0;             // VBO Data Loading Procedure
//PFNGLBUFFERSUBDATAARBPROC         pglBufferSubDataARB = 0;          // VBO Sub Data Loading Procedure
//PFNGLDELETEBUFFERSARBPROC         pglDeleteBuffersARB = 0;          // VBO Deletion Procedure
//PFNGLGETBUFFERPARAMETERIVARBPROC  pglGetBufferParameterivARB = 0;   // return various parameters of VBO
//PFNGLMAPBUFFERARBPROC             pglMapBufferARB = 0;              // map VBO procedure
//PFNGLUNMAPBUFFERARBPROC           pglUnmapBufferARB = 0;            // unmap VBO procedure
//
//#define glGenBuffersARB           pglGenBuffersARB
//#define glBindBufferARB           pglBindBufferARB
//#define glBufferDataARB           pglBufferDataARB
//#define glBufferSubDataARB        pglBufferSubDataARB
//#define glDeleteBuffersARB        pglDeleteBuffersARB
//#define glGetBufferParameterivARB pglGetBufferParameterivARB
//#define glMapBufferARB            pglMapBufferARB
//#define glUnmapBufferARB          pglUnmapBufferARB
//
//#endif

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
	//glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
	//glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
	//glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
	//glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
	//glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
	//glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
	//glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
	//glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");

	//// check if VBO extension is supported
	//if (glGenBuffersARB && glBindBufferARB && glBufferDataARB && glBufferSubDataARB &&
	//	glMapBufferARB && glUnmapBufferARB && glDeleteBuffersARB && glGetBufferParameterivARB)
	//{
	//	vboSupported = true;
	//	std::cout << "Video card supports GL_ARB_vertex_buffer_object." << std::endl;
	//}
	//else
	//{
	//	vboSupported = false;
	//	std::cout << "Video card does NOT support GL_ARB_vertex_buffer_object." << std::endl;
	//}

	//if (vboSupported)
	//{
	//	int bufferSize;
	//	glGenBuffersARB(1, &vboID);
	//	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboID);
	//	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(normals) + sizeof(colors), 0, GL_STATIC_DRAW_ARB);
	//	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), vertices);                             // copy vertices starting from 0 offest
	//	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(normals), normals);                // copy normals after vertices
	//	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices) + sizeof(normals), sizeof(colors), colors);  // copy colours after normals
	//}

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	GLfloat cube_vertices[] = {
		// front
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// back
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0,
	};
	glGenBuffers(1, &vbo_cube_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	GLfloat cube_colors[] = {
		// front colors
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0,
		// back colors
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0,
	};
	glGenBuffers(1, &vbo_cube_colors);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);

	GLushort cube_elements[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3,
	};
	glGenBuffers(1, &ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	GLint link_ok = GL_FALSE;

	std::string vertexShaderString = loadFile("cube.vert");
	std::string fragmentShaderString = loadFile("cube.frag");

	int vlen = vertexShaderString.length();
	int flen = fragmentShaderString.length();

	const char *vertexShaderCStr = vertexShaderString.c_str();
	const char *fragmentShaderCStr = fragmentShaderString.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, (const GLchar **)&vertexShaderCStr, &vlen);
	glShaderSource(fs, 1, (const GLchar **)&fragmentShaderCStr, &flen);

	glCompileShader(vs);
	glCompileShader(fs);

	//if ((vs = glCreateShader("cube.v.glsl", GL_VERTEX_SHADER)) == 0) return 0;
	//if ((fs = create_shader("cube.f.glsl", GL_FRAGMENT_SHADER)) == 0) return 0;

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		fprintf(stderr, "glLinkProgram:");
		//print_log(shaderProgram);
		return;
	}

	const char* attribute_name;
	attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(shaderProgram, attribute_name);
	if (attribute_coord3d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return;
	}
	attribute_name = "v_color";
	attribute_v_color = glGetAttribLocation(shaderProgram, attribute_name);
	if (attribute_v_color == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return;
	}
	const char* uniform_name;
	uniform_name = "mvp";
	uniform_mvp = glGetUniformLocation(shaderProgram, uniform_name);
	if (uniform_mvp == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
	glClearDepth(1.0f);                   // Set background depth to farthest
	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
	glShadeModel(GL_SMOOTH);   // Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
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

	// Get a handle for our "MVP" uniform
	//GLuint MatrixID = glGetUniformLocation(shaderProgram, "mvp");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(-4, 3, -10), 
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

	glUseProgram(shaderProgram);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, &MVP[0][0]);

	return;
}

void Render(float& theta, HDC hDC)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	glEnableVertexAttribArray(attribute_coord3d);
	// Describe our vertices array to OpenGL (it can't guess its format automatically)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glVertexAttribPointer(
		attribute_coord3d, // attribute
		3,                 // number of elements per vertex, here (x,y,z)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
		);

	glEnableVertexAttribArray(attribute_v_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);
	glVertexAttribPointer(
		attribute_v_color, // attribute
		3,                 // number of elements per vertex, here (R,G,B)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
		);

	/* Push each element in buffer_vertices to the vertex shader */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_v_color);
	//// OpenGL animation code goes here
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//glLoadIdentity();
	//glTranslatef(-1.5f, 0.0f, -6.0f);

	//glBegin(GL_TRIANGLES);                      // Drawing Using Triangles
	//glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(0.0f, 1.0f);              // Top
	//glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(0.87f, -0.5f);              // Bottom Left
	//glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(-0.87f, -0.5f);              // Bottom Right
	//glEnd();                            // Finished Drawing The Triangle

	//GLenum err = glGetError();

	//gltranslatef(3.0f, 0.0f, 0.0f);                   // move right 3 units

	//glbegin(gl_quads);                      // draw a quad
	//glvertex3f(-1.0f, 1.0f, 0.0f);              // top left
	//glvertex3f(1.0f, 1.0f, 0.0f);              // top right
	//glvertex3f(1.0f, -1.0f, 0.0f);              // bottom right
	//glvertex3f(-1.0f, -1.0f, 0.0f);              // bottom left
	//glend();                            // Done Drawing The Quad
	//if (vboSupported)
	//{
	//	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboID);

	//	// enable vertex arrays
	//	glEnableClientState(GL_NORMAL_ARRAY);
	//	glEnableClientState(GL_COLOR_ARRAY);
	//	glEnableClientState(GL_VERTEX_ARRAY);

	//	// before draw, specify vertex and index arrays with their offsets
	//	glNormalPointer(GL_FLOAT, 0, (void*)sizeof(vertices));
	//	glColorPointer(3, GL_FLOAT, 0, (void*)(sizeof(vertices) + sizeof(normals)));
	//	glVertexPointer(3, GL_FLOAT, 0, 0);

	//	glDrawArrays(GL_TRIANGLES, 0, 36);

	//	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	//	glDisableClientState(GL_COLOR_ARRAY);
	//	glDisableClientState(GL_NORMAL_ARRAY);

	//	// it is good idea to release VBOs with ID 0 after use.
	//	// Once bound with 0, all pointers in gl*Pointer() behave as real
	//	// pointer, so, normal vertex array operations are re-activated
	//	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	//}

	

	SwapBuffers(hDC);

	//theta += 1.0f;
}

// loadFile - loads text file into char* fname
// allocates memory - so need to delete after use
// size of file returned in fSize
std::string loadFile(const char *fname)
{
	std::ifstream file(fname);
	if (!file.is_open())
	{
		std::cout << "Unable to open file " << fname << std::endl;
		exit(1);
	}

	std::stringstream fileData;
	fileData << file.rdbuf();
	file.close();

	return fileData.str();
}
