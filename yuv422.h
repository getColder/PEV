#ifndef YUV422_H
#define YUV422_H

#define USE_OPENCV 1

#include <QDialog>

using namespace std;
#if USE_OPENCV
    //#include "opencv2/opencv.hpp"
    #include <opencv2/core.hpp>
    #include <opencv2/opencv.hpp>
    using namespace cv;
#endif

// YUV文件流信息 //
#define MAX_PIC_NUM 4
#define PIC_WIDTH 640
#define PIC_HEIGHT 480

class CYUV422
{


public:
    explicit CYUV422(QWidget *parent = 0 );
    ~CYUV422();
public:
    //读取获取yuv422数据，并存放于自身buffer中//
    uint64_t get_yuv422_data(QString pth);
    //将自身buffer中的yuv422数据转换为Mat类型输出（BGR）
    Mat yuv_2_rgb(int height,int width);
private:
    //缓存buffer，用来读取存放yuv422数据//
   unsigned char yuv_buffer[MAX_PIC_NUM*PIC_WIDTH*PIC_HEIGHT*2];

};

#endif
