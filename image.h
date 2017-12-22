#pragma once

//#ifndef IMAGE_H
//#define IMAGE_H

#include <png.h>
#include <iostream>

#define COLOR(a) (a>0?(a<255?a:255):0)

struct RGB {
    RGB(double r=0, double g=0, double b=0) : r(COLOR(r)), g(COLOR(g)), b(COLOR(b)) {
    };
    
    double r, g, b;
    bool operator==(const RGB& rhs) const {
        return r == rhs.r && g == rhs.g && b == rhs.b;
    }
};

std::ostream& operator<<(std::ostream& out, const RGB& x) {
    out << x.r << " " << x.g << " " << x.b;
    return out;
}

class Image {
public:
    explicit Image(const std::string& filename) {
        FILE *fp = fopen(filename.c_str(), "rb");
        
        auto png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if(!png) abort();
        
        png_infop info = png_create_info_struct(png);
        if(!info) abort();
        
        if(setjmp(png_jmpbuf(png))) abort();
        
        png_init_io(png, fp);
        
        png_read_info(png, info);
        
        width_      = png_get_image_width(png, info);
        height_     = png_get_image_height(png, info);
        auto color_type = png_get_color_type(png, info);
        auto bit_depth  = png_get_bit_depth(png, info);
        
        if(bit_depth == 16)
            png_set_strip_16(png);
        
        if(color_type == PNG_COLOR_TYPE_PALETTE)
            png_set_palette_to_rgb(png);
        
        if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
            png_set_expand_gray_1_2_4_to_8(png);
        
        if(png_get_valid(png, info, PNG_INFO_tRNS))
            png_set_tRNS_to_alpha(png);
        
        if(color_type == PNG_COLOR_TYPE_RGB ||
           color_type == PNG_COLOR_TYPE_GRAY ||
           color_type == PNG_COLOR_TYPE_PALETTE)
            png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
        
        if(color_type == PNG_COLOR_TYPE_GRAY ||
           color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
            png_set_gray_to_rgb(png);
        
        png_read_update_info(png, info);
        
        bytes_ = (png_bytep*)malloc(sizeof(png_bytep) * height_);
        for(int y = 0; y < height_; y++) {
            bytes_[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
        }
        
        png_read_image(png, bytes_);
        
        fclose(fp);
        
        png_destroy_read_struct(&png, &info, NULL);
    }

    void Write(const std::string& filename) {
        FILE *fp = fopen(filename.c_str(), "wb");
        if(!fp) abort();

        png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png) abort();
        
        png_infop info = png_create_info_struct(png);
        if (!info) abort();
        
        if (setjmp(png_jmpbuf(png))) abort();
        
        png_init_io(png, fp);
        
        // Output is 8bit depth, RGBA format.
        png_set_IHDR(
                     png,
                     info,
                     width_, height_,
                     8,
                     PNG_COLOR_TYPE_RGBA,
                     PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_DEFAULT,
                     PNG_FILTER_TYPE_DEFAULT
                     );
        png_write_info(png, info);
        
        png_write_image(png, bytes_);
        png_write_end(png, NULL);
        
        fclose(fp);
        
        if (png && info)
            png_destroy_write_struct(&png, &info);
    }
    
    ~Image() {
        for(int y = 0; y < height_; y++) {
            free(bytes_[y]);
        }
        free(bytes_);
    }

    RGB GetPixel(int y, int x) const {
        if (y >= 0 && x >= 0 && y < height_ && x < width_) {
            png_bytep px = &(bytes_[y][x * 4]);
            return RGB(px[0], px[1], px[2]);
        } else {
            return RGB();
        }
    }

    void SetPixel(const RGB& pixel, int y, int x) {
        if (y >= 0 && x >= 0 && y < height_ && x < width_) {
            png_bytep px = &(bytes_[y][x * 4]);
            px[0] = pixel.r;
            px[1] = pixel.g;
            px[2] = pixel.b;
        }
    }

    int Height() const {
        return height_;
    }

    int Width() const {
        return width_;
    }

private:
    int width_, height_;
    
    png_bytep* bytes_;
};

//#endif

