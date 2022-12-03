#include "py_func.h"
#include "numpy/arrayobject.h"
#include <QDebug>

Py_Func::Py_Func()
{
    setActive();
}

PyObject* Py_Func::mat_CvToNumpy(cv::Mat &img, uchar *m)
{
    import_array();
    npy_intp mdim[] = {img.rows, img.cols, img.channels()};
    PyObject* mat = PyArray_SimpleNewFromData(img.channels(), mdim, NPY_UINT8, (void*) m);
    return mat;
}
