#ifndef _SPRITE_H
#define _SPRITE_H



typedef struct
{
	float x, y; // current position
	int width, height; // dimensions
	float xspeed, yspeed; // current speed
	char *map; // the pixmap
} Sprite;


Sprite *create_sprite(char*pic[], unsigned short xi, unsigned short yi, unsigned short hor, short delta, unsigned short time);

void destroy_sprite(Sprite* sp);

void draw_sprite(Sprite* sp);

void clear_sprite (Sprite* sp);


#endif
