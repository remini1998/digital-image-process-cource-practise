//
// Created by 彼端。 on 2018/11/21.
//

#ifndef DIGITAL_UTILS_H
#define DIGITAL_UTILS_H

#include <string>

#include "EasyBMP/EasyBMP.h"

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

#endif //DIGITAL_UTILS_H
