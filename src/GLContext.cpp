#include "GLContext.h"


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    GLContext* context = static_cast<GLContext*>(glfwGetWindowUserPointer(window));
    glfwGetWindowSize(window, &context->SCR_WIDTH, &context->SCR_HEIGHT);
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        GLContext* context = static_cast<GLContext*>(glfwGetWindowUserPointer(window));
        ((void(*)(int))context->onInput)(key);
    }
}


int GLContext::init(const char* windowName,int width , int height, void(*onDraw)(), void(*onInput)(int)) {

    window_name = windowName;
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    this->onDraw = onDraw;
    this->onInput = onInput;
    glfwInit();
    if (fullscreen) {window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, window_name, glfwGetPrimaryMonitor(), NULL);}
    else {window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, window_name, NULL, NULL);}
    if (window == NULL)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << endl;
        return -1;
    }
    glfwSetWindowUserPointer(window, static_cast<void*>(this));
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    cout << "OpenGL version : " << glGetString(GL_VERSION) << endl;
    return 0;
}

void GLContext::updateFullScreen() {

    if (fullScreenUpdate == fullscreen) {
        return;
    }

    if (fullscreen)
    {
        // backup window position and window size
        glfwGetWindowPos(window, &oldX, &oldY);
        glfwGetWindowSize(window, &oldWidth, &oldHeight);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        // get resolution of monitor
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        // switch to full screen
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0);
    }
    else
    {
        // restore last window size and position
        glfwSetWindowMonitor(window, nullptr, oldX, oldY, oldWidth, oldHeight, 0);
    }
    fullScreenUpdate = !fullScreenUpdate;
}

void GLContext::renderLoop() {
    while (!glfwWindowShouldClose(window)){
        updateFullScreen();
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (!alpha) { glClear(GL_COLOR_BUFFER_BIT); }
        onDraw();
        
    }
    glfwTerminate();
}


void GLContext::TakeScreenshot(string filePath) {
    std::vector<unsigned char> image(SCR_WIDTH * SCR_HEIGHT * 4);
    glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
    std::vector<unsigned char> PngBuffer;
    unsigned error = lodepng::encode(PngBuffer, image, SCR_WIDTH, SCR_HEIGHT);
    if (!error) { lodepng::save_file(PngBuffer, filePath); }
    if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}
