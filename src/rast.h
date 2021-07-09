#ifndef RAST_H
#define RAST_H
#include "tools.h"
#include "platform.h"
#include "geometry.h"
extern Platform p;

typedef struct RenderingContext
{
	u32 render_width, render_height;
	f32 *data; //render_width * render_height * sizeof(f32) * 4
	f32 *zbuf; //render_width * render_height * sizeof(f32)
	// maybe data + zbuf = FRAMEBUFFER??????
	b32 cull_faces; //perform backface culling
	b32 blend_func; //make the appropriate ENUM
    b32 render_mode;
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
    b32 steep = FALSE;
    if (abs(t0.x - t1.x)<abs(t0.y - t1.y))
    {
        swap_ints(&t0.x,&t0.y);
        swap_ints(&t1.x, &t1.y);
        steep = TRUE;
    }
    if (t0.x > t1.x)
        ivec2_swap(&t0, &t1);
    i32 dx = t1.x - t0.x;
    i32 dy = t1.y - t0.y;
    //how much steep accumulation we get per ++x
    i32 derror = abs(dy)*2;
    i32 err = 0;
    i32 y = t0.y;
	for (i32 x = t0.x; x <= t1.x; ++x)
	{
        u32 index;
        if (steep)
            index = y + x * rc->render_width;
        else
            index = x + y * rc->render_width;
        err += derror;
        if (err > dx)
        {
            y += (t1.y > t0.y ? 1 : -1);
            err -= dx*2;
        }
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

    sphere_data = gen_sphere_data(1.f, 4, 4);
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
	rend_clear(&rc, v4(0,0,0,1));

    ivec2 origin = (ivec2){rc.render_width / 2, rc.render_height / 2};
    ivec2 dest = (ivec2){origin.x + cos(p.current_time)*rc.render_width/4.f, origin.y + sin(p.current_time)*rc.render_width /4.f};
    rend_line(&rc, origin, dest, v4(1,1,1,1));

    for (u32 i = 0; i < 6; i+=3)
    {
        ivec2 v0 = (ivec2){(quad_verts[i].pos.x + 1.f) * 100, (quad_verts[i].pos.y + 1.f) * 100};
        ivec2 v1 = (ivec2){(quad_verts[i+1].pos.x + 1.f) * 100, (quad_verts[i+1].pos.y + 1.f) * 100};
        ivec2 v2 = (ivec2){(quad_verts[i+2].pos.x + 1.f) * 100, (quad_verts[i+2].pos.y + 1.f) * 100};
        rend_line(&rc, v0, v1, v4(1,0,0,1));
        rend_line(&rc, v1, v2, v4(0,1,0,1));
        rend_line(&rc, v2, v0, v4(0,0,1,1));
    }

    for (u32 i = 0; i < sphere_verts_count; i+=3)
    {
        ivec2 v0 = (ivec2){(sphere_data[i].pos.x + 2.f) * 100, (sphere_data[i].pos.y + 2.f) * 100};
        ivec2 v1 = (ivec2){(sphere_data[i+1].pos.x + 2.f) * 100, (sphere_data[i+1].pos.y + 2.f) * 100};
        ivec2 v2 = (ivec2){(sphere_data[i+2].pos.x + 2.f) * 100, (sphere_data[i+2].pos.y + 2.f) * 100};
        rend_line(&rc, v0, v1, v4(1,0,0,1));
        rend_line(&rc, v1, v2, v4(0,1,0,1));
        rend_line(&rc, v2, v0, v4(0,0,1,1));
    }

}
#endif
