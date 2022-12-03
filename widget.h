#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QAction>
#include <opencv2/core.hpp>
extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
    #include "libavdevice/avdevice.h"
    #include "libavformat/version.h"
    #include "libavutil/time.h"
    #include "libavutil/mathematics.h"
    #include "libavutil/imgutils.h"
    #include <zuc128.h>
}

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


class Py_Func;
void zuc(const char *keyFile);
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void readDir(QString _dirname);
       void readFile(QString file_path);
       void readUiArg();
       void setUiArg();
       void yuvToRgb(void* str,AVPixelFormat yuv_fmt,uint32_t str_w, uint32_t str_h);
       void showImag(QString file_path);

   private slots:
        void listWidget_file_itemDoubleClicked(QListWidgetItem *item);
        void on_pushButton_r_open_clicked();
        void grayStat(QImage qim);
        void rgbStat(QImage qim);
        void covStat(QImage qim);
        void simlarStat(QImage qim1, QImage qim2);
        void secure_action_triggered(QAction *action);    //安全特性

signals:
        void sig_GetOneFrame(QImage img);

   private:
       Ui::Widget *ui;
       QHash<QString, QString> filenamePath;    //filename -> file绝对路径
       QHash<QString, AVPixelFormat> codectype; //编码格式
       QImage rgbImage,displayImage;    //

       QByteArray str_date;

       /* ffmpeg变量 */
       uint32_t file_size;
       uint32_t yuv_w,yuv_h,yuv_size;
       uint32_t rgb_w,rgb_h,rgb_size;
       AVPixelFormat cur_yuv_fmt;

       uint8_t *yuvBuffer;
       AVFrame *pFrame ;
       AVFrame *pFrameRGB;

       uint8_t * rgbBuffer;
       SwsContext *img_convert_ctx;

       QString cur_img_path;
       int n_count;
       int index;

       /* action */
       QAction* action_gray; //灰度图统计
       QAction* action_cov;  //相邻像素相关性
       QAction* action_key;  //密钥分析
       QAction* action_rgbMean;  //rgb均值统计
       QAction* action_decrypt;  //解密
       /* 数量 */
       unsigned long cryLen;
       u32 *zucBuffer;
       /* python模块 */
       Py_Func *py_secure_analy;
       /* 私有函数 */
       void init_secure_menu();
       void display_yuv();
       void secureAnaly(QImage &qim, const char* funcName);
       void secureAnaly2(QImage &qim1, QImage &qim2, const char* funcName);

       /* 默认配置 */
       QString dirname = "E:\\yuv";    //绝对路径
};
#endif // WIDGET_H
