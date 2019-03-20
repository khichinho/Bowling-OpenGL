#ifndef WINDOW_H
#define WINDOW_H

#include <stdio.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GLFW/glfw3.h>

static GLFWwindow* window;

static double last = 0.0;
static double now = 0.0;
static float delta = 1.0f;

int Window_init(int width, int height, char* title);
void Window_update(void (*update)(float));
int Window_close();
int Window_destroy();

int Window_init(int width, int height, char* title){
	if(!glfwInit()){
		return 0;
	}

	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if(!window){
		return 0;
	}

	glfwMakeContextCurrent(window);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	return 1;
}

void Window_update(void (*update)(float)){
	last = glfwGetTime();

	(update)(delta);

	glfwSwapBuffers(window);
	glfwPollEvents();

	now = glfwGetTime();
	delta = (float)(now - last)*10.0f;
}

int Window_close(){
	return glfwWindowShouldClose(window);
}

int Window_destroy(){
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


#endif