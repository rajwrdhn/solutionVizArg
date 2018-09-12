/****************************************************************************
** Meta object code from reading C++ file 'solution.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/solution.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'solution.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Solution_t {
    QByteArrayData data[9];
    char stringdata[135];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Solution_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Solution_t qt_meta_stringdata_Solution = {
    {
QT_MOC_LITERAL(0, 0, 8),
QT_MOC_LITERAL(1, 9, 23),
QT_MOC_LITERAL(2, 33, 0),
QT_MOC_LITERAL(3, 34, 18),
QT_MOC_LITERAL(4, 53, 13),
QT_MOC_LITERAL(5, 67, 20),
QT_MOC_LITERAL(6, 88, 13),
QT_MOC_LITERAL(7, 102, 17),
QT_MOC_LITERAL(8, 120, 14)
    },
    "Solution\0signalsetSizeOfArgument\0\0"
    "calculateMapReduce\0afSolutionstr\0"
    "calculateShapeRandom\0calculateSize\0"
    "setSizeOfArgument\0calculateColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Solution[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   45,    2, 0x0a /* Public */,
       5,    0,   48,    2, 0x0a /* Public */,
       6,    0,   49,    2, 0x0a /* Public */,
       7,    0,   50,    2, 0x0a /* Public */,
       8,    0,   51,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Solution::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Solution *_t = static_cast<Solution *>(_o);
        switch (_id) {
        case 0: _t->signalsetSizeOfArgument(); break;
        case 1: _t->calculateMapReduce((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->calculateShapeRandom(); break;
        case 3: _t->calculateSize(); break;
        case 4: _t->setSizeOfArgument(); break;
        case 5: _t->calculateColor(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Solution::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Solution::signalsetSizeOfArgument)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Solution::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Solution.data,
      qt_meta_data_Solution,  qt_static_metacall, 0, 0}
};


const QMetaObject *Solution::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Solution::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Solution.stringdata))
        return static_cast<void*>(const_cast< Solution*>(this));
    return QObject::qt_metacast(_clname);
}

int Solution::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Solution::signalsetSizeOfArgument()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
