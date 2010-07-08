
#include <stdlib.h>

#include "layer.h"

Layer::Layer(int w, int h, int z, uint32_t *in_data)
{
    width = w;
    height = h;
    altitude = z;
    data = in_data;

#if 0
    fread(data, sizeof(unsigned int), width * height, fp);
    for (int n = 0; n < width * height; n++)
    {
        unsigned int i = data[n];
        // XXX: endianness swapping might be necessary here
        data[n] = i ? i - 1 : 0;
    }
#endif
}

Layer::~Layer()
{
    free(data);
}

void Layer::Render(Scene *scene, int x, int y, int z)
{
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
            if (data[j * width + i])
                scene->AddTile(data[j * width + i],
                               i * 32 - x, j * 32 - y, altitude + z);
}

int Layer::GetZ()
{
    return altitude;
}

