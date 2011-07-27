// Framework
#include "maplib.h"

#include <png.h>

#include "pngsave.h"


static inline void PNGWriteCleanup(png_infop *info_ptr, png_structp *png_ptr, FILE* fileHandle)
{
    if (*info_ptr != NULL)
        png_destroy_info_struct(*png_ptr, info_ptr);
    if (*png_ptr != NULL)
        png_destroy_write_struct(png_ptr, NULL);
    if (fileHandle != NULL)
        fclose(fileHandle);
}

static bool savePngInternal(const char *fileName, unsigned char *pixels, int w, int h, int bitdepth, int color_type)
{
    unsigned char** scanlines = NULL;
    png_infop info_ptr = NULL;
    png_structp png_ptr = NULL;
    FILE *fp;
    
    if (fileName == NULL || *fileName == '\0' || pixels == NULL)
    {
        return false;
    }
    if (!(fp = fopen(fileName, "wb")))
    {
        debug(LOG_ERROR, "%s won't open for writing!", fileName);
        return false;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        debug(LOG_ERROR, "savePng: Unable to create png struct\n");
        PNGWriteCleanup(&info_ptr, &png_ptr, fp);
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        debug(LOG_ERROR, "savePng: Unable to create png info struct\n");
        PNGWriteCleanup(&info_ptr, &png_ptr, fp);
        return false;
    }

    // If libpng encounters an error, it will jump into this if-branch    
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        debug(LOG_ERROR, "savePng: Error encoding PNG data\n");
        PNGWriteCleanup(&info_ptr, &png_ptr, fp);
        return false;        
    }
    else
    {
        unsigned int channelsPerPixel = 3;
        unsigned int currentRow, row_stride;

        if (color_type == PNG_COLOR_TYPE_GRAY)
        {
            channelsPerPixel = 1;
        }
        row_stride = w * channelsPerPixel * bitdepth / 8;

        scanlines = (unsigned char **)malloc(sizeof(unsigned char *) * h);
        if (scanlines == NULL)
        {
            debug(LOG_ERROR, "pie_PNGSaveFile: Couldn't allocate memory\n");
            PNGWriteCleanup(&info_ptr, &png_ptr, fp);
            return false;
        }

        png_init_io(png_ptr, fp);
        
        //png_set_write_fn(png_ptr, fp, wzpng_write_data, wzpng_flush_data);

        // Set the compression level of ZLIB
        // Right now we stick with the default, since that one is the
        // fastest which still produces acceptable filesizes.
        // The highest compression level hardly produces smaller files than default.
        //
        // Below are some benchmarks done while taking screenshots at 1280x1024
        // Z_NO_COMPRESSION:
        // black (except for GUI): 398 msec
        // 381, 391, 404, 360 msec
        //
        // Z_BEST_SPEED:
        // black (except for GUI): 325 msec
        // 611, 406, 461, 608 msec
        //
        // Z_DEFAULT_COMPRESSION:
        // black (except for GUI): 374 msec
        // 1154, 1121, 627, 790 msec
        //
        // Z_BEST_COMPRESSION:
        // black (except for GUI): 439 msec
        // 1600, 1078, 1613, 1700 msec

        // Not calling this function is equal to using the default
        // so to spare some CPU cycles we comment this out.
        // png_set_compression_level(png_ptr, Z_DEFAULT_COMPRESSION);
        png_set_IHDR(png_ptr, info_ptr, w, h, bitdepth,
                     color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
        png_write_info(png_ptr, info_ptr);

        // Create an array of scanlines
        for (currentRow = 0; currentRow < h; ++currentRow)
        {
            // We're filling the scanline from the bottom up here,
            // otherwise we'd have a vertically mirrored image.
            scanlines[currentRow] = pixels + row_stride * (h - currentRow - 1);
        }

        png_write_image(png_ptr, (png_bytepp)scanlines);

        png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
        png_write_end(png_ptr, NULL);
    }

    free(scanlines);
    PNGWriteCleanup(&info_ptr, &png_ptr, fp);

    return true;
}

/**************************************************************************
  Save an RGBA8888 image buffer to a PNG file.
**************************************************************************/
bool savePng(const char *filename, char *pixels, int w, int h)
{
	return savePngInternal(filename, (unsigned char *)pixels, w, h, 8, PNG_COLOR_TYPE_RGBA);
}

/**************************************************************************
  Save an I16 image buffer to a PNG file.
**************************************************************************/
bool savePngI16(const char *filename, uint16_t *pixels, int w, int h)
{
	return savePngInternal(filename, (unsigned char *)pixels, w, h, 16, PNG_COLOR_TYPE_GRAY);
}

/**************************************************************************
  Save an I8 image buffer to a PNG file.
**************************************************************************/
bool savePngI8(const char *filename, uint8_t *pixels, int w, int h)
{
	return savePngInternal(filename, (unsigned char *)pixels, w, h, 8, PNG_COLOR_TYPE_GRAY);
}
