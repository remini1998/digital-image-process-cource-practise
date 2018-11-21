//
// Created by 彼端。 on 2018/11/21.
//

#include <ios>
#include <fstream>
#include <sstream>
#include "utils.h"

using namespace std;

void show_img_info(const string &file_name) {

    try{
        BMP_FILE r = open_bmp(file_name, true);

        char FileSize[1025];
        sprintf(FileSize, "%0.2f", r.bmfh.bfSize / 1024.0);

        cout << " 文件路径\t: " << file_name << endl
             << " 文件大小\t: " << r.bmfh.bfSize << " bytes ("
             << FileSize << " KB)" << endl
             << " 图像大小\t: " << r.bmih.biWidth << " x " << r.bmih.biHeight << " 像素" << endl
             << " 起始偏移\t: " << r.bmfh.bfOffBits << endl
             << " 图像位数\t: " << r.bmih.biBitCount << " (";
        if (r.bmih.biBitCount != 32) {
            cout << (int) pow(2, (double) r.bmih.biBitCount) << " 颜色)" << endl;
        } else {
            cout << IntPow(2, 24) << " 颜色和 256 透明色度)" << endl;
        }

        cout << endl;
    }
    catch (const runtime_error& err){
        cout << endl;
    }

}



void write_bmp_2_raw(const string &in_file_name, const string &out_file_name){
    try{
        BMP_FILE r = open_bmp(in_file_name);
        cout << "源文件「 " << in_file_name << " 」打开成功!" << endl;
        string target_name;
        if (out_file_name.empty()){
            target_name = in_file_name + ".raw";
        }else{
            target_name = out_file_name;
        }
        ofstream ofs(target_name, ofstream::binary | ofstream::trunc);
        int height = r.bmih.biHeight;
        int width = r.bmih.biWidth;
        for (int h = 0; h < height; h++)
            for(int w = 0; w < width; w++){
                char buffer[3];
                buffer[0] = (char) r.bmp(w, h).Red;
                buffer[1] = (char) r.bmp(w, h).Green;
                buffer[2] = (char) r.bmp(w, h).Blue;
                ofs.write(buffer, 3);
            }
        ofs.close();
        cout << "目标文件「 " << out_file_name << " 」写入成功!" << endl;
        cout << endl;
    }
    catch (const runtime_error& err){
        cout << endl;
    }
}

void split_image(const string &in_file_name, int split_x, int split_y){
    BMP in_file;
    cout << "源文件「 " << in_file_name << " 」打开成功!" << endl;
    in_file.ReadFromFile(in_file_name);
    for(int x = 0; x < split_x; x++)
        for(int y = 0; y < split_y; y++){
            BMP out_file;
            split_single_part(in_file, out_file, x, split_x, y, split_y);
            stringstream ss;
            ss << in_file_name.substr(0, in_file_name.length() - 4);
            ss << "_" << x + 1 << "_" << y + 1 << ".bmp";
            cout << "文件「 " << ss.str() << " 」保存成功!" << endl;
            out_file.WriteToFile(ss.str());
        }
}

void split_single_part(BMP &in_bmp, BMP &out_bmp, int width_block, int width_block_total, int height_block, int height_block_total){

    try{

        int width = in_bmp.TellWidth();
        int height = in_bmp.TellHeight();

        int block_width = width / width_block_total;
        int block_height = height / height_block_total;

        int start_x = block_width * width_block;
        int start_y = block_height * height_block;

        out_bmp.SetSize(block_width, block_height);
        out_bmp.SetBitDepth(24);
//        out_bmp.SetBitDepth(in_bmp.TellBitDepth());

        for (int x = 0; x < block_width; x++)
            for(int y = 0; y < block_height; y++)
                out_bmp(x, y) = in_bmp(start_x + x, start_y + y);

    }
    catch (const runtime_error& err){
        cout << "*错误* 处理图像时出错！" << endl;
    }

}


BMP_FILE open_bmp(const string &file_name, bool only_read_header) {

    // 获取头文件信息
    BMFH bmfh;
    try {
        bmfh = GetBMFH(file_name);
    }
    catch (const runtime_error &error) {
        cout << " *错误* 您指定的文件「 " << file_name << " 」不存在！" << endl
             << "         已跳过该文件 ..." << endl;
        throw runtime_error("未能成功打开文件");
    }

    if (bmfh.bfType != 19778) {
        cout << " *错误* 您指定的文件「 " << file_name << " 」不是 bitmap 函数！" << endl
             << "         已跳过该文件 ..." << endl;
        throw runtime_error("未能成功打开文件");
    } else {
        BMIH bmih = GetBMIH(file_name);

        BMP_FILE re = BMP_FILE();

        if (! only_read_header){
            re.bmp.ReadFromFile(file_name);
        }

        re.bmfh = bmfh;
        re.bmih = bmih;

        return re;
    }

    throw runtime_error("未能成功打开文件");
}