//
// Created by 彼端。 on 2018/11/21.
//

#include <ios>
#include <fstream>
#include <sstream>
#include "utils.h"

using namespace std;

int min(int a, int b){
    return a < b ? a : b;
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

// 最近邻缩放
void scale_near(BMP &in_bmp, BMP &out_bmp, float width_scale, float height_scale){

    try{

        int width = in_bmp.TellWidth();
        int height = in_bmp.TellHeight();

        int new_width = (int) (width * width_scale);
        int new_height = (int) (height * height_scale);

        out_bmp.SetSize(new_width, new_height);
        out_bmp.SetBitDepth(24);
//        out_bmp.SetBitDepth(in_bmp.TellBitDepth());

        for (int x = 0; x < new_width; x++)
            for(int y = 0; y < new_height; y++)
                out_bmp(x, y) = in_bmp((int) (x / width_scale), (int) (y / height_scale));

    }
    catch (const runtime_error& err){
        cout << "*错误* 处理图像时出错！" << endl;
    }

}

void scale_near(const string& file, float scale_x, float scale_y){
    BMP in, out;
    in.ReadFromFile(file);
    cout << "源文件「 " << file << " 」打开成功!" << endl;

    scale_near(in, out, scale_x, scale_y);

    stringstream ss;
    ss << file.substr(0, file.length() - 4);
    if (scale_x == scale_y){
        ss << "_scale_near_" << scale_x << ".bmp";
    } else{
        ss << "_scale_near_" << scale_x << "_" << scale_y << ".bmp";
    }

    out.WriteToFile(ss.str());
    cout << "文件「 " << ss.str() << " 」保存成功!" << endl;
}





class PixelCalcer{
public:
    float Red = 0;
    float Green = 0;
    float Blue = 0;
    float Alpha = 0;

    PixelCalcer() = default;

    PixelCalcer(float red, float green, float blue, float alpha=0){
        Red = red;
        Green = green;
        Blue = blue;
        Alpha = alpha;
    }

    PixelCalcer(RGBApixel& input){
        Red = input.Red;
        Green = input.Green;
        Blue = input.Blue;
        Alpha = input.Alpha;
    }

    PixelCalcer(const PixelCalcer& input){
        Red = input.Red;
        Green = input.Green;
        Blue = input.Blue;
        Alpha = input.Alpha;
    }

    static PixelCalcer add(const PixelCalcer& a, const PixelCalcer& b){
        PixelCalcer result;
        result.Red = a.Red + b.Red;
        result.Green = a.Green + b.Green;
        result.Blue = a.Blue + b.Blue;
        result.Alpha = a.Alpha + b.Alpha;
        return result;
    }

    static PixelCalcer times(const PixelCalcer& in, float rate){
        PixelCalcer result;
        result.Red = in.Red * rate;
        result.Green = in.Green * rate;
        result.Blue = in.Blue * rate;
        result.Alpha = in.Alpha * rate;
        return result;
    }

    RGBApixel to_pixel(){
        RGBApixel out;
        out.Red = (unsigned char) Red;
        out.Green = (unsigned char) Green;
        out.Blue = (unsigned char) Blue;
        out.Alpha = (unsigned char) Alpha;
        return out;
    }

    PixelCalcer operator+(const PixelCalcer &in){
        return add(*this, in);
    }

    PixelCalcer operator+(RGBApixel in){
        return add(*this, PixelCalcer(in));
    }

    PixelCalcer operator-(){
        PixelCalcer r;
        r.Red = this->Red;
        r.Green = this->Green;
        r.Blue = this->Blue;
        r.Alpha = this->Alpha;
        return r;
    }

    PixelCalcer operator-(PixelCalcer in){
        return add(*this, -in);
    }

    PixelCalcer operator-(RGBApixel in){
        return add(*this, -PixelCalcer(in));
    }

    PixelCalcer operator*(float rate){
        return times(*this, rate);
    }

    PixelCalcer operator/(float rate){
        return times(*this, 1 / rate);
    }

};

// 双线性插值
void scale_interpolation(BMP &in_bmp, BMP &out_bmp, float width_scale, float height_scale){

    try{

        int width = in_bmp.TellWidth();
        int height = in_bmp.TellHeight();

        int new_width = (int) (width * width_scale);
        int new_height = (int) (height * height_scale);

        out_bmp.SetSize(new_width, new_height);
        out_bmp.SetBitDepth(24);
//        out_bmp.SetBitDepth(in_bmp.TellBitDepth());

        for (int x = 0; x < new_width; x++)
            for(int y = 0; y < new_height; y++){

                float target_x = x / width_scale;
                float target_y = y / height_scale;

                int start_x = (int) target_x;
                int start_y = (int) target_y;

                // 边界问题
                int end_x = min(start_x + 1, width - 1);
                int end_y = min(start_y + 1, height - 1);

                // 小数部分
                float padding_x = target_x - start_x;
                float padding_y = target_y - start_y;

                PixelCalcer tl = in_bmp(start_x, start_y);
                PixelCalcer tr = in_bmp(end_x, start_y);
                PixelCalcer bl = in_bmp(start_x, end_y);
                PixelCalcer br = in_bmp(end_x, end_y);

                out_bmp(x, y) = (tl * (1 - padding_x) * (1 - padding_y)
                                + (tr * padding_x * (1 - padding_y))
                                + (bl * (1 - padding_x) * padding_y)
                                + (br * padding_x * padding_y)).to_pixel();
            }

    }
    catch (const runtime_error& err){
        cout << "*错误* 处理图像时出错！" << endl;
    }

}



void scale_interpolation(const string& file, float scale_x, float scale_y){
    BMP in, out;
    in.ReadFromFile(file);
    cout << "源文件「 " << file << " 」打开成功!" << endl;

    scale_interpolation(in, out, scale_x, scale_y);

    stringstream ss;
    ss << file.substr(0, file.length() - 4);
    if (scale_x == scale_y){
        ss << "_scale_interpolation_" << scale_x << ".bmp";
    } else{
        ss << "_scale_interpolation_" << scale_x << "_" << scale_y << ".bmp";
    }

    out.WriteToFile(ss.str());
    cout << "文件「 " << ss.str() << " 」保存成功!" << endl;
}



// 平均池化缩小
void scale_pooling_avg(BMP &in_bmp, BMP &out_bmp, int width_scale, int height_scale){

    try{

        int width = in_bmp.TellWidth();
        int height = in_bmp.TellHeight();

        int new_width = (int) (width / width_scale);
        int new_height = (int) (height / height_scale);

        out_bmp.SetSize(new_width, new_height);
        out_bmp.SetBitDepth(24);
//        out_bmp.SetBitDepth(in_bmp.TellBitDepth());

        for (int x = 0; x < new_width; x++)
            for(int y = 0; y < new_height; y++){

                int start_x = x * width_scale;
                int start_y = y * height_scale;

                PixelCalcer total;

                for(int i = 0; i < width_scale; i++)
                    for(int j = 0; j < height_scale; j++){
                        // 边界问题
                        int new_x = min(start_x + i, width - 1);
                        int new_y = min(start_y + j, height - 1);

                        total = total + in_bmp(new_x, new_y);
                    }

                out_bmp(x, y) = (total / width_scale / height_scale).to_pixel();
            }

    }
    catch (const runtime_error& err){
        cout << "*错误* 处理图像时出错！" << endl;
    }

}



void scale_pooling_avg(const string& file, int scale_x, int scale_y){
    BMP in, out;
    in.ReadFromFile(file);
    cout << "源文件「 " << file << " 」打开成功!" << endl;

    scale_pooling_avg(in, out, scale_x, scale_y);

    stringstream ss;
    ss << file.substr(0, file.length() - 4);
    if (scale_x == scale_y){
        ss << "_scale_pooling_avg_" << scale_x << ".bmp";
    } else{
        ss << "_scale_pooling_avg_" << scale_x << "_" << scale_y << ".bmp";
    }

    out.WriteToFile(ss.str());
    cout << "文件「 " << ss.str() << " 」保存成功!" << endl;
}

template <class T>
class ImgBalanceInfo{

    T values[MAX_IMG_INFO_SIZE];

public:

    T get(int num){
        return values[num];
    }

    void set(int num, T val){
        values[num] = val;
    }

    void reset(T val){
        for (int a = 0; a < MAX_IMG_INFO_SIZE; a++){
            values[a] = val;
        }
    }

    ImgBalanceInfo(){

    }

    ImgBalanceInfo(T init_val){
        reset(init_val);
    }

    T& operator[](int i) {
        return values[i];
    }
};

int max_rgb_in_pixel(RGBApixel& pixel){
    return max(pixel.Red, max(pixel.Green, max(pixel.Blue, pixel.Alpha)));
}


ImgBalanceInfo<int> count_histogram_hsv(BMP &in_bmp){
    ImgBalanceInfo<int> raw(0);
    int width = in_bmp.TellWidth();
    int height = in_bmp.TellHeight();

    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++){
            RGBApixel p = in_bmp(x, y);
            int m = max_rgb_in_pixel(p);
            raw[m]++;
        }

    return raw;
}


ImgBalanceInfo<RGBApixel> count_histogram_rgb(BMP &in_bmp){
    ImgBalanceInfo<RGBApixel> raw;
    int width = in_bmp.TellWidth();
    int height = in_bmp.TellHeight();

    // 初始化
    for (int x = 0; x < MAX_IMG_INFO_SIZE; x++){
        raw[x].Red = 0;
        raw[x].Green = 0;
        raw[x].Blue = 0;
        raw[x].Alpha = 0;
    }

    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++){
            RGBApixel p = in_bmp(x, y);
            raw[p.Red].Red++;
            raw[p.Green].Green++;
            raw[p.Blue].Blue++;
            raw[p.Alpha].Alpha++;
        }

    return raw;
}


// 将直方图转化为映射，输入累计数据，输出映射
ImgBalanceInfo<double> acc_rate_histogram(ImgBalanceInfo<int> &in_info){

    // 累计颜色数量
    ImgBalanceInfo<int> acc_info(0);
    int total = 0;
    for (int i = 0; i < MAX_IMG_INFO_SIZE; i++) {
        total += in_info[i];
        acc_info[i] = total;
    }

    // 累计颜色比率
    ImgBalanceInfo<double> rate_info(0);
    for (int i = 0; i < MAX_IMG_INFO_SIZE; i++) {
        rate_info[i] = acc_info[i] / (double) total;
    }

    return rate_info;

}

double random_from_0_to_1(int pos = 5){
    return rand() % (pos + 1) / (float)(pos + 1);
}

int random_0_1(double input){
    return (input < random_from_0_to_1()) ? 1 : 0;
}

void trans_histogram_hsv(BMP &in_bmp, BMP &out_bmp, ImgBalanceInfo<double> &in_info, int balance_type){

    int width = in_bmp.TellWidth();
    int height = in_bmp.TellHeight();

    out_bmp.SetSize(width, height);
    out_bmp.SetBitDepth(24);

    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++){
            // 计算目标映射值
            RGBApixel p = in_bmp(x, y);
            int v = max_rgb_in_pixel(p);
            double acc = in_info[v];
            double target_v = acc * MAX_IMG_INFO_SIZE;

            out_bmp(x, y).Red = (int) (p.Red * target_v / v);
            out_bmp(x, y).Green = (int) (p.Green * target_v / v);
            out_bmp(x, y).Blue = (int) (p.Blue * target_v / v);
            out_bmp(x, y).Alpha = p.Alpha;

            switch (balance_type) {
                case BALANCE_TYPE_TYPICAL:
                    break;
                case BALANCE_TYPE_ADVANCE:

                    // 随机补齐拉平直方图并防止溢出
                    if (out_bmp(x, y).Red < 255)
                        out_bmp(x, y).Red += random_0_1(p.Red * target_v / v - out_bmp(x, y).Red);
                    if (out_bmp(x, y).Green < 255)
                        out_bmp(x, y).Green += random_0_1(p.Green * target_v / v - out_bmp(x, y).Green);
                    if (out_bmp(x, y).Blue < 255)
                        out_bmp(x, y).Blue += random_0_1(p.Blue * target_v / v - out_bmp(x, y).Blue);

                    break;
                default: {
                    throw "没有该处理类型";
                }
            }
        }

}


void trans_histogram_rgb(BMP &in_bmp, BMP &out_bmp,
        ImgBalanceInfo<double> &in_info_r,
        ImgBalanceInfo<double> &in_info_g,
        ImgBalanceInfo<double> &in_info_b,
        int balance_type){

    int width = in_bmp.TellWidth();
    int height = in_bmp.TellHeight();

    out_bmp.SetSize(width, height);
    out_bmp.SetBitDepth(24);

    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++){
            // 计算目标映射值
            RGBApixel p = in_bmp(x, y);
            double acc_r = in_info_r[p.Red];
            double acc_g = in_info_g[p.Green];
            double acc_b = in_info_b[p.Blue];

            out_bmp(x, y).Red = (int) (MAX_IMG_INFO_SIZE * acc_r);
            out_bmp(x, y).Green = (int) (MAX_IMG_INFO_SIZE * acc_g);
            out_bmp(x, y).Blue = (int) (MAX_IMG_INFO_SIZE * acc_b);
            out_bmp(x, y).Alpha = p.Alpha;

            switch (balance_type) {

                case BALANCE_TYPE_TYPICAL:
                    break;

                case BALANCE_TYPE_ADVANCE:

                    // 随机补齐拉平直方图并防止溢出
                    if (out_bmp(x, y).Red < 255)
                        out_bmp(x, y).Red += random_0_1(MAX_IMG_INFO_SIZE * acc_r - out_bmp(x, y).Red);
                    if (out_bmp(x, y).Green < 255)
                        out_bmp(x, y).Green += random_0_1(MAX_IMG_INFO_SIZE * acc_g - out_bmp(x, y).Green);
                    if (out_bmp(x, y).Blue < 255)
                        out_bmp(x, y).Blue += random_0_1(MAX_IMG_INFO_SIZE * acc_b - out_bmp(x, y).Blue);

                    break;

                default: {
                    throw "没有该处理类型";
                }
            }
        }

}




void histogram_balance(BMP &in_bmp, BMP &out_bmp, int balance_type, int balance_style){
    // 统计直方图信息
    ImgBalanceInfo<int> raw;
    switch (balance_style){

        case BALANCE_STYLE_HSV: {
            ImgBalanceInfo<int> count_info = count_histogram_hsv(in_bmp);
            ImgBalanceInfo<double> acc_info = acc_rate_histogram(count_info);
            trans_histogram_hsv(in_bmp, out_bmp, acc_info, balance_type);
        }
            break;

        case BALANCE_STYLE_RGB:{

            ImgBalanceInfo<RGBApixel> count_info = count_histogram_rgb(in_bmp);

            ImgBalanceInfo<int> count_info_r;
            ImgBalanceInfo<int> count_info_g;
            ImgBalanceInfo<int> count_info_b;

            for (int a = 0; a < MAX_IMG_INFO_SIZE; a++){
                count_info_r[a] = count_info[a].Red;
                count_info_g[a] = count_info[a].Green;
                count_info_b[a] = count_info[a].Blue;
            }

            ImgBalanceInfo<double> acc_info_r = acc_rate_histogram(count_info_r);
            ImgBalanceInfo<double> acc_info_g = acc_rate_histogram(count_info_g);
            ImgBalanceInfo<double> acc_info_b = acc_rate_histogram(count_info_b);

            trans_histogram_rgb(in_bmp, out_bmp, acc_info_r, acc_info_g, acc_info_b, balance_type);

        }
            break;

        default: {
            throw "没有该处理风格";
        }
    }
}

void histogram_balance(const string& file, int balance_type, int balance_style){

    BMP in, out;
    in.ReadFromFile(file);
    cout << "源文件「 " << file << " 」打开成功!" << endl;

    histogram_balance(in, out, balance_type, balance_style);

    stringstream ss;
    ss << file.substr(0, file.length() - 4);
    ss << "_histogram_balance_"<< balance_type << balance_style << ".bmp";

    out.WriteToFile(ss.str());
    cout << "文件「 " << ss.str() << " 」保存成功!" << endl;
}
