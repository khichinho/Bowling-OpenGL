#ifndef RECT_H
#define RECT_H

#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

typedef struct _RECT{
	float x;
	float y;
	float width;
	float height;

	GLuint pos;
	GLuint color;
	GLuint list;
}Rect;

Rect* Rect_new(float x, float y, float width, float height);
void Rect_draw(Rect* self);
void Rect_destroy(Rect* self);

Rect* Rect_new(float x, float y, float width, float height){

	Rect* self = (Rect*)malloc(sizeof(Rect));
	self->x = x;
	self->y = y ;
	self->width = width;
	self->height = height;

	float position[] = {
		x, y,
		x + width, y,
		x + width, y + height,
		x, y +height
	};

	glGenBuffers(1, &(self->pos) );
	glBindBuffer(GL_ARRAY_BUFFER, self->pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, &position, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	float colors[] = {
		0,0,255,
		0,255,0,
		255,0,0,
		255,255,255
	};

	glGenBuffers(1, &(self->color) );
	glBindBuffer(GL_ARRAY_BUFFER, self->color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*12, &colors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	int indices[] = {
		0, 1, 2,
		0, 3, 2
	};

	glGenBuffers(1, &(self->list) );
	glBindBuffer(GL_ARRAY_BUFFER, self->list);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6, &indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return self;
}

void Rect_draw(Rect* self){
	glBindBuffer(GL_ARRAY_BUFFER, self->pos);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, self->color);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->list);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void Rect_destroy(Rect* self){
	glDeleteBuffers(1, &(self->pos) );
	glDeleteBuffers(1, &(self->color) );
	glDeleteBuffers(1, &(self->list) );
	free(self);
}
#endif