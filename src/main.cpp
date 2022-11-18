#define BX_CONFIG_DEBUG 1
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <glfw3.h>


#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

#include <string>
#include <cstdio>

#include "MemPool.h"

static int WIN_WIDTH = 1920;
static int WIN_HEIGHT = 1080;


void WindowResizeCback(GLFWwindow* window, int w, int h)
{

	WIN_WIDTH = w;
	WIN_HEIGHT = h;
	bgfx::reset(w, h, BGFX_RESET_VSYNC);
	bgfx::setViewRect(0, 0, 0, w, h);
}

struct KeyInput
{
	bool isPressed, isHeld;
};


KeyInput KEYS_PRESSED[GLFW_KEY_LAST];


void WindowKeyCback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	default:
		bool isDown = action == GLFW_RELEASE ? false : true;

		KeyInput& input = KEYS_PRESSED[key];
		if (isDown)
		{
			input.isPressed = true;
			if (input.isPressed) input.isHeld = true;
		}
		else
		{
			input.isHeld = false;
			input.isPressed = false;
		}
	}
}
bgfx::ShaderHandle loadShader(const char* FILENAME)
{
	const char* shaderPath = "shaders/";

	size_t shaderLen = strlen(shaderPath);
	size_t fileLen = strlen(FILENAME);
	char* filePath = (char*)calloc(1, shaderLen + fileLen + 1);
	if (!filePath) return bgfx::createShader(0);
	memcpy(filePath, shaderPath, shaderLen);
	memcpy(&filePath[shaderLen], FILENAME, fileLen);

	FILE* file = fopen(filePath, "rb");
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	const bgfx::Memory* mem = bgfx::alloc(fileSize + 1);
	fread(mem->data, 1, fileSize, file);
	mem->data[mem->size - 1] = '\0';
	fclose(file);

	return bgfx::createShader(mem);
}

struct PosColorVertex
{
	float x;
	float y;
	float z;
	uint32_t abgr;
};

static PosColorVertex cubeVertices[] =
{
	{-1.0f,  1.0f,  1.0f, 0xff000000 },
	{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
	{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
	{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
	{-1.0f,  1.0f, -1.0f, 0xffff0000 },
	{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
	{-1.0f, -1.0f, -1.0f, 0xffffff00 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeTriList[] =
{
	0, 1, 2,
	1, 3, 2,
	4, 6, 5,
	5, 6, 7,
	0, 2, 4,
	4, 2, 6,
	1, 5, 3,
	5, 7, 3,
	0, 4, 1,
	4, 5, 1,
	2, 3, 6,
	6, 3, 7,
};

void DoDebugCube(GLFWwindow* window)
{

	bgfx::VertexLayout layout;
	layout.begin();
	layout.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);
	layout.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true);
	layout.end();
	bgfx::VertexBufferHandle vbo = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), layout);
	bgfx::IndexBufferHandle ibo = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));



	bgfx::ShaderHandle vsHandle = loadShader("vs_cubes.bin");
	bgfx::ShaderHandle fsHandle = loadShader("fs_cubes.bin");
	bgfx::ProgramHandle program = bgfx::createProgram(vsHandle, fsHandle, true);
	unsigned int counter = 0;

	while (!glfwWindowShouldClose(window))
	{
		bgfx::touch(0);
		const bx::Vec3 at = { 0.0f, 0.0f,  0.0f };
		const bx::Vec3 eye = { 0.0f, 0.0f, -5.0f };
		float view[16];
		bx::mtxLookAt(view, eye, at);
		float proj[16];
		bx::mtxProj(proj, 60.0f, float(WIN_WIDTH) / float(WIN_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
		bgfx::setViewTransform(0, view, proj);

		float mtx[16];
		bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
		bgfx::setTransform(mtx);

		bgfx::setVertexBuffer(0, vbo);
		bgfx::setIndexBuffer(ibo);

		bgfx::submit(0, program);
		counter++;


		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F1 to toggle stats.");
		bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");
		bgfx::dbgTextPrintf(80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
		bgfx::dbgTextPrintf(80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");
		bgfx::dbgTextPrintf(80, 2, 0x0f, "Counter = %d", counter);
		const bgfx::Stats* stats = bgfx::getStats();
		bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.", stats->width, stats->height, stats->textWidth, stats->textHeight);
		bgfx::setDebug(BGFX_DEBUG_TEXT);
		bgfx::frame();
		counter++;
		glfwPollEvents();
	}
}

struct Asteroid
{
	union
	{
		struct
		{
			float x, y;
			float xVel, yVel;

			//bgfx::VertexBufferHandle mesh;
			//bgfx::IndexBufferHandle indices;
		};

		Asteroid* next;
	};
};



int main()
{
	if (!glfwInit()) return -1;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Hello World!", nullptr, nullptr);
	

	glfwSetWindowSizeCallback(window, &WindowResizeCback);
	glfwSetKeyCallback(window, &WindowKeyCback);


	bgfx::Init init;
	
	bgfx::PlatformData pd{};
	pd.nwh = glfwGetWin32Window(window);
	//pd.context = glfwGetCurrentContext();
	init.platformData = pd;
	
	
	
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	init.resolution.width = (uint32_t)width;
	init.resolution.height = (uint32_t)height;
	init.resolution.reset = BGFX_RESET_VSYNC;
	init.type = bgfx::RendererType::OpenGL;
	if (!bgfx::init(init)) return -1;
	


	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, width, height);

	
	/*
	MemPool<Asteroid> asteroidPool = AllocatePool<Asteroid>(1000);
	
	
	
	
	
	*/
	
	//DoDebugCube(window);
	MemPool<Asteroid> asteroidPool = AllocatePool<Asteroid>(1000);
	Asteroid* a = asteroidPool.Allocate(Asteroid{ 10.0f, 25.0f, 0.0125f, 6.891f });
	Asteroid* b = asteroidPool.Allocate(Asteroid{ 15.0f, -25.0f, 3.14f, -100.0f });
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		bgfx::touch(0);
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F1 to toggle stats.");
		bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");
		bgfx::dbgTextPrintf(80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
		bgfx::dbgTextPrintf(80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");
		const bgfx::Stats* stats = bgfx::getStats();
		bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.", stats->width, stats->height, stats->textWidth, stats->textHeight);
		bgfx::dbgTextPrintf(0, 5, 0x0f, "Stats of asteroid: x:%f, y:%f, xVel:%f, yVel:%f", a->x, a->y, a->xVel, a->yVel);
		bgfx::dbgTextPrintf(0, 10, 0x0f, "Stats of asteroid: x:%f, y:%f, xVel:%f, yVel:%f", b->x, b->y, b->xVel, b->yVel);
		bgfx::setDebug(BGFX_DEBUG_TEXT);




		//Update plyaer
		//Update Asteroids


		bgfx::frame();


	}

	bgfx::shutdown();
	glfwTerminate();

}