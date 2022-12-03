#include "YUV422.h"
#include "QDebug"

// 转换方式1：https://www.cnblogs.com/lihaiping/p/4441518.html
#define YUV_2_R(y, v)   ((int)( y + 1.402*(v-128)  ))
#define YUV_2_G(y, u, v)    ((int)( 1.0*y - 0.34414*(u-128) - 0.71414*(v-128)   ))
#define YUV_2_B(y, u)    ((int)( 1.0*y + 1.772*(u-128) ))

/*
//转换方式2：https://github.com/kd40629rtlrtl/yuv422_to_rgb
#define YUV_2_R(y, v)((int)(1.0*y + 1.370705  * (1.0*v - 128)))
#define YUV_2_G(y, u, v)((int)(1.0*y - 0.698001 * (1.0*u - 128) - 0.703125 * (1.0*v - 128)))
#define YUV_2_B(y, u) ((int)(1.0*y + 1.732446 * (1.0*u - 128)))

// 转换方式3：https://blog.csdn.net/qq_29350001/article/details/52032540
#define YUV_2_R(y, v)   ((int)( 1.164*(y-16) + 1.596*(v-128)  ))
#define YUV_2_G(y, u, v)    ((int)( 1.164*(y-16) - 0.813*(v-128) - 0.392*(u-128)   ))
#define YUV_2_B(y, u)    ((int)( 1.164*(y-16) + 2.017*(u-128) ))
*/

#define CONSTRAINT(x) {x=x>255?255:x; x= x<0?0:x;}

CYUV422::CYUV422(QWidget *parent)
{
    parent = parent;
}

CYUV422::~CYUV422()
{

}
/*
读取YUV422格式文件，模拟从设备中获取yuv422数据
*/
uint64_t CYUV422::get_yuv422_data(QString pth)
{
    uint64_t file_size = 0 ;
    FILE *fp = fopen((const char*)pth.toStdString().data(),"rb");
    if(fp!=NULL)
    {
        file_size = fread(yuv_buffer,1, sizeof(yuv_buffer),  fp);//fill in yuv_data
        fclose(fp);
    }
    return file_size;
}

/*
将读取到的yuv422数据转换为Mat类型输出（BRG）
*/
Mat CYUV422::yuv_2_rgb(int height,int width)//假定YUV数据存放格式为：yuyvyuyv
{
    Mat rgb_data =Mat(height, width, CV_8UC3 );
    int row=0,col=0;
    for(row=0;row<height;row++)
    {
        for(col=0;col<width;col++)
        {
            int yIdx = 2*(row*width+col);
            int uIdx = 4*((row*width+col)/2) + 1;
            int vIdx = uIdx+2;
            int r = YUV_2_R(yuv_buffer[yIdx], yuv_buffer[vIdx]);
            int g = YUV_2_G(yuv_buffer[yIdx],  yuv_buffer[uIdx],yuv_buffer[vIdx]);
            int b = YUV_2_B(yuv_buffer[yIdx],  yuv_buffer[uIdx]);
            CONSTRAINT(r);
            CONSTRAINT(g);
            CONSTRAINT(b);
            rgb_data.at<cv::Vec3b>(row,col) = cv::Vec3b(b,g,r);
        }
    }
    //cv::imshow("x",rgb_data);
    return rgb_data.clone();
}
