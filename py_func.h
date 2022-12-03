#ifndef PY_FUNC_H
#define PY_FUNC_H

#pragma push_macro("slots")
#undef slots
#include <Python.h>
#pragma pop_macro("slots")
#include <QString>
#include <opencv2/core.hpp>



class Py_Func
{

public:
    Py_Func();
   /* 初始化模块 */
    Py_Func(const char* modulePath) : Py_Func(){
        _pModule = PyImport_ImportModule(modulePath);
    }
    Py_Func(QString modulePath) : Py_Func(){
        _pModule = PyImport_ImportModule(modulePath.toStdString().c_str());
    }
    /*初始化模块与函数*/
    Py_Func(QString modulePath, QString funcName): Py_Func(){
        _pModule = PyImport_ImportModule(modulePath.toStdString().c_str());
        assert(_pModule != nullptr);
        _pFunc = PyObject_GetAttrString(_pModule, funcName.toStdString().c_str());
    }
    Py_Func(const char* modulePath, const char* funcName): Py_Func(){
        _pModule = PyImport_ImportModule(modulePath);
        assert(_pModule != nullptr);
        _pFunc = PyObject_GetAttrString(_pModule, funcName);
    }
    /* 激活python模块 */
    static int setActive(){
        if(Py_IsInitialized()) return 0;
        Py_Initialize();
        if ( !Py_IsInitialized() )
        {
            PyErr_Print();
            return -1;
        }
        return 0;
    }
    /* 取消python模块 */
    static void setInActive(){
        if(!Py_IsInitialized()) return;
        Py_Finalize();
    }
    ~Py_Func(){
        Py_Finalize();
    }
    /* 无参调用 */
    void operator()(){
        this->call();
    }
    void call(){
        assert(Py_IsInitialized());
        assert(_pFunc != nullptr);
        PyObject_CallObject(_pFunc, NULL);
    }
    void call(PyObject* arg){
        assert(Py_IsInitialized());
        assert(_pFunc != nullptr);
        PyObject_CallObject(_pFunc, arg);
    }
/* interface */
    void setFunc(QString func){_pFunc = PyObject_GetAttrString(_pModule, func.toStdString().c_str());}
    static PyObject* mat_CvToNumpy(cv::Mat &img, uchar *m);
private:
    PyObject* _pModule = NULL;   //模块
    PyObject* _pFunc = NULL;     //模块内函数
};

#endif // PY_FUNC_H
