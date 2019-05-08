#include <iostream>
#include <fstream>
#include "geometry2d/Point.h"
#include "geometry2d/Rectangle.h"
#include "flame/FlameTransformation.h"
#include "flame/Flame.h"
#include "color/InterpolatedPalette.h"

using namespace std;

int main() {

    const char *FILENAME = "output/flame.bmp";
    dim2 dims; // image dimensions
    dims.width = 1920;
    dims.height = 1200;
    const Point CENTER_POINT(Point(-0.4, 0.0));
    const Rectangle RECTANGLE(CENTER_POINT, 5.0, 4.0); // flame dimensions

    // BMP format stuff
    typedef struct                       /**** BMP file header structure ****/
    {
        unsigned int bfSize;           /* Size of file */
        unsigned short bfReserved1;      /* Reserved */
        unsigned short bfReserved2;      /* ... */
        unsigned int bfOffBits;        /* Offset to bitmap data */
    } BITMAPFILEHEADER;

    typedef struct                       /**** BMP file info structure ****/
    {
        unsigned int biSize;           /* Size of info header */
        int biWidth;          /* Width of image */
        int biHeight;         /* Height of image */
        unsigned short biPlanes;         /* Number of color planes */
        unsigned short biBitCount;       /* Number of bits per pixel */
        unsigned int biCompression;    /* Type of compression to use */
        unsigned int biSizeImage;      /* Size of image data */
        int biXPelsPerMeter;  /* X pixels per meter */
        int biYPelsPerMeter;  /* Y pixels per meter */
        unsigned int biClrUsed;        /* Number of colors used */
        unsigned int biClrImportant;   /* Number of important colors */
    } BITMAPINFOHEADER;

    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;

    unsigned int padding_bytes = (4 - ((dims.width * 3) % 4)) % 4;

    // Magic number for file. It does not fit in the header structure due to alignment requirements, so put it outside
    char padding[] = {0, 0, 0, 0};
    unsigned short bfType = 0x4d42;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfSize =
            2 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (dims.width * 3 + padding_bytes) * dims.height;
    bfh.bfOffBits = 0x36;

    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = dims.width;
    bih.biHeight = dims.height;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;
    bih.biSizeImage = (dims.width * 3 + padding_bytes) * dims.height;
    bih.biXPelsPerMeter = 5000;
    bih.biYPelsPerMeter = 5000;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;

    // fractal parameters
    vector<FlameTransformation> list;

    double p1[6];
    double p2[6];
    double p3[6];

    p1[0] = 1;
    p1[1] = 0.1;
    p1[2] = 0;
    p1[3] = 0;
    p1[4] = 0;
    p1[5] = 0;

    p2[0] = 0;
    p2[1] = 0;
    p2[2] = 0;
    p2[3] = 0;
    p2[4] = 0.8;
    p2[5] = 1;

    p3[0] = 1;
    p3[1] = 0;
    p3[2] = 0;
    p3[3] = 0;
    p3[4] = 0;
    p3[5] = 0;

    list.emplace_back(FlameTransformation(AffineTransformation(-0.4113504,
                                                               -0.7124804, -0.4, 0.7124795, -0.4113508, 0.8), p1));
    list.emplace_back(FlameTransformation(AffineTransformation(-0.3957339,
                                                               0, -1.6, 0, -0.3957337, 0.2), p2));
    list.emplace_back(FlameTransformation(AffineTransformation(0.4810169, 0,
                                                               1, 0, 0.4810169, 0.9), p3));

    Flame flame(list);

    FlameAccumulator fractal = flame.compute(RECTANGLE, dims, 50);

    vector<Color> colorList;
    colorList.emplace_back(Color::RED);
    colorList.emplace_back(Color::GREEN);
    colorList.emplace_back(Color::BLUE);

    Palette palette = InterpolatedPalette(colorList);
    Color background(0, 0, 0);

    // print to file
    FILE *f;
    f = fopen(FILENAME, "wb");
    fwrite(&bfType, 1, sizeof(bfType), f);
    fwrite(&bfh, 1, sizeof(bfh), f);
    fwrite(&bih, 1, sizeof(bih), f);

    for (unsigned int i = 0; i < dims.height; i++) {
        for (unsigned int j = 0; j < dims.width; j++) {
            Color color = fractal.color(palette, background, j, i);

            unsigned char r = Color::srgb_encode(color.get_r(), 255);
            unsigned char g = Color::srgb_encode(color.get_g(), 255);
            unsigned char b = Color::srgb_encode(color.get_b(), 255);

            fwrite(&b, 1, 1, f);
            fwrite(&g, 1, 1, f);
            fwrite(&r, 1, 1, f);
        }

        fwrite(padding, 1, padding_bytes, f);
    }

    fclose(f);
}