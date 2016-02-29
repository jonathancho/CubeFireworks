#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include "Cube.h"


const int clientWidth = 800;
const int clientHeight = 600;

const float CAMERA_DISTANCE = 5.0f;

const int STARTING_LIVES = 3;
const int NUM_PER_EXPLOSION = 3;

float  g_fElapsedTime = 0.0f;
float  g_dCurrentTime = 0.0f;
float  g_dLastTime = 0.0f;

//int curExplosions = 0;
//int maxExplosions = 1;

bool vboSupported;
GLuint vboID = 0;

GLuint vbo_cube_vertices; 
GLuint vbo_cube_colors;
GLuint ibo_cube_elements;

GLint attribute_coord3d;
GLint attribute_v_color;
GLint uniform_mvp;

GLuint vertexBuffer;
GLuint colorBuffer;

GLuint shaderProgram;

glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
glm::mat4 View = glm::lookAt(
	glm::vec3(0, 0, -20),
	glm::vec3(0, 0, 0), // and looks at the origin
	glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

HWND Hwnd = NULL;

std::vector<Cube> cubes;

// Function Declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC);
void ShutDownOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
void UpdateScene(float dt);
void Render(float& theta, HDC hDC);
std::string loadFile(const char *fname);

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


	UnregisterClass(L"CubeFireworks", winClass.hInstance);

	return uMsg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static POINT ptCurrentMousePosit;

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
	break;

	case WM_LBUTTONUP:
	{
		ptCurrentMousePosit.x = LOWORD(lParam);
		ptCurrentMousePosit.y = HIWORD(lParam);

		double x = 2.0 * ptCurrentMousePosit.x / clientWidth - 1;
		double y = -2.0 * ptCurrentMousePosit.y / clientHeight + 1;
		glm::mat4 viewProjectionInverse = glm::inverse(Projection * View);

		glm::vec4 mousePosWorld = viewProjectionInverse * glm::vec4(x, y, 1.0f, 1);
		mousePosWorld.w = 1.0f / mousePosWorld.w;

		Cube newCube;
		newCube.trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -10.0f, 0.0f));
		newCube.targetPos = glm::vec3(mousePosWorld.x * mousePosWorld.w, mousePosWorld.y * mousePosWorld.w, mousePosWorld.z * mousePosWorld.w);
		newCube.lives = STARTING_LIVES;

		cubes.push_back(newCube);
	}
	break;
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

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};

	// One color for each vertex. They were generated randomly.
	static const GLfloat g_color_buffer_data[] = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
	};

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

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

	static float theta = 45.0f;

	for (int i = 0; i < cubes.size(); ++i)
	{
		// Model matrix : an identity matrix (model will be at the origin)

		cubes[i].rot = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(0.0f, 1.0f, 0.0f));

		cubes[i].currentPos = glm::mix(cubes[i].startingPos, cubes[i].targetPos, cubes[i].tValue);

		cubes[i].trans = glm::translate(glm::mat4(1.0f), cubes[i].currentPos);

		glm::mat4 Model = cubes[i].trans * cubes[i].rot;
		// Our ModelViewProjection : multiplication of our 3 matrices
		//glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		cubes[i].MVP = Projection * View * Model;

		cubes[i].tValue += 0.01f;

		if (cubes[i].tValue >= 1.0f)
		{
			if (--cubes[i].lives > 0)
			{
					glm::vec3 randVec1 = glm::sphericalRand(20.0f);
					glm::vec3 randVec2 = glm::sphericalRand(20.0f);

					Cube cube1;
					cube1.currentPos = cubes[i].currentPos;
					cube1.startingPos = cubes[i].currentPos;
					cube1.targetPos = glm::vec3(cubes[i].currentPos.x + randVec1.x, cubes[i].currentPos.y + randVec1.y, cubes[i].currentPos.z);
					cube1.lives = cubes[i].lives;

					Cube cube2;
					cube2.currentPos = cubes[i].currentPos;
					cube2.startingPos = cubes[i].currentPos;
					cube2.targetPos = glm::vec3(cubes[i].currentPos.x + randVec2.x, cubes[i].currentPos.y + randVec2.y, cubes[i].currentPos.z);
					cube2.lives = cubes[i].lives;

					cubes.push_back(cube1);
					cubes.push_back(cube2);
			}
			cubes.erase(cubes.begin() + i);
		}
	}

	theta += 0.02f;

	return;
}

void Render(float& theta, HDC hDC)
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(shaderProgram);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);


	for (GLuint i = 0; i < cubes.size(); ++i)
	{
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, &cubes[i].MVP[0][0]);


		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles
	}
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

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

//glm::vec3 get3dPoint(glm::vec2 point2D, int width,
//	int height, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
//
//	double x = 2.0 * winX / clientWidth - 1;
//	double y = -2.0 * winY / clientHeight + 1;
//	Matrix4 viewProjectionInverse = inverse(projectionMatrix *
//		viewMatrix);
//
//	Point3D point3D = new Point3D(x, y, 0);
//	return viewProjectionInverse.multiply(point3D);
//}