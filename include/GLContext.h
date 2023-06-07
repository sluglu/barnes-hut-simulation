#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "lodepng.h"
#include <iostream>


using namespace std;

class GLContext {
private:
	int oldWidth = 1000;
	int oldHeight = 1000;
	int oldX = 500;
	int oldY = 500;
	bool fullScreenUpdate = false;
public:
	const char* window_name = "";
	bool alpha = false;  
	bool fullscreen = false;
	int SCR_WIDTH = 1000;
	int SCR_HEIGHT = 1000;
	GLFWwindow* window;
	int init(const char*,int, int, void(*)(), void(*)(int));
	void updateFullScreen();
	void renderLoop();
	void TakeScreenshot(string);
	void(*onInput)(int) {};
	void(*onDraw)() {};
	
};

