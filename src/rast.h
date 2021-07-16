#ifndef RAST_H
#define RAST_H
#include "tools.h"
#include "platform.h"
#include "texture.h"
#include "geometry.h"
#include "camera.h"

/*
		TODO:
	
	-blending
	
	-replace cstdlib functions (sin,tan...)
*/

extern Platform p;

typedef enum RenderMode
{
	TRIANGLE_MODE = 1,
	LINE_MODE,
	TRIANGLE_STRIP_MODE,
	LINE_STRIP_MODE,
} RenderMode;

typedef enum RenderSettings
{
	CULL_FACES = 0x1,
	ZBUF_ON = 0x2,
} RenderSettings;

typedef struct RenderingContext
{
	u32 render_width, render_height;
	f32 *data; //render_width * render_height * sizeof(f32) * 4
	f32 *zbuf; //render_width * render_height * sizeof(f32)
	b32 blend_func; //make the appropriate ENUM
    RenderMode render_mode;
	RenderSettings render_settings;
	//maybe add double buffering??? idk
	Camera cam;
	mat4 proj;
}RenderingContext;

vec3 light_dir = {0.2f, 0.5f, -1.f};

global RenderingContext rc;
Texture tex;

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
		rc->zbuf[i] = 1.f;
	}

}
internal void rend_line(ivec2 t0, ivec2 t1, vec4 color)
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
            index = y + x * rc.render_width;
        else
            index = x + y * rc.render_width;
        err += derror;
        if (err > dx)
        {
            y += (t1.y > t0.y ? 1 : -1);
            err -= dx*2;
        }
		rc.data[4 * index + 0] = color.elements[0];
		rc.data[4 * index + 1] = color.elements[1];
		rc.data[4 * index + 2] = color.elements[2];
		rc.data[4 * index + 3] = color.elements[3];
	}
}

vec3 barycentric(vec3 *points, ivec3 P)
{
    vec3 u = vec3_cross(v3(points[2].x - points[0].x, points[1].x - points[0].x, points[0].x - P.x),v3(points[2].y - points[0].y, points[1].y - points[0].y, points[0].y - P.y));
    if (fabs(u.z) < 1.f)
        return v3(-1,-1,-1);
    return v3(1.f - (u.x +u.y)/u.z, u.y/u.z, u.x/u.z);
}

internal vec3 project_point(vec3 coords, f32 *w)
{
    vec4 local_coords = v4(coords.x, coords.y, coords.z, 1.f);
    mat4 t = mat4_mul(mat4_translate(v3(0,0,0)), mat4_rotate(p.current_time *0, v3(0,1,0)));
    vec4 ndc_coords = mat4_mulv(mat4_mul(rc.proj, mat4_mul(get_view_mat(&rc.cam),t)), local_coords);
    //perspective divide??
    ndc_coords.x /= ndc_coords.w;
    ndc_coords.y /= ndc_coords.w;
    ndc_coords.z /= ndc_coords.w;
	*w = ndc_coords.w;
    ivec2 screen_coords = iv2(((ndc_coords.x + 1.f)/2) * rc.render_width, ((ndc_coords.y + 1.f)/2.f) * rc.render_height); 
    vec4 wc = mat4_mulv(t, local_coords);
	//ndc_coords.z = (ndc_coords.z+1)/2.f;
    return (vec3){screen_coords.x, screen_coords.y, ndc_coords.z};

}
void triangle_tex(Vertex *verts, Texture *t)
{ 
	f32 w[3]; //the w's from projection of the 3 points!
	verts[0].pos = project_point(verts[0].pos, &w[0]);
	verts[1].pos = project_point(verts[1].pos, &w[1]);
	verts[2].pos = project_point(verts[2].pos, &w[2]);
    ivec2 bboxmin = iv2(rc.render_width-1, rc.render_height-1);
    ivec2 bboxmax = iv2(0,0);
    ivec2 clamp = iv2(rc.render_width-1, rc.render_height-1);
    //we find the bounding box to test via barycentric coordinates
    for (i32 i = 0; i < 3; ++i)
    {
        for (i32 j = 0; j < 2;++j)
        {
            bboxmin.elements[j] = maximum(0, minimum(bboxmin.elements[j], verts[i].pos.elements[j]));
            bboxmax.elements[j] = minimum(clamp.elements[j], maximum(bboxmax.elements[j], verts[i].pos.elements[j]));
        }
    }
    ivec3 P;
    f32 depth;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; ++P.x)
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; ++P.y)
        {
            //we find if the point is in the trangle by testing barycentric coords
			vec3 points[3] = {verts[0].pos,verts[1].pos,verts[2].pos};
            vec3 bc_screen = barycentric(points, P);
            vec3 bc_clip = v3(bc_screen.x / w[0], bc_screen.y / w[1], bc_screen.z / w[2]);
            bc_clip = vec3_mulf(bc_clip, 1.f / (bc_screen.x / w[0]+bc_screen.y / w[1]+ bc_screen.z / w[2]));

            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
                continue;
            //if it is we paint!
            depth = 0;
			vec2 tex_coords = v2(0,0);
            //we find the real depth through interpolation!!
            for(i32 i = 0; i < 3; ++i)
			{
                depth += (f32)verts[i].pos.z*bc_screen.elements[i];
				tex_coords.x += verts[i].texcoord.x * (f32)bc_clip.elements[i];
				tex_coords.y += verts[i].texcoord.y * (f32)bc_clip.elements[i];
            }
			u32 index = P.x + P.y * rc.render_width;
            //if our pixel is closer than the zbuf's current, we render
			vec4 col = t->data[(int)(tex_coords.x * t->texture_width) + (int)(tex_coords.y * t->texture_height)*t->texture_width];
			//col = v4(random01(),0,random01(), 1);
			if (rc.zbuf[index] > depth)
            //if (TRUE)
            {
                rc.zbuf[index] = depth;
                rc.data[4 * index + 0] = col.x;
                rc.data[4 * index + 1] = col.y;
                rc.data[4 * index + 2] = col.z;
                rc.data[4 * index + 3] = col.w;
            }

        }
    }

}

internal void init(void)
{
	rc = (RenderingContext){0};
	rc.render_width = p.window_width;
	rc.render_height = p.window_height;
	rc.data = (f32*)malloc(sizeof(vec4) * rc.render_width * rc.render_height);
	rc.zbuf = (f32*)malloc(sizeof(f32) * rc.render_width * rc.render_height);
	rc.blend_func = FALSE; //no blending!
	rc.render_mode = TRIANGLE_MODE;
	rc.render_settings = ZBUF_ON;
    camera_init(&rc.cam);
    rc.proj = perspective_proj(45.f,rc.render_width/ (f32)rc.render_height, 0.1f,100.f); 
	tex = gen_sample_texture();
}


internal void update(f32 dt)
{
	//resize the wios
    if (p.window_width != rc.render_width || p.window_height != rc.render_height)
	{
		init(); //TODO change later :)
	}
	
	if (p.key_pressed[KEY_TAB])
	{
		if (rc.render_mode == TRIANGLE_MODE)rc.render_mode = LINE_MODE;
		else if (rc.render_mode == LINE_MODE)rc.render_mode = TRIANGLE_MODE;
	}

    camera_update(&rc.cam);
}

internal void render(void)
{
	rend_clear(&rc, v4(0.1,0.1,0.1,1));

    //here we transform all points from local coordinate -> global coords -> view coords -> NDC coords -> screen coords
    for (u32 i = 0; i < 36; i+=3)
    {
		/*
        vec3 points[3] = { (vec3){cube_data[i].pos.x, cube_data[i].pos.y,cube_data[i].pos.z},
                             (vec3){cube_data[i+1].pos.x, cube_data[i+1].pos.y,cube_data[i+1].pos.z},
                             (vec3){cube_data[i+2].pos.x, cube_data[i+2].pos.y,cube_data[i+2].pos.z}};

        points[0] = project_point(points[0]);
        points[1] = project_point(points[1]);
        points[2] = project_point(points[2]);
		*/
		Vertex verts[3] = {cube_data[i], cube_data[i+1], cube_data[i+2]};


        vec4 base_color = v4(0.7,0.3,0.3,1.f);
        f32 intensity = (i+10) / (f32)36;
        base_color = vec4_mulf(base_color, intensity);
		

		if (rc.render_mode == TRIANGLE_MODE)
		{
			//triangle(points, base_color);
			triangle_tex(verts, &tex);
		}else if (rc.render_mode == LINE_MODE)
		{
			
			rend_line(iv2(verts[0].pos.x,verts[0].pos.y), iv2(verts[1].pos.x,verts[1].pos.y), base_color);
			rend_line(iv2(verts[1].pos.x,verts[1].pos.y), iv2(verts[2].pos.x,verts[2].pos.y), base_color);
			rend_line(iv2(verts[2].pos.x,verts[2].pos.y), iv2(verts[0].pos.x,verts[0].pos.y), base_color);
		}
    }
}
#endif
