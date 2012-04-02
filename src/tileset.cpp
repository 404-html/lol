//
// Lol Engine
//
// Copyright: (c) 2010-2011 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://sam.zoy.org/projects/COPYING.WTFPL for more details.
//

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>

#if defined WIN32 && !defined _XBOX
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#include "core.h"
#include "lolgl.h"

using namespace std;

namespace lol
{

/*
 * TileSet implementation class
 */

class TileSetData
{
    friend class TileSet;

private:
    char *name, *path;
    int *tiles, ntiles;
    ivec2 size, isize, count;
    vec2 scale;
    float tx, ty;

    Image *img;
#if defined _XBOX
#   define STR0(x) #x
#   define STR(x) STR0(x)
#   pragma message(__FILE__ "(" STR(__LINE__) "): warning: TileSet not implemented")
#   undef STR
#   undef STR0
#else
    GLuint texture;
#endif
};

/*
 * Public TileSet class
 */

TileSet::TileSet(char const *path, ivec2 size, ivec2 count, vec2 scale)
  : data(new TileSetData())
{
    data->name = (char *)malloc(10 + strlen(path) + 1);
    data->path = data->name + 10;
    sprintf(data->name, "<tileset> %s", path);

    data->tiles = NULL;
#if defined _XBOX
#   define STR0(x) #x
#   define STR(x) STR0(x)
#   pragma message(__FILE__ "(" STR(__LINE__) "): warning: TileSet::TileSet() not implemented")
#   undef STR
#   undef STR0
#else
    data->texture = 0;
#endif
    data->img = new Image(path);
    data->isize = data->img->GetSize();

    if (count.x > 0 && count.y > 0)
    {
        data->count = count;
        data->size = data->isize / count;
    }
    else
    {
        if (size.x <= 0 || size.y <= 0)
            size = ivec2(32, 32);
        data->count.x = data->isize.x > size.x ? data->isize.x / size.x : 1;
        data->count.y = data->isize.y > size.y ? data->isize.y / size.y : 1;
        data->size = size;
    }

    data->tx = (float)data->size.x / PotUp(data->isize.x);
    data->ty = (float)data->size.y / PotUp(data->isize.y);

    data->scale = scale;
    data->ntiles = data->count.x * data->count.y;

    m_drawgroup = DRAWGROUP_BEFORE;
}

TileSet::~TileSet()
{
    free(data->tiles);
    free(data->name);
    delete data;
}

void TileSet::TickDraw(float deltams)
{
    Entity::TickDraw(deltams);

    if (IsDestroying())
    {
        if (data->img)
            delete data->img;
#if defined _XBOX
#   define STR0(x) #x
#   define STR(x) STR0(x)
#   pragma message(__FILE__ "(" STR(__LINE__) "): warning: TileSet::TickDraw() not implemented")
#   undef STR
#   undef STR0
#else
        else
            glDeleteTextures(1, &data->texture);
#endif
    }
    else if (data->img)
    {
#if defined _XBOX
#   define STR0(x) #x
#   define STR(x) STR0(x)
#   pragma message(__FILE__ "(" STR(__LINE__) "): warning: TileSet::TickDraw() not implemented")
#   undef STR
#   undef STR0
#else
        GLuint format;
        int planes;

        switch (data->img->GetFormat())
        {
        case Image::FORMAT_RGB:
           format = GL_RGB;
           planes = 3;
           break;
        case Image::FORMAT_RGBA:
        default:
           format = GL_RGBA;
           planes = 4;
           break;
        }

        int w = PotUp(data->isize.x);
        int h = PotUp(data->isize.y);

        uint8_t *pixels = (uint8_t *)data->img->GetData();
        if (w != data->isize.x || h != data->isize.y)
        {
            uint8_t *tmp = (uint8_t *)malloc(planes * w * h);
            for (int line = 0; line < data->isize.y; line++)
                memcpy(tmp + planes * w * line,
                       pixels + planes * data->isize.x * line,
                       planes * data->isize.x);
            pixels = tmp;
        }

        glGenTextures(1, &data->texture);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, data->texture);

        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0,
                     format, GL_UNSIGNED_BYTE, pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        if (pixels != data->img->GetData())
            free(pixels);
#endif
        delete data->img;
        data->img = NULL;
    }
}

char const *TileSet::GetName()
{
    return data->name;
}

ivec2 TileSet::GetCount() const
{
    return data->count;
}

ivec2 TileSet::GetSize(int tileid) const
{
    return data->size;
}

void TileSet::Bind()
{
#if defined _XBOX
#   define STR0(x) #x
#   define STR(x) STR0(x)
#   pragma message(__FILE__ "(" STR(__LINE__) "): warning: TileSet::Bind() not implemented")
#   undef STR
#   undef STR0
#else
    if (!data->img && data->texture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, data->texture);
    }
#endif
}

void TileSet::BlitTile(uint32_t id, vec3 pos, int o,
                       float *vertex, float *texture)
{
    float tx = data->tx * ((id & 0xffff) % data->count.x);
    float ty = data->ty * ((id & 0xffff) / data->count.x);
    vec2 scale = data->scale;

    int dx = data->size.x;
    int dy = o ? 0 : data->size.y;
    int dz = o ? data->size.y : 0;

#if defined _XBOX
#   define STR0(x) #x
#   define STR(x) STR0(x)
#   pragma message(__FILE__ "(" STR(__LINE__) "): warning: TileSet::TileSet() not implemented")
#   undef STR
#   undef STR0
#else
    if (!data->img && data->texture)
    {
        float tmp[10];

        *vertex++ = tmp[0] = scale.x * pos.x;
        *vertex++ = tmp[1] = scale.y * (pos.y + dy);
        *vertex++ = tmp[2] = scale.y * (pos.z + dz);
        *texture++ = tmp[3] = tx;
        *texture++ = tmp[4] = ty;

        *vertex++ = scale.x * (pos.x + dx);
        *vertex++ = scale.y * (pos.y + dy);
        *vertex++ = scale.y * (pos.z + dz);
        *texture++ = tx + data->tx;
        *texture++ = ty;

        *vertex++ = tmp[5] = scale.x * (pos.x + dx);
        *vertex++ = tmp[6] = scale.y * pos.y;
        *vertex++ = tmp[7] = scale.y * pos.z;
        *texture++ = tmp[8] = tx + data->tx;
        *texture++ = tmp[9] = ty + data->ty;

        *vertex++ = tmp[0];
        *vertex++ = tmp[1];
        *vertex++ = tmp[2];
        *texture++ = tmp[3];
        *texture++ = tmp[4];

        *vertex++ = tmp[5];
        *vertex++ = tmp[6];
        *vertex++ = tmp[7];
        *texture++ = tmp[8];
        *texture++ = tmp[9];

        *vertex++ = scale.x * pos.x;
        *vertex++ = scale.y * pos.y;
        *vertex++ = scale.y * pos.z;
        *texture++ = tx;
        *texture++ = ty + data->ty;
    }
    else
    {
        memset(vertex, 0, 3 * sizeof(float));
        memset(texture, 0, 2 * sizeof(float));
    }
#endif
}

} /* namespace lol */

