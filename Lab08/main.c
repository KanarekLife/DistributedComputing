#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <png.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define KERNEL_RADIUS 15
double kernel[2 * KERNEL_RADIUS + 1][2 * KERNEL_RADIUS + 1];

int img_width, img_height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers = NULL;
png_bytep *dst_row_pointers = NULL;

void allocate_pixels(int width, int height, png_bytep **row_pointers)
{
    if (*row_pointers != NULL)
        abort();

    png_byte *pixels = (png_byte *)malloc(sizeof(png_byte) * height * width * 4);
    if (pixels == NULL)
        abort();
    *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
    if (*row_pointers == NULL)
        abort();
    for (int y = 0; y < height; y++)
    {
        (*row_pointers)[y] = pixels + y * width * 4;
    }
}

void free_pixels(int width, int height, png_bytep **row_pointers)
{
    if (*row_pointers == NULL)
        abort();
    free(*row_pointers[0]);
    free(*row_pointers);
}

void read_png_file(char *filename)
{
    FILE *fp = fopen(filename, "rb");

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
        abort();

    png_infop info = png_create_info_struct(png);
    if (!info)
        abort();

    if (setjmp(png_jmpbuf(png)))
        abort();

    png_init_io(png, fp);
    png_read_info(png, info);

    img_width = png_get_image_width(png, info);
    img_height = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    allocate_pixels(img_width, img_height, &row_pointers);
    allocate_pixels(img_width, img_height, &dst_row_pointers);

    png_read_image(png, row_pointers);

    fclose(fp);

    png_destroy_read_struct(&png, &info, NULL);
}

void write_png_file(char *filename)
{
    int y;

    FILE *fp = fopen(filename, "wb");
    if (!fp)
        abort();

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
        abort();

    png_infop info = png_create_info_struct(png);
    if (!info)
        abort();

    if (setjmp(png_jmpbuf(png)))
        abort();

    png_init_io(png, fp);

    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
        png,
        info,
        img_width, img_height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    // png_set_filler(png, 0, PNG_FILLER_AFTER);

    if (dst_row_pointers == NULL)
        abort();

    png_write_image(png, dst_row_pointers);
    png_write_end(png, NULL);

    free_pixels(img_width, img_height, &row_pointers);
    free_pixels(img_width, img_height, &dst_row_pointers);

    fclose(fp);

    png_destroy_write_struct(&png, &info);
}

void populate_kernel()
{
    const double sigma = fmax(KERNEL_RADIUS / 2.0f, 1.0f);

    for (int i = -KERNEL_RADIUS; i <= KERNEL_RADIUS; ++i)
    {
        for (int j = -KERNEL_RADIUS; j <= KERNEL_RADIUS; ++j)
        {
            const double exponentNumerator = -(i * i + j * j);
            const double exponentDenominator = 2 * sigma * sigma;
            const double eExpression = exp(exponentNumerator / exponentDenominator);
            const double kernelValue = (eExpression / (2 * M_PI * sigma * sigma));
            kernel[j + KERNEL_RADIUS][i + KERNEL_RADIUS] = kernelValue;
        }
    }
}

int get_px_val(int x, int y, int color)
{
    if (x < 0 || x >= img_width)
        return -1;
    if (y < 0 || y >= img_height)
        return -1;
    png_bytep px = &(row_pointers[y][4 * x]);
    return px[color];
}

void gaussian_blur(int x, int y, png_bytep dst_px)
{
    for (int color = 0; color < 4; ++color)
    {
        double val = 0;
        double div_val = 0;
        for (int i = -KERNEL_RADIUS; i <= KERNEL_RADIUS; ++i)
        {
            for (int j = -KERNEL_RADIUS; j <= KERNEL_RADIUS; ++j)
            {
                int px_val = get_px_val(x + i, y + j, color);
                if (px_val == -1)
                    continue;
                val += (double)px_val * kernel[j + KERNEL_RADIUS][i + KERNEL_RADIUS];
                div_val += kernel[j + KERNEL_RADIUS][i + KERNEL_RADIUS];
            }
        }
        val /= div_val;
        dst_px[color] = val;
    }
}

void process_png_file(int sx, int sy, int width, int height)
{
    for (int y = sy; y < height; y++)
    {
        png_bytep dst_row = dst_row_pointers[y];
        for (int x = sx; x < width; x++)
        {
            png_bytep dst_px = &(dst_row[x * 4]);
            gaussian_blur(x, y, dst_px);
        }
    }
}

struct worker_args
{
    int sx, sy, width, height;
};

void *worker_job(void *args)
{
    struct worker_args *wa = (struct worker_args *)args;
    process_png_file(wa->sx, wa->sy, wa->width, wa->height);
    return NULL;
}

void dispatch_processes(int thread_count)
{
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
    if (threads == NULL)
        abort();
    struct worker_args *wa = (struct worker_args *)malloc(sizeof(struct worker_args) * thread_count);
    if (wa == NULL)
        abort();

    for (int i = 0; i < thread_count; i++)
    {
        wa[i].sx = 0;
        wa[i].sy = i * img_height / thread_count;
        wa[i].width = img_width;
        wa[i].height = fmin((i + 1) * img_height / thread_count, img_height);
        pthread_create(&threads[i], NULL, worker_job, (void *)&wa[i]);
    }
    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(wa);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: ./a.out <src> <dst>\n");
        return 0;
    }

    populate_kernel();
    read_png_file(argv[1]);

    dispatch_processes(14);

    write_png_file(argv[2]);

    return 0;
}
