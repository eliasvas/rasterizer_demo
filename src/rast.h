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
#define TEXTURE_SAMPLERS_COUNT 8
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
	vec4 *data; //render_width * render_height * sizeof(f32) * 4
	f32 *zbuf; //render_width * render_height * sizeof(f32)
	b32 blend_func; //make the appropriate ENUM
    RenderMode render_mode;
	RenderSettings render_settings;
	//maybe add double buffering??? idk
	Camera cam;
	mat4 proj;
}RenderingContext;
global RenderingContext rc;
global Texture tex;

typedef struct Shader
{
	//here we put every variable that's ever needed
	vec4 ndc_coords;
	vec3 screen_coords;
	Texture *texture_samplers[TEXTURE_SAMPLERS_COUNT];
	vec4 (*vertex)(vec3 coords);
	vec4 (*fragment)(vec2 tex_coords, Texture *t);
}Shader;


global Shader s;


vec4 base_vertex(vec3 coords)
{
	vec4 local_coords = v4(coords.x, coords.y, coords.z, 1.f);
    mat4 t = mat4_mul(mat4_translate(v3(0,0,0)), mat4_rotate(p.current_time *0, v3(0,1,0)));
    vec4 ndc_coords = mat4_mulv(mat4_mul(rc.proj, mat4_mul(get_view_mat(&rc.cam),t)), local_coords);
    //perspective divide??
    ndc_coords.x /= ndc_coords.w;
    ndc_coords.y /= ndc_coords.w;
    ndc_coords.z /= ndc_coords.w;
    ivec2 screen_coords = iv2(((ndc_coords.x + 1.f)/2) * rc.render_width, ((ndc_coords.y + 1.f)/2.f) * rc.render_height); 
    vec4 wc = mat4_mulv(t, local_coords);
	//ndc_coords.z = (ndc_coords.z+1)/2.f;
    return (vec4){screen_coords.x, screen_coords.y, ndc_coords.z, ndc_coords.w};
}

vec4 base_fragment(vec2 tex_coords, Texture *t)
{
	vec4 col = t->data[(int)(tex_coords.x * t->texture_width) + (int)(tex_coords.y * t->texture_height)*t->texture_width];
	return col;
}

internal Shader shader_default(void)
{
	Shader shad;
	shad.vertex = base_vertex;
	shad.fragment = base_fragment;
	shad.texture_samplers[0] = &tex;
	return shad;
}

vec3 light_dir = {0.2f, 0.5f, -1.f};




internal void rend_clear(RenderingContext *rc, vec4 clear_color)
{
	for (u32 i = 0; i < rc->render_width * rc->render_height; ++i)
	{
		//clear fbo color info
		rc->data[i] = clear_color;
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
	//TODO make this bullshit readable darn it
	for (i32 x = t0.x; x <= t1.x; ++x)
	{
		u32 write = TRUE;
        u32 index;
        if (steep)
		{
            index = y + x * rc.render_width;
			if (x >= rc.render_height || y >= rc.render_width)write = FALSE;
        }
		else
		{
            index = x + y * rc.render_width;
			if (y >= rc.render_height || x >= rc.render_width)write = FALSE;
        }
		err += derror;
        if (err > dx)
        {
            y += (t1.y > t0.y ? 1 : -1);
            err -= dx*2;
        }
		if (write != TRUE)continue;
		rc.data[index] = color;
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
void triangle(Vertex *verts,Shader *s)
{ 
	vec4 v0 = s->vertex(verts[0].pos);
	vec4 v1 = s->vertex(verts[1].pos);
	vec4 v2 = s->vertex(verts[2].pos);
	verts[0].pos = (vec3){v0.x,v0.y,v0.z};
	verts[1].pos = v3(v1.x,v1.y,v1.z);
	verts[2].pos = v3(v2.x,v2.y,v2.z);
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
            vec3 bc_clip = v3(bc_screen.x / v0.w, bc_screen.y / v1.w, bc_screen.z / v2.w);
            bc_clip = vec3_mulf(bc_clip, 1.f / (bc_screen.x / v0.w+bc_screen.y / v1.w+ bc_screen.z /v2.w));

            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
                continue;
            //if it is we paint!
            depth = 0;
			vec2 tex_coords = {0,0};
            //we find the real depth through interpolation!!
            for(i32 i = 0; i < 3; ++i)
			{
                depth += (f32)verts[i].pos.z*bc_screen.elements[i];
				tex_coords.x += verts[i].texcoord.x * (f32)bc_clip.elements[i];
				tex_coords.y += verts[i].texcoord.y * (f32)bc_clip.elements[i];
            }
			u32 index = P.x + P.y * rc.render_width;
			vec4 col = s->fragment(tex_coords, s->texture_samplers[0]);
			if (rc.zbuf[index] > depth)
            {
                rc.zbuf[index] = depth;
                rc.data[index] = col;
            }

        }
    }

}

internal void init(void)
{
	rc = (RenderingContext){0};
	rc.render_width = p.window_width;
	rc.render_height = p.window_height;
	rc.data = (vec4*)malloc(sizeof(vec4) * rc.render_width * rc.render_height);
	rc.zbuf = (f32*)malloc(sizeof(f32) * rc.render_width * rc.render_height);
	rc.blend_func = FALSE; //no blending!
	rc.render_mode = TRIANGLE_MODE;
	rc.render_settings = ZBUF_ON;
    camera_init(&rc.cam);
    rc.proj = perspective_proj(45.f,rc.render_width/ (f32)rc.render_height, 0.1f,100.f); 
	tex = gen_sample_texture();
	s = shader_default();
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
		Vertex verts[3] = {cube_data[i], cube_data[i+1], cube_data[i+2]};
		
		if (rc.render_mode == TRIANGLE_MODE)
		{
			//triangle(points, base_color);
			vec3 normal = vec3_cross(vec3_sub(verts[2].pos, verts[0].pos), vec3_sub(verts[1].pos, verts[0].pos));
			normal = vec3_normalize(normal);
			f32 light_intensity = vec3_dot(normal, light_dir);
			if (light_intensity > 0.f)
				triangle(verts, &s);
		}else if (rc.render_mode == LINE_MODE)
		{
			vec4 base_color = v4(0.7,0.3,0.3,1.f);
			f32 fake_w = 0;
			verts[0].pos = project_point(verts[0].pos, &fake_w);
			verts[1].pos = project_point(verts[1].pos, &fake_w);
			verts[2].pos = project_point(verts[2].pos, &fake_w);
			
			rend_line(iv2(verts[0].pos.x,verts[0].pos.y), iv2(verts[1].pos.x,verts[1].pos.y), base_color);
			rend_line(iv2(verts[1].pos.x,verts[1].pos.y), iv2(verts[2].pos.x,verts[2].pos.y), base_color);
			rend_line(iv2(verts[2].pos.x,verts[2].pos.y), iv2(verts[0].pos.x,verts[0].pos.y), base_color);
		}
    }
	/*
	//we render the coordinate system!
	vec4 base_color = v4(0.7,0.3,0.3,1.f);
	f32 fake_w = 0;
	vec3 x_min = project_point(v3(-1000,0,0), &fake_w);
	vec3 x_max = project_point(v3(1000,0,0), &fake_w);
	vec3 y_min = project_point(v3(0,-1000,0), &fake_w);
	vec3 y_max = project_point(v3(0,1000,0), &fake_w);
	vec3 z_min = project_point(v3(0,0,-1000), &fake_w);
	vec3 z_max = project_point(v3(0,0,1000), &fake_w);	
	rend_line(iv2(x_min.x,x_min.y), iv2(x_max.x,x_max.y), base_color);
	rend_line(iv2(y_min.x,y_min.y), iv2(y_max.x,y_max.y), base_color);
	rend_line(iv2(z_min.x,z_min.y), iv2(z_max.x,z_max.y), base_color);
	*/
}
#endif
