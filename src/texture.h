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
	tex.texture_width = 256;
	tex.texture_height = 256;
	tex.data = (vec4*)malloc(sizeof(vec4)*tex.texture_width*tex.texture_height);
	for (u32 y = 0;y < tex.texture_height; ++y)
	{
		for (u32 x = 0; x < tex.texture_width; ++x)
		{
			tex.data[x + y * tex.texture_width] = v4((x%16)/16.f,0,0,1);
		}
	}
	tex.tf = LINEAR_FILTER;
	return tex;
}
#endif
