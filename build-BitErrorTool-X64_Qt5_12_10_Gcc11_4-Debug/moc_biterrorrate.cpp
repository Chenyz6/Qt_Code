/****************************************************************************
** Meta object code from reading C++ file 'biterrorrate.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../BitErrorTool/biterrorrate.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'biterrorrate.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BitErrorRateThread_t {
    QByteArrayData data[12];
    char stringdata0[126];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BitErrorRateThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BitErrorRateThread_t qt_meta_stringdata_BitErrorRateThread = {
    {
QT_MOC_LITERAL(0, 0, 18), // "BitErrorRateThread"
QT_MOC_LITERAL(1, 19, 16), // "EmitBitNumResult"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 5), // "char*"
QT_MOC_LITERAL(4, 43, 6), // "result"
QT_MOC_LITERAL(5, 50, 6), // "length"
QT_MOC_LITERAL(6, 57, 13), // "ProgressValue"
QT_MOC_LITERAL(7, 71, 5), // "value"
QT_MOC_LITERAL(8, 77, 15), // "RecBitErrorPara"
QT_MOC_LITERAL(9, 93, 8), // "fileName"
QT_MOC_LITERAL(10, 102, 11), // "bitFileName"
QT_MOC_LITERAL(11, 114, 11) // "startOffset"

    },
    "BitErrorRateThread\0EmitBitNumResult\0"
    "\0char*\0result\0length\0ProgressValue\0"
    "value\0RecBitErrorPara\0fileName\0"
    "bitFileName\0startOffset"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BitErrorRateThread[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   29,    2, 0x06 /* Public */,
       6,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    3,   37,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    5,
    QMetaType::Void, QMetaType::Double,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::UInt,    9,   10,   11,

       0        // eod
};

void BitErrorRateThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BitErrorRateThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->EmitBitNumResult((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->ProgressValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->RecBitErrorPara((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BitErrorRateThread::*)(char * , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BitErrorRateThread::EmitBitNumResult)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BitErrorRateThread::*)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BitErrorRateThread::ProgressValue)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject BitErrorRateThread::staticMetaObject = { {
    &QThread::staticMetaObject,
    qt_meta_stringdata_BitErrorRateThread.data,
    qt_meta_data_BitErrorRateThread,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *BitErrorRateThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BitErrorRateThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BitErrorRateThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int BitErrorRateThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void BitErrorRateThread::EmitBitNumResult(char * _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void BitErrorRateThread::ProgressValue(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_BitErrorRate_t {
    QByteArrayData data[12];
    char stringdata0[193];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BitErrorRate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BitErrorRate_t qt_meta_stringdata_BitErrorRate = {
    {
QT_MOC_LITERAL(0, 0, 12), // "BitErrorRate"
QT_MOC_LITERAL(1, 13, 16), // "EmitBitErrorPara"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 8), // "fileName"
QT_MOC_LITERAL(4, 40, 11), // "bitFileName"
QT_MOC_LITERAL(5, 52, 11), // "startOffset"
QT_MOC_LITERAL(6, 64, 30), // "on_pushButton_filefile_clicked"
QT_MOC_LITERAL(7, 95, 33), // "on_pushButton_bitfilepath_cli..."
QT_MOC_LITERAL(8, 129, 27), // "on_pushButton_start_clicked"
QT_MOC_LITERAL(9, 157, 18), // "onPieSeriesClicked"
QT_MOC_LITERAL(10, 176, 10), // "QPieSlice*"
QT_MOC_LITERAL(11, 187, 5) // "slice"

    },
    "BitErrorRate\0EmitBitErrorPara\0\0fileName\0"
    "bitFileName\0startOffset\0"
    "on_pushButton_filefile_clicked\0"
    "on_pushButton_bitfilepath_clicked\0"
    "on_pushButton_start_clicked\0"
    "onPieSeriesClicked\0QPieSlice*\0slice"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BitErrorRate[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   46,    2, 0x08 /* Private */,
       7,    0,   47,    2, 0x08 /* Private */,
       8,    0,   48,    2, 0x08 /* Private */,
       9,    1,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::UInt,    3,    4,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,

       0        // eod
};

void BitErrorRate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BitErrorRate *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->EmitBitErrorPara((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        case 1: _t->on_pushButton_filefile_clicked(); break;
        case 2: _t->on_pushButton_bitfilepath_clicked(); break;
        case 3: _t->on_pushButton_start_clicked(); break;
        case 4: _t->onPieSeriesClicked((*reinterpret_cast< QPieSlice*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QPieSlice* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BitErrorRate::*)(QString , QString , unsigned int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BitErrorRate::EmitBitErrorPara)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject BitErrorRate::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_BitErrorRate.data,
    qt_meta_data_BitErrorRate,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *BitErrorRate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BitErrorRate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BitErrorRate.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int BitErrorRate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void BitErrorRate::EmitBitErrorPara(QString _t1, QString _t2, unsigned int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
