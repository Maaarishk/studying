#pragma once

//#ifndef DITHER_H
//#define DITHER_H

#include <string>
#include <vector>

#include "image.h"
#include "kd_tree.h"

void Dither(const std::string& input_file, const std::string& output_file,
            const std::vector<RGB>& ok_pixels) {
    std::vector<std::vector<double>> pixels;
    pixels.resize(ok_pixels.size());
    int c = 0;
    
    for (const auto& pixel: ok_pixels) {
        pixels[c].push_back(((double)(pixel.r))/255.0);
        pixels[c].push_back(((double)(pixel.g))/255.0);
        pixels[c].push_back(((double)(pixel.b))/255.0);
        c++;
    }
    KdTree kd(pixels);
    
    Image img(input_file);
    
    for (int y = 0; y < img.Height(); ++y) {
        for (int x = 0; x < img.Width(); ++x) {
            auto oldpixel = img.GetPixel(y, x);
            std::vector<double> pix;
            
            pix.push_back(((double)(oldpixel.r))/255.0);
            pix.push_back(((double)(oldpixel.g))/255.0);
            pix.push_back(((double)(oldpixel.b))/255.0);
            
            auto newpixel  = ok_pixels[kd.GetNearest(pix)];
            img.SetPixel(newpixel, y, x);
            
            double r = oldpixel.r - newpixel.r;
            double g = oldpixel.g - newpixel.g;
            double b = oldpixel.b - newpixel.b;
            
            auto p = img.GetPixel(y, x + 1);
            img.SetPixel( RGB(p.r + COLOR(r * 7.0) / 16.0, p.g + COLOR(g * 7.0) / 16.0, p.b + COLOR(b * 7.0) / 16.0), y, x+1);
            
            p = img.GetPixel(y+1, x - 1);
            img.SetPixel( RGB(p.r + COLOR(r * 3.0) / 16.0, p.g + COLOR(g * 3.0) / 16.0, p.b + COLOR(b * 3.0) / 16.0), y+1, x-1);
            
            p = img.GetPixel(y+1, x);
            img.SetPixel( RGB(p.r + COLOR(r * 5.0) / 16.0, p.g + COLOR(g * 5.0) / 16.0, p.b + COLOR(b * 5.0) / 16.0), y+1, x);
            
            p = img.GetPixel(y+1, x + 1);
            img.SetPixel( RGB(p.r + r * 1.0 / 16.0, p.g + g * 1.0 / 16.0, p.b + b * 1.0 / 16.0), y+1, x+1);
        }
    }
    
    img.Write(output_file);
}

//#endif

