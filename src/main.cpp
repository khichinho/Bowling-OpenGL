#include <stdio.h>
#include <stdlib.h>

#include "window.h"
#include "rect.h"
#include "ball.h"

#include <GLFW/glfw3.h>

Rect* plane;
Ball* bowling;

void game_loop(float delta){
	glClearColor(0.0f,0.0f,0.0f,1);
	glClear(GL_COLOR_BUFFER_BIT);
	// Rect_draw(plane);
	Ball_draw(bowling);
}

int main(int argc, char* argv[]){

	if(!Window_init(800,600,"BowlingOpenGL")){
		printf("Problem loading window\n");
		return 1;
	}

	printf("yes");

	// plane = Rect_new(100,100,100,100);
	bowling = Ball_new(100,100,50);
	while(!Window_close()){
		Window_update(game_loop);
	}

	// Rect_destroy(plane);
	Ball_destroy(bowling);
	Window_destroy();
	return 0;
}