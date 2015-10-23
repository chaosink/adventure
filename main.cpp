#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "adventure.hpp"
#include "graphics.hpp"
using namespace std;

GLFWwindow* window;
Castle* castle_ptr;

int main(void)
{
	int k_room, size;
	cout << "Please input the side length of the castle: ";
	cin >> size;
	cout << "Please input the number of the rooms in the castle: ";
	cin >> k_room;
	getchar();
	cout << endl;
	Castle castle(k_room, size);
	castle_ptr = &castle;
//	Castle castle(125, 5);
//	Castle castle(64, 4);
//	Castle castle(8, 2);
//	Castle castle(8, 0, 1, 0, 1, 0, 1);
//	castle.Print();

	int samples = 10;
	int width = 1024, height = 768;
	float ratio = width / (float) height;

	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_SAMPLES, samples);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
//	glfwWindowHint(GLFW_FOCUSED, GL_FALSE);
//	glfwWindowHint(GLFW_DECORATED, GL_FALSE);
//	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
	window = glfwCreateWindow(width, height, "adventure", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetFramebufferSizeCallback(window, ResizeWindow);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

/*	if (!glfwExtensionSupported("GL_ARB_multisample"))
	{
		printf("GL_ARB_multisample extension not supported\n");

		glfwTerminate();
		exit(EXIT_FAILURE);
	}*/

	InitGL();
	ResizeWindow(window, width, height);
	castle.Run();

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
