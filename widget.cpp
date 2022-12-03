#include "widget.h"
#include "ui_widget.h"
#include <QDir>
#include <QPainter>
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QDesktopServices>
#include <QMenu>
#include "py_func.h"
#include <fstream>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    //ui
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "YUV");
    ui->tabWidget->setTabText(1,"VIDEO");
    //init
    codectype = {
        {"YUYV422", AV_PIX_FMT_YUYV422},
        {"YUYV422P", AV_PIX_FMT_YUV422P},
        {"YUYV420P", AV_PIX_FMT_YUV420P},
    };
    init_secure_menu(); //分析按钮
    readDir(dirname);
    connect(ui->listwidget_r_dir ,&QListWidget::itemDoubleClicked, this, &Widget::listWidget_file_itemDoubleClicked);
    py_secure_analy = new Py_Func("logistic");

    //zuc初始化
    zuc("E:\\zuckey");
}

Widget::~Widget()
{
    free(zucBuffer);
    zucBuffer = nullptr;
    delete py_secure_analy;
    delete ui;
}

void zuc(const char *keyFile)
{

    std::ifstream keyText(keyFile, std::ios_base::in);
    u8 key[32];
    for (int i = 0; i < 32; ++i)
    {
        keyText >> key[i];
    }
    u8 iv[25];
    for (int i = 0; i < 25; ++i)
    {
        iv[i] = i;
        if (i >= 17)
        {
            iv[i] &= 0x3F;
        }
    }
    Initialization(key, iv);
}


void Widget::yuvToRgb(void *yuvBuffer,AVPixelFormat yuv_fmt, uint32_t str_w, uint32_t str_h)
{
    rgb_w = str_w;
    rgb_h = str_h;

    yuv_size = str_w * str_h * 3 /2;

    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    rgb_size = av_image_get_buffer_size(AV_PIX_FMT_RGB32, rgb_w, rgb_h, 1);
    rgbBuffer = (uint8_t *) av_malloc(rgb_size * sizeof(uint8_t));

    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, rgbBuffer, AV_PIX_FMT_RGB32, rgb_w, rgb_h,1);

    //转换上下文
    img_convert_ctx = sws_getContext(str_w, str_h, yuv_fmt, rgb_w, rgb_h, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    av_image_fill_arrays(pFrame->data,pFrame->linesize,(uint8_t *)yuvBuffer, yuv_fmt, str_w,str_h,1);

    //转换
    sws_scale(img_convert_ctx,(uint8_t const * const *) pFrame->data,\
              pFrame->linesize, 0, str_h, pFrameRGB->data, pFrameRGB->linesize);
    for(int i=0;i<10000;i++)
        for(int i=0;i<1000;i++);
    QImage tmpImg((uchar *)rgbBuffer,rgb_w,rgb_h,QImage::Format_RGB32);
    rgbImage = tmpImg.copy();
}

void Widget::display_yuv()
{
    if (rgbImage.size().width() <= 0) return;
    //将图像按比例缩放成和窗口一样大小

    QImage img = rgbImage.scaled(ui->label_l_display->size(),Qt::KeepAspectRatio);
    ui->label_l_display->setPixmap(QPixmap::fromImage(img));  //画出图像

}

void Widget::readDir(QString _dirname)
{
    QDir mydir(_dirname);
    QStringList filelist=mydir.entryList();
    for(int i=0;i<filelist.size();i++)
    {
        QString path=QString("%1\\%2").arg(_dirname).arg(filelist.at(i));
        if(filelist.at(i)=="."||filelist.at(i)=="..")
            continue;
        QFileInfo myinfo(path);
        if(myinfo.isDir())
        {
            readDir(path);//递归
        }
        if(myinfo.isFile())
        {
            if(filelist.at(i).contains(".yuv"))
            {
                ui->listwidget_r_dir->addItem(filelist.at(i));
                filenamePath.insert(filelist.at(i),path);
            }
        }
    }
}

void Widget::listWidget_file_itemDoubleClicked(QListWidgetItem *item){
    QString path = filenamePath[item->text()];
    QFile file(path);
    ui->lineEdit_l_path->setText(path);//路径显示
    file.open(QIODevice::ReadOnly);
    auto buff_q = file.readAll();
    int w = 0;
    int h = 0;
    qDebug() << buff_q.size();
    if(buff_q.size() == 614400){
        h = 480;
        w = 640;
    }else{
        h = 600;
        w = 800;
    }
    yuvToRgb(buff_q.data(), codectype[ui->comboBox_codecType->currentText()], w, h);
    display_yuv();
    file.close();
}


void Widget::on_pushButton_r_open_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(dirname));
}

void Widget::init_secure_menu(){
    QMenu *menuSlection = new QMenu(this);
    action_key = new QAction("密钥敏感分析",this);
    action_cov = new QAction("相邻像素相关性",this);
    action_gray = new QAction("灰度直方统计",this);
    action_rgbMean = new QAction("rgb均值统计",this);
    action_decrypt = new QAction("图像解密",this);
    menuSlection->addAction(action_cov);
    menuSlection->addAction(action_key);
    menuSlection->addAction(action_gray);
    menuSlection->addAction(action_rgbMean);
    menuSlection->addAction(action_decrypt);

    ui->pushButton_r_secure->setMenu(menuSlection);
    connect(menuSlection, &QMenu::triggered, this, &Widget::secure_action_triggered);
}

void Widget::secure_action_triggered(QAction *action)
{
    py_secure_analy->setActive();
    if(action == action_key)
    {
        //base
        QString path = filenamePath["2022-11-25_15-06-43_E.yuv"];
        QFile file(path);
        file.open(QIODevice::ReadOnly);
        auto buff_q = file.readAll();
        yuvToRgb(buff_q.data(), codectype[ui->comboBox_codecType->currentText()], 640, 480);
        QImage img1 = rgbImage;
        //current
        QString path_cur = filenamePath[ui->listwidget_r_dir->currentItem()->text()];
        qDebug() << path_cur;
        QFile file_cur(path_cur);
        ui->lineEdit_l_path->setText(path_cur);//路径显示
        file_cur.open(QIODevice::ReadOnly);
        auto buff_q_cur = file.readAll();
        yuvToRgb(buff_q.data(), codectype[ui->comboBox_codecType->currentText()], 640, 480);
        simlarStat(img1, rgbImage);

        file.close();
        file_cur.close();
    }
    else if(action == action_cov)
    {
        this->covStat(rgbImage);
    }else if(action == action_gray)
    {
        this->grayStat(rgbImage);
    }
    else if(action == action_rgbMean)
    {
        this->rgbStat(rgbImage);
    }
    else if(action == action_decrypt)
    {
        QString path = filenamePath[ui->listwidget_r_dir->currentItem()->text()];
        qDebug() << path;
        QFile file(path);
        ui->lineEdit_l_path->setText(path);//路径显示
        file.open(QIODevice::ReadOnly);
        auto buff_q = file.readAll();
        const unsigned long byteLen = buff_q.size();
        const unsigned long cryLen = byteLen / 4;
        if(zucBuffer != nullptr) free(zucBuffer);
        zucBuffer = (u32 *)malloc(sizeof(u32) * cryLen);
        assert(zucBuffer != NULL);
        GenerateKeystream(zucBuffer, cryLen);
        u8 *pBuffer = (u8 *)zucBuffer;

        for(unsigned long i = 0;i < byteLen; ++i){
            buff_q.data()[i] ^= pBuffer[i];
        }
        yuvToRgb(buff_q.data(), codectype[ui->comboBox_codecType->currentText()], 640, 480);
        display_yuv();
        if(zucBuffer != nullptr){
            free(zucBuffer);
            zucBuffer = nullptr;
        }
        file.close();
    }

}

void Widget::grayStat(QImage qim)
{
    secureAnaly(qim, "grayTest");
}

void Widget::covStat(QImage qim){
    secureAnaly(qim, "covTest");
}

void Widget::rgbStat(QImage qim){
    secureAnaly(qim, "rgbTest");
}

void Widget::simlarStat(QImage qim1, QImage qim2){
    secureAnaly2(qim1, qim2, "simlarTest");
}

void Widget::secureAnaly(QImage &qim, const char* funcName){
    qim = qim.rgbSwapped().convertToFormat(QImage::Format_RGB888);
    cv::Mat mat = cv::Mat(qim.height(), qim.width(),
                          CV_8UC3,(void*)qim.constBits(),qim.bytesPerLine());
    int r = mat.rows;
    int c = mat.cols;
    int chnl = mat.channels();
    int bufSize = r * c * chnl;
    uchar* buff = new uchar[bufSize];
    std::memcpy(buff, mat.data,  bufSize * sizeof(uchar));
    /* args & call */
    qDebug() << mat.rows << " x " << mat.cols;
    auto pymat = Py_Func::mat_CvToNumpy(mat, buff);
    PyObject* args = Py_BuildValue("(O)", pymat);
    py_secure_analy->setFunc(funcName);
    py_secure_analy->call(args);
    delete[] buff;
}

void Widget::secureAnaly2(QImage &qim1, QImage &qim2, const char* funcName){
    qim1 = qim1.rgbSwapped().convertToFormat(QImage::Format_RGB888);
    cv::Mat mat1 = cv::Mat(qim1.height(), qim1.width(),
                          CV_8UC3,(void*)qim1.constBits(),qim1.bytesPerLine());
    int r1 = mat1.rows;
    int c1 = mat1.cols;
    int chnl1 = mat1.channels();
    int bufSize1 = r1 * c1 * chnl1;
    uchar* buff1 = new uchar[bufSize1];
    std::memcpy(buff1, mat1.data,  bufSize1 * sizeof(uchar));

    qim2 = qim2.rgbSwapped().convertToFormat(QImage::Format_RGB888);
    cv::Mat mat2 = cv::Mat(qim2.height(), qim2.width(),
                          CV_8UC3,(void*)qim2.constBits(),qim2.bytesPerLine());
    int r2 = mat2.rows;
    int c2 = mat2.cols;
    int chnl2 = mat2.channels();
    int bufSize2 = r2 * c2 * chnl2;
    uchar* buff2 = new uchar[bufSize2];
    std::memcpy(buff2, mat2.data,  bufSize2 * sizeof(uchar));
    /* args & call */
    qDebug() << mat2.rows << " x " << mat2.cols;
    auto pymat1 = Py_Func::mat_CvToNumpy(mat1, buff1);
    auto pymat2 = Py_Func::mat_CvToNumpy(mat2, buff2);
    PyObject* args = Py_BuildValue("(OO)", pymat1, pymat2);
    py_secure_analy->setFunc(funcName);
    py_secure_analy->call(args);
    delete[] buff1;
    delete[] buff2;
}


