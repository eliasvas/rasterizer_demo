#ifndef TEXTURE_H
#define TEXTURE_H

#include "tools.h"

typedef enum TextureFilter
{
	LINEAR_FILTER = 1,
	BILINEAR_FILTER,
	FILTERS_COUNT,
}TextureFilter;

typedef struct Texture
{
	vec4 *data;
	u32 texture_width, texture_height;
	TextureFilter tf;
}Texture;

internal Texture gen_sample_texture(void)
{
	Texture tex;
	tex.texture_width = 128;
	tex.texture_height = 128;
	i32 size_x = 16;
	i32 size_y = 16;
	tex.data = (vec4*)malloc(sizeof(vec4)*tex.texture_width*tex.texture_height);
	for (u32 y = 0;y < tex.texture_height; ++y)
	{
		for (u32 x = 0; x < tex.texture_width; ++x)
		{
			vec4 col = v4(1,1,1,1);
			col = vec4_mulf(col, (((x/size_x + y/size_y) % 2 == 0)+0.5)/2.f);
            tex.data[x + y * tex.texture_width] = col;
		}
	}
	tex.tf = LINEAR_FILTER;
	return tex;
}
#endif
