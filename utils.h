//
// Created by 彼端。 on 2018/11/21.
//

#ifndef DIGITAL_UTILS_H
#define DIGITAL_UTILS_H

#include <string>
//#include <algorithm>

#include "EasyBMP/EasyBMP.h"

// hsv模型转化
#define BALANCE_STYLE_HSV 0
// rgb三通道均衡
#define BALANCE_STYLE_RGB 1

// 典型离散型均衡
#define BALANCE_TYPE_TYPICAL 0
// 自创高级连续均衡
#define BALANCE_TYPE_ADVANCE 1

#define MAX_IMG_INFO_SIZE 256


using namespace std;

typedef struct {
    BMP bmp;
    BMFH bmfh;
    BMIH bmih;
} BMP_FILE;

BMP_FILE open_bmp(const string &file_name, bool only_read_header = false);

void show_img_info(const string &file_name);

void write_bmp_2_raw(const string &in_file_name, const string &out_file_name="");

void split_single_part(BMP &in_bmp, BMP &out_bmp, int width_block, int width_block_total, int height_block, int height_block_total);

void split_image(const string &in_file_name, int split_x, int split_y);

void scale_near(BMP &in_bmp, BMP &out_bmp, float width_scale, float height_scale);

void scale_near(const string& file, float scale_x, float scale_y);

void scale_interpolation(BMP &in_bmp, BMP &out_bmp, float width_scale, float height_scale);

void scale_interpolation(const string& file, float scale_x, float scale_y);

void scale_pooling_avg(BMP &in_bmp, BMP &out_bmp, int width_scale, int height_scale);

void scale_pooling_avg(const string& file, int scale_x, int scale_y);

void histogram_balance(BMP &in_bmp, BMP &out_bmp, int balance_type=BALANCE_STYLE_HSV, int balance_style=BALANCE_TYPE_TYPICAL);

void histogram_balance(const string& file, int balance_type=BALANCE_STYLE_HSV, int balance_style=BALANCE_TYPE_TYPICAL);


#endif //DIGITAL_UTILS_H
