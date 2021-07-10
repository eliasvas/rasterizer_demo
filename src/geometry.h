#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "tools.h"

typedef struct Vertex
{
	vec3 pos;
	vec3 norm;
	vec2 texcoord;
}Vertex;

internal Vertex *sphere_data;
internal u32 sphere_verts_count;

internal Vertex quad_verts[6] = 
{
	{{-0.5f,-0.5f,0.f},{0.f,0.f,-1.f},{0.f,0.f}},
	{{0.5,-0.5,0},{0,0,-1},{1,0}},
	{{0.5,0.5,0},{0,0,-1},{1,1}},
	{{0.5,0.5,0},{0,0,-1},{1,1}},
	{{-0.5,0.5,0},{0,0,-1},{0,1}},
	{{-0.5,-0.5,0},{0,0,-1},{0,0}}
};
internal Vertex cube_data[] = {
        // positions          // normals           // texture coords
    {{-1.f, -1.f, -1.f},  {0.0f, 0.0f, -1.0f},  {0.0f,  0.0f}},
         {{1.f, -1.f, -1.f},  {0.0f,  0.0f, -1.0f},  {1.0f,  0.0f}},
         {{1.f,  1.f, -1.f},  {0.0f,  0.0f, -1.0f},  {1.0f,  1.0f}},
         {{1.f,  1.f, -1.f},  {0.0f,  0.0f, -1.0f},  {1.0f,  1.0f}},
        {{-1.f,  1.f, -1.f},  {0.0f,  0.0f, -1.0f},  {0.0f,  1.0f}},
        {{-1.f, -1.f, -1.f},  {0.0f,  0.0f, -1.0f},  {0.0f,  0.0f}},

        {{-1.f, -1.f,  1.f},  {0.0f,  0.0f,  1.0f},  {0.0f,  0.0f}},
         {{1.f, -1.f,  1.f},  {0.0f,  0.0f,  1.0f},  {1.0f,  0.0f}},
         {{1.f,  1.f,  1.f},  {0.0f,  0.0f,  1.0f},  {1.0f,  1.0f}},
         {{1.f,  1.f,  1.f},  {0.0f,  0.0f,  1.0f},  {1.0f,  1.0f}},
        {{-1.f,  1.f,  1.f},  {0.0f,  0.0f,  1.0f},  {0.0f,  1.0f}},
        {{-1.f, -1.f,  1.f},  {0.0f,  0.0f,  1.0f},  {0.0f,  0.0f}},

        {{-1.f,  1.f,  1.f}, {-1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}},
        {{-1.f,  1.f, -1.f}, {-1.0f,  0.0f,  0.0f},  {1.0f,  1.0f}},
        {{-1.f, -1.f, -1.f}, {-1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}},
        {{-1.f, -1.f, -1.f}, {-1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}},
        {{-1.f, -1.f,  1.f}, {-1.0f,  0.0f,  0.0f},  {0.0f,  0.0f}},
        {{-1.f,  1.f,  1.f}, {-1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}},

         {{1.f,  1.f,  1.f},  {1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}},
         {{1.f,  1.f, -1.f},  {1.0f,  0.0f,  0.0f},  {1.0f,  1.0f}},
         {{1.f, -1.f, -1.f},  {1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}},
         {{1.f, -1.f, -1.f},  {1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}},
         {{1.f, -1.f,  1.f},  {1.0f,  0.0f,  0.0f},  {0.0f,  0.0f}},
         {{1.f,  1.f,  1.f},  {1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}},

        {{-1.f, -1.f, -1.f},  {0.0f, -1.0f,  0.0f},  {0.0f,  1.0f}},
        { {1.f, -1.f, -1.f},  {0.0f, -1.0f,  0.0f},  {1.0f,  1.0f}},
       {  {1.f, -1.f,  1.f},  {0.0f, -1.0f,  0.0f},  {1.0f,  0.0f}},
       {  {1.f, -1.f,  1.f},  {0.0f, -1.0f,  0.0f},  {1.0f,  0.0f}},
       { {-1.f, -1.f,  1.f},  {0.0f, -1.0f,  0.0f},  {0.0f,  0.0f}},
       { {-1.f, -1.f, -1.f},  {0.0f, -1.0f,  0.0f},  {0.0f,  1.0f}},

     {   {-1.f,  1.f, -1.f},  {0.0f,  1.0f,  0.0f},  {0.0f,  1.0f}},
      {   {1.f,  1.f, -1.f},  {0.0f,  1.0f,  0.0f},  {1.0f,  1.0f}},
      {   {1.f,  1.f,  1.f},  {0.0f,  1.0f,  0.0f},  {1.0f,  0.0f}},
     {    {1.f,  1.f,  1.f},  {0.0f,  1.0f,  0.0f},  {1.0f,  0.0f}},
     {   {-1.f,  1.f,  1.f},  {0.0f,  1.0f,  0.0f},  {0.0f,  0.0f}},
     {   {-1.f,  1.f, -1.f},  {0.0f,  1.0f,  0.0f},  {0.0f,  1.0f}}
};


internal Vertex*
gen_sphere_data(f32 radius, u32 sector_count, u32 stack_count)
{
    //Vertex *res = malloc(sizeof(Vertex) * sectors * stacks * 3);
    u32 vertices_count = sector_count * stack_count * 3 * 2;
    Vertex *res = (Vertex*)malloc(sizeof(Vertex) * vertices_count);
    u32 vertex_count = 0;

    vec3 pos;
    vec3 norm;
    vec2 texcoords;
    f32 length_inv = 1.f/radius;

    vec3 *positions = (vec3*)malloc(sizeof(vec3) * sector_count * stack_count*10);
    u32 positions_count = 0;
    vec3 *normals = (vec3*)malloc(sizeof(vec3) * sector_count * stack_count*10);
    u32 normals_count = 0;
    vec2 *tex_coords= (vec2*)malloc(sizeof(vec2) * sector_count * stack_count*10);
    u32 tex_count = 0;


    f32 sector_step = 2 * PI / sector_count;
    f32 stack_step = PI / stack_count;
    f32 sector_angle, stack_angle;
    f32 xy, z;
    vec3 position;
    vec3 normal;
    vec2 tex_coord;

    for (u32 i = 0; i <= stack_count; ++i)
    {
        stack_angle = PI / 2.f -i*stack_step;
        xy = radius * cos(stack_angle);
        z = radius * sin(stack_angle);

        for (u32 j = 0; j <= sector_count; ++j)
        {
            sector_angle = j * sector_step;

            position.x = xy * cos(sector_angle);
            position.y = xy * sin(sector_angle);
            position.z = z;
            positions[positions_count++] = position;

            normal.x = position.x * length_inv;
            normal.y = position.y * length_inv;
            normal.z = position.z * length_inv;
            normals[normals_count++] = normal; 

            tex_coord.y = (f32)j / (f32)sector_count;
            tex_coord.x = (f32)i / (f32)stack_count;
            tex_coords[tex_count++] = tex_coord;
        }
    }


    // indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    for (u32 i = 0; i < stack_count; ++i)
    {
        u32 k1 = i * (sector_count+1);
        u32 k2 = k1 + (sector_count+1);

        for (u32 j = 0; j < sector_count; ++j, ++k1,++k2)
        {
            //for triangle k1 - k2 - k1+1
            if(i != 0)
            {
                //addIndices(k1, k2, k1+1);   // k1---k2---k1+1
                res[vertex_count++] = (Vertex){positions[k1],normals[k1],tex_coords[k1]};
                res[vertex_count++] = (Vertex){positions[k2],normals[k2],tex_coords[k2]};
                res[vertex_count++] = (Vertex){positions[k1+1],normals[k1+1],tex_coords[k1+1]};
            }

            //for triangle k1+1 - k2 - k2+1
            if(i != (stack_count-1))
            {
                //addIndices(k1+1, k2, k2+1); // k1+1---k2---k2+1
                res[vertex_count++] = (Vertex){positions[k1+1],normals[k1+1],tex_coords[k1+1]};
                res[vertex_count++] = (Vertex){positions[k2],normals[k2],tex_coords[k2]};
                res[vertex_count++] = (Vertex){positions[k2+1],normals[k2+1],tex_coords[k2+1]};
            }
        }
    }
    //sprintf(error_log, "%i vertices", vertex_count);
    sphere_verts_count = vertex_count;

    return res;
}

#endif
