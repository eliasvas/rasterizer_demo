#ifndef RAST_H
#define RAST_H
#include "tools.h"
#include "platform.h"
extern Platform p;

typedef struct Vertex
{
	vec3 pos;
	vec3 norm;
	vec3 texcoord;
}Vertex;

/*
Vertex quad_vertices[6] = {
	(Vertex){v3(-0.5,-0.5,0),v3(0,0,-1),v2(0,0)},
	(Vertex){v3(0.5,-0.5,0),v3(0,0,-1),v2(1,0)},
	(Vertex){v3(0.5,0.5,0),v3(0,0,-1),v2(1,1)},
	(Vertex){v3(0.5,0.5,0),v3(0,0,-1),v2(1,1)},
	(Vertex){v3(-0.5,0.5,0),v3(0,0,-1),v2(0,1)},
	(Vertex){v3(-0.5,-0.5,0),v3(0,0,-1),v2(0,0)}
};
*/

typedef struct RenderingContext
{
	u32 render_width, render_height;
	f32 *data; //render_width * render_height * sizeof(f32) * 4
	f32 *zbuf; //render_width * render_height * sizeof(f32)
	// maybe data + zbuf = FRAMEBUFFER??????
	b32 cull_faces; //perform backface culling
	b32 blend_func; //make the appropriate ENUM
	//maybe add double buffering??? idk
}RenderingContext;

global RenderingContext rc;

internal void rend_clear(RenderingContext *rc, vec4 clear_color)
{
	for (u32 i = 0; i < rc->render_width * rc->render_height; ++i)
	{
		//clear fbo color info
		rc->data[4 * i + 0] = clear_color.elements[0];
		rc->data[4 * i + 1] = clear_color.elements[1];
		rc->data[4 * i + 2] = clear_color.elements[2];
		rc->data[4 * i + 3] = clear_color.elements[3];
		//clear fbo depth info
		rc->zbuf[i] = -1.f;
	}

}
internal void rend_line(RenderingContext *rc, ivec2 t0, ivec2 t1, vec4 color)
{
	for (f32 i = 0.f; i < 1.f; i += 0.005)
	{
		u32 x = t0.x + i * (t1.x - t0.x);
		u32 y = t0.y + i * (t1.y - t0.y);
		u32 index = x + y * rc->render_width;
		rc->data[4 * index + 0] = color.elements[0];
		rc->data[4 * index + 1] = color.elements[1];
		rc->data[4 * index + 2] = color.elements[2];
		rc->data[4 * index + 3] = color.elements[3];
	}
}

internal void init(void)
{
	rc = (RenderingContext){0};
	rc.render_width = p.window_width;
	rc.render_height = p.window_height;
	rc.data = (f32*)malloc(sizeof(vec4) * rc.render_width * rc.render_height);
	rc.zbuf = (f32*)malloc(sizeof(f32) * rc.render_width * rc.render_height);
	rc.cull_faces = FALSE;
	rc.blend_func = FALSE; //no blending!
	
}

internal void update(f32 dt)
{
	//resize the wios
    if (p.window_width != rc.render_width || p.window_height != rc.render_height)
	{
		init(); //TODO change later :)
	}
}

internal void render(void)
{
	rend_clear(&rc, v4(fabs(sin(p.current_time / 10.f)),0.1f,0.3f,1.f));
rend_line(&rc, (ivec2){0,0}, (ivec2){(i32)rc.render_width,(i32)rc.render_height}, v4(1,1,1,1));
}
#endif
