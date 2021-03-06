//
// Created by 彼端。 on 2018/11/21.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "EasyBMP/EasyBMP.h"
#include "utils.h"

using namespace std;

void show_help(bool exit_after = true){
    cout << "【帮助】" << endl << endl;
    cout << "digital info [文件路径] ([文件路径2] ...)" << endl << "\t显示图片信息" << endl << endl;
    cout << "digital trans [源bmp文件路径] ([目标raw文件路径])" << endl << "\t将bmp文件转换为raw文件" << endl << endl;
    cout << "digital split [文件路径] [切分细分数]" << endl << "\t将bmp文件切分成若干份" << endl << endl;
    cout << "digital split [文件路径] [横轴细分数] [纵轴细分数]" << endl << "\t将bmp文件切分成若干份" << endl << endl;
    cout << "digital scale near [文件路径] [缩放倍数]" << endl << "\t使用邻近法缩放bmp图片" << endl << endl;
    cout << "digital scale ip [文件路径] [缩放倍数]" << endl << "\t使用双线性插值法缩放bmp图片" << endl << endl;
    cout << "digital scale pooling [文件路径] [缩小倍数]" << endl << "\t使用平均池化法缩小bmp图片" << endl << endl;
    cout << "digital histogram [文件路径]" << endl << "\t使用2x2种方法进行交叉直方图均衡" << endl << endl;

//    histogram_balance("image_color.bmp");

    if (exit_after){
        exit(1);
    }
}

void show_info_func(int argc, char* argv[], int start=0){
    for(int i = start; i < argc; i++){
        show_img_info(argv[i]);
    }
}



int main(int argc, char* argv[]){

    if ( argc > 1 ) {

        // 检测用户是否需要帮助
        string arg(argv[1]);
        if( arg == "--help" || arg == "-h" || arg == "help") {
            show_help();
        }
        // 显示图片信息
        else if ( arg == "info"){
            show_info_func(argc, argv, 2);
        }
        // 将bmp转换为raw
        else if ( arg == "trans"){
            if (argc < 4){
                write_bmp_2_raw(argv[2]);
            } else{
                write_bmp_2_raw(argv[2], argv[3]);
            }
        }
            // 将bmp转换为raw
        else if ( arg == "split"){
            if (argc < 5){
                stringstream ss;
                ss << argv[3];
                int split;
                ss >> split;
                split_image(argv[2], split, split);
            } else{
                int split_x, split_y;
                stringstream ss;
                ss << argv[3];
                ss >> split_x;
                ss.clear();
                ss << argv[4];
                ss >> split_y;
                split_image(argv[2], split_x, split_y);
            }
        }
            // 将缩放bmp
        else if ( arg == "scale"){
            if (argc < 5){
                show_help();
            } else{
                string method(argv[2]);
                if (method == "near"){
                    float scale;
                    stringstream ss;
                    ss << argv[4];
                    ss >> scale;
                    scale_near(argv[3], scale, scale);
                }
                else if(method == "ip"){
                    float scale;
                    stringstream ss;
                    ss << argv[4];
                    ss >> scale;
                    scale_interpolation(argv[3], scale, scale);
                }
                else if(method == "pooling"){
                    int scale;
                    stringstream ss;
                    ss << argv[4];
                    ss >> scale;
                    scale_pooling_avg(argv[3], scale, scale);
                }else{
                    show_help();
                }
            }
        }
            // 将直方图均衡
        else if ( arg == "histogram"){
            if (argc < 3){
                show_help();
            } else{
                string file_name(argv[2]);
                histogram_balance(file_name, BALANCE_TYPE_TYPICAL, BALANCE_STYLE_HSV);
                histogram_balance(file_name, BALANCE_TYPE_ADVANCE, BALANCE_STYLE_HSV);
                histogram_balance(file_name, BALANCE_TYPE_TYPICAL, BALANCE_STYLE_RGB);
                histogram_balance(file_name, BALANCE_TYPE_ADVANCE, BALANCE_STYLE_RGB);
            }
        }
        else{
            show_help();
        }
    }
    else{
        show_help();
    }

}
