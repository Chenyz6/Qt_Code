/****************************************************************************
** Meta object code from reading C++ file 'Filecontrol.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Filecontrol.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Filecontrol.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Filecontrol_t {
    QByteArrayData data[14];
    char stringdata0[152];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Filecontrol_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Filecontrol_t qt_meta_stringdata_Filecontrol = {
    {
QT_MOC_LITERAL(0, 0, 11), // "Filecontrol"
QT_MOC_LITERAL(1, 12, 16), // "ModuleDataRouter"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 5), // "char*"
QT_MOC_LITERAL(4, 36, 11), // "cCmdContent"
QT_MOC_LITERAL(5, 48, 11), // "nDataLength"
QT_MOC_LITERAL(6, 60, 10), // "usInfoType"
QT_MOC_LITERAL(7, 71, 8), // "usDestID"
QT_MOC_LITERAL(8, 80, 11), // "SendFileMsg"
QT_MOC_LITERAL(9, 92, 19), // "Data_File_Attribute"
QT_MOC_LITERAL(10, 112, 10), // "stSendData"
QT_MOC_LITERAL(11, 123, 6), // "Sendok"
QT_MOC_LITERAL(12, 130, 6), // "Finish"
QT_MOC_LITERAL(13, 137, 14) // "stRecvFileData"

    },
    "Filecontrol\0ModuleDataRouter\0\0char*\0"
    "cCmdContent\0nDataLength\0usInfoType\0"
    "usDestID\0SendFileMsg\0Data_File_Attribute\0"
    "stSendData\0Sendok\0Finish\0stRecvFileData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Filecontrol[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   34,    2, 0x06 /* Public */,
       8,    1,   43,    2, 0x06 /* Public */,
      11,    0,   46,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,   47,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::UShort, QMetaType::UShort,    4,    5,    6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 9,   13,

       0        // eod
};

void Filecontrol::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Filecontrol *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ModuleDataRouter((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< unsigned short(*)>(_a[3])),(*reinterpret_cast< unsigned short(*)>(_a[4]))); break;
        case 1: _t->SendFileMsg((*reinterpret_cast< Data_File_Attribute(*)>(_a[1]))); break;
        case 2: _t->Sendok(); break;
        case 3: _t->Finish((*reinterpret_cast< Data_File_Attribute(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Filecontrol::*)(char * , int , unsigned short , unsigned short );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Filecontrol::ModuleDataRouter)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Filecontrol::*)(Data_File_Attribute );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Filecontrol::SendFileMsg)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Filecontrol::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Filecontrol::Sendok)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Filecontrol::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Filecontrol.data,
    qt_meta_data_Filecontrol,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Filecontrol::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Filecontrol::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Filecontrol.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Filecontrol::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Filecontrol::ModuleDataRouter(char * _t1, int _t2, unsigned short _t3, unsigned short _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Filecontrol::SendFileMsg(Data_File_Attribute _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Filecontrol::Sendok()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
struct qt_meta_stringdata_DealData_t {
    QByteArrayData data[7];
    char stringdata0[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DealData_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DealData_t qt_meta_stringdata_DealData = {
    {
QT_MOC_LITERAL(0, 0, 8), // "DealData"
QT_MOC_LITERAL(1, 9, 8), // "Finished"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 19), // "Data_File_Attribute"
QT_MOC_LITERAL(4, 39, 14), // "stRecvFileData"
QT_MOC_LITERAL(5, 54, 11), // "RecvFileMsg"
QT_MOC_LITERAL(6, 66, 10) // "stRecvData"

    },
    "DealData\0Finished\0\0Data_File_Attribute\0"
    "stRecvFileData\0RecvFileMsg\0stRecvData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DealData[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   27,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    6,

       0        // eod
};

void DealData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DealData *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Finished((*reinterpret_cast< Data_File_Attribute(*)>(_a[1]))); break;
        case 1: _t->RecvFileMsg((*reinterpret_cast< Data_File_Attribute(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DealData::*)(Data_File_Attribute );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DealData::Finished)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DealData::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_DealData.data,
    qt_meta_data_DealData,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DealData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DealData::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DealData.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QRunnable"))
        return static_cast< QRunnable*>(this);
    return QObject::qt_metacast(_clname);
}

int DealData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void DealData::Finished(Data_File_Attribute _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
