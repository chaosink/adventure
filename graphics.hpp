#ifndef __GRAPHICS__
#define __GRAPHICS__

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ResizeWindow(GLFWwindow* window, int width, int height);
int LoadBMP(const char * imagepath);
void LoadTexture();
void InitGL();
void Render(GLFWwindow* window);

#endif
