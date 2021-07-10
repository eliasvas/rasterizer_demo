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

vec3 light_dir = {0.2f, 0.5f, -1.f};
mat4 proj;
mat4 view;
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
		rc->zbuf[i] = 1000.f;
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

vec3 barycentric(vec3 *points, ivec3 P)
{
    vec3 u = vec3_cross(v3(points[2].x - points[0].x, points[1].x - points[0].x, points[0].x - P.x),v3(points[2].y - points[0].y, points[1].y - points[0].y, points[0].y - P.y));
    if (fabs(u.z) < 1.f)
        return v3(-1,-1,-1);
    return v3(1.f - (u.x +u.y)/u.z, u.y/u.z, u.x/u.z);
}

void triangle(vec3 *points, vec4 color)
{ 
    ivec2 bboxmin = iv2(rc.render_width-1, rc.render_height-1);
    ivec2 bboxmax = iv2(0,0);
    ivec2 clamp = iv2(rc.render_width-1, rc.render_height-1);
    //we find the bounding box to test via barycentric coordinates
    for (i32 i = 0; i < 3; ++i)
    {
        for (i32 j = 0; j < 2;++j)
        {
            bboxmin.elements[j] = maximum(0, minimum(bboxmin.elements[j], points[i].elements[j]));
            bboxmax.elements[j] = minimum(clamp.elements[j], maximum(bboxmax.elements[j], points[i].elements[j]));
        }
    }
    ivec3 P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; ++P.x)
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; ++P.y)
        {
            //we find if the point is in the trangle by testing barycentric coords
            vec3 bc_screen = barycentric(points, P);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
                continue;
            //if it is we paint!
            P.z = 0;
            //we find the real depth through interpolation!!
            for(i32 i = 0; i < 3; ++i)
                P.z += points[i].elements[2]*bc_screen.elements[i];
            u32 index = P.x + P.y * rc.render_width;
            //if our pixel is closer than the zbuf's current, we render
            if (rc.zbuf[index] > P.z)
            //if (TRUE)
            {
                rc.zbuf[index] = P.z;
                rc.data[4 * index + 0] = color.elements[0];
                rc.data[4 * index + 1] = color.elements[1];
                rc.data[4 * index + 2] = color.elements[2];
                rc.data[4 * index + 3] = color.elements[3];
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
	rc.cull_faces = FALSE;
	rc.blend_func = FALSE; //no blending!

    //look_at(vec3 eye, vec3 center, vec3 fake_up)
    view = look_at(v3(0,0,-10), v3(0,0,0), v3(0,1,0));
    proj = perspective_proj(45.f,rc.render_width/ (f32)rc.render_height, 0.1f,100.f); 

    /*
    sphere_data = gen_sphere_data(1.f, 8, 8);
    for (u32 i = 0; i < sphere_verts_count; ++i)
    {
        vec4 local_coords = v4(sphere_data[i].pos.x, sphere_data[i].pos.y, sphere_data[i].pos.z, 1.f);
        mat4 t = mat4_translate(v3(0,0,0));
        t.elements[0][0] = 1.f;
        t.elements[1][1] = 1.f;
        t.elements[2][2] = 1.f;
        vec4 ndc_coords = mat4_mulv(mat4_mul(proj, mat4_mul(view,t)), local_coords);
        //perspective divide??
        ndc_coords.x /= ndc_coords.z;
        ndc_coords.y /= ndc_coords.z;
        ivec2 screen_coords = iv2(((ndc_coords.x + 1.f)/2) * rc.render_width, ((ndc_coords.y + 1.f)/2.f) * rc.render_height); 
        sphere_data[i].pos = (vec3){screen_coords.x, screen_coords.y, ndc_coords.z};
    }
    */

   /* 
    for (u32 i = 0; i < 36; ++i)
    {
        vec4 local_coords = v4(cube_data[i].pos.x, cube_data[i].pos.y, cube_data[i].pos.z, 1.f);
        mat4 t = mat4_mul(mat4_translate(v3(0,0,0)), mat4_rotate(28.f, v3(0,1,0.2)));
        t.elements[0][0] = 1.f;
        t.elements[1][1] = 1.f;
        t.elements[2][2] = 1.f;
        vec4 ndc_coords = mat4_mulv(mat4_mul(proj, mat4_mul(view,t)), local_coords);
        //perspective divide??
        ndc_coords.x /= ndc_coords.z;
        ndc_coords.y /= ndc_coords.z;
        ivec2 screen_coords = iv2(((ndc_coords.x + 1.f)/2) * rc.render_width, ((ndc_coords.y + 1.f)/2.f) * rc.render_height); 
        cube_data[i].pos = (vec3){screen_coords.x, screen_coords.y, ndc_coords.z};
    }
    */

}
internal vec3 project_point(vec3 coords)
{
    vec4 local_coords = v4(coords.x, coords.y, coords.z, 1.f);
    mat4 t = mat4_mul(mat4_translate(v3(0,0,0)), mat4_rotate(p.current_time * 40, v3(0,1,0)));
    t.elements[0][0] = 1.f;
    t.elements[1][1] = 1.f;
    t.elements[2][2] = 1.f;
    vec4 ndc_coords = mat4_mulv(mat4_mul(proj, mat4_mul(view,t)), local_coords);
    //perspective divide??
    ndc_coords.x /= ndc_coords.w;
    ndc_coords.y /= ndc_coords.w;
    ndc_coords.z /= ndc_coords.w;
    ivec2 screen_coords = iv2(((ndc_coords.x + 1.f)/2) * rc.render_width, ((ndc_coords.y + 1.f)/2.f) * rc.render_height); 
    vec4 wc = mat4_mulv(t, local_coords);
    return (vec3){screen_coords.x, screen_coords.y, ndc_coords.z};

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
	rend_clear(&rc, v4(0.05,0.05,0.05,1));

    ivec2 origin = (ivec2){rc.render_width / 2, rc.render_height / 2};
    ivec2 dest = (ivec2){origin.x + cos(p.current_time)*rc.render_width/4.f, origin.y + sin(p.current_time)*rc.render_width /4.f};
    rend_line(&rc, origin, dest, v4(1,1,1,1));

    /*
    for (u32 i = 0; i < 6; i+=3)
    {
        ivec2 v0 = (ivec2){(quad_verts[i].pos.x + 1.f) * 100, (quad_verts[i].pos.y + 1.f) * 100};
        ivec2 v1 = (ivec2){(quad_verts[i+1].pos.x + 1.f) * 100, (quad_verts[i+1].pos.y + 1.f) * 100};
        ivec2 v2 = (ivec2){(quad_verts[i+2].pos.x + 1.f) * 100, (quad_verts[i+2].pos.y + 1.f) * 100};
        rend_line(&rc, v0, v1, v4(1,0,0,1));
        rend_line(&rc, v1, v2, v4(0,1,0,1));
        rend_line(&rc, v2,- v0, v4(0,0,1,1));
    }
    */

    //here we transform all points from local coordinate -> global coords -> view coords -> NDC coords -> screen coords
    for (u32 i = 0; i < 36; i+=3)
    {
        vec3 points[3] = { (vec3){cube_data[i].pos.x, cube_data[i].pos.y,cube_data[i].pos.z},
                             (vec3){cube_data[i+1].pos.x, cube_data[i+1].pos.y,cube_data[i+1].pos.z},
                             (vec3){cube_data[i+2].pos.x, cube_data[i+2].pos.y,cube_data[i+2].pos.z}};

        points[0] = project_point(points[0]);
        points[1] = project_point(points[1]);
        points[2] = project_point(points[2]);




        vec4 base_color = v4(0.7,0.3,0.3,1.f);
        f32 intensity = vec3_dot(vec3_normalize(cube_data[i].norm), vec3_normalize(light_dir));
        intensity = (i+2) / (f32)36;
        base_color = vec4_mulf(base_color, intensity);
#if defined(EPILEPSY)
        if (intensity > 0)
            triangle(points, v4(random01(), random01(), random01(),1));
#else
        if (intensity > 0)
            triangle(points, base_color);
#endif
    }
}
#endif
