/****************************************************************************
** Meta object code from reading C++ file 'argument.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/argument.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'argument.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Argument_t {
    QByteArrayData data[10];
    char stringdata[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Argument_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Argument_t qt_meta_stringdata_Argument = {
    {
QT_MOC_LITERAL(0, 0, 8),
QT_MOC_LITERAL(1, 9, 15),
QT_MOC_LITERAL(2, 25, 0),
QT_MOC_LITERAL(3, 26, 9),
QT_MOC_LITERAL(4, 36, 23),
QT_MOC_LITERAL(5, 60, 11),
QT_MOC_LITERAL(6, 72, 15),
QT_MOC_LITERAL(7, 88, 14),
QT_MOC_LITERAL(8, 103, 15),
QT_MOC_LITERAL(9, 119, 14)
    },
    "Argument\0ArgumentClicked\0\0Argument*\0"
    "openArgumentContextMenu\0startAttack\0"
    "adjustOutAttack\0adjustInAttack\0"
    "removeOutAttack\0removeInAttack"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Argument[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    0,   52,    2, 0x06 /* Public */,
       5,    1,   53,    2, 0x06 /* Public */,
       6,    0,   56,    2, 0x06 /* Public */,
       7,    0,   57,    2, 0x06 /* Public */,
       8,    0,   58,    2, 0x06 /* Public */,
       9,    0,   59,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Argument::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Argument *_t = static_cast<Argument *>(_o);
        switch (_id) {
        case 0: _t->ArgumentClicked((*reinterpret_cast< Argument*(*)>(_a[1]))); break;
        case 1: _t->openArgumentContextMenu(); break;
        case 2: _t->startAttack((*reinterpret_cast< Argument*(*)>(_a[1]))); break;
        case 3: _t->adjustOutAttack(); break;
        case 4: _t->adjustInAttack(); break;
        case 5: _t->removeOutAttack(); break;
        case 6: _t->removeInAttack(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Argument* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Argument* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Argument::*_t)(Argument * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Argument::ArgumentClicked)) {
                *result = 0;
            }
        }
        {
            typedef void (Argument::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Argument::openArgumentContextMenu)) {
                *result = 1;
            }
        }
        {
            typedef void (Argument::*_t)(Argument * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Argument::startAttack)) {
                *result = 2;
            }
        }
        {
            typedef void (Argument::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Argument::adjustOutAttack)) {
                *result = 3;
            }
        }
        {
            typedef void (Argument::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Argument::adjustInAttack)) {
                *result = 4;
            }
        }
        {
            typedef void (Argument::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Argument::removeOutAttack)) {
                *result = 5;
            }
        }
        {
            typedef void (Argument::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Argument::removeInAttack)) {
                *result = 6;
            }
        }
    }
}

const QMetaObject Argument::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Argument.data,
      qt_meta_data_Argument,  qt_static_metacall, 0, 0}
};


const QMetaObject *Argument::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Argument::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Argument.stringdata))
        return static_cast<void*>(const_cast< Argument*>(this));
    if (!strcmp(_clname, "QGraphicsItem"))
        return static_cast< QGraphicsItem*>(const_cast< Argument*>(this));
    if (!strcmp(_clname, "org.qt-project.Qt.QGraphicsItem"))
        return static_cast< QGraphicsItem*>(const_cast< Argument*>(this));
    return QObject::qt_metacast(_clname);
}

int Argument::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Argument::ArgumentClicked(Argument * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Argument::openArgumentContextMenu()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Argument::startAttack(Argument * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Argument::adjustOutAttack()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Argument::adjustInAttack()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void Argument::removeOutAttack()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void Argument::removeInAttack()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}
QT_END_MOC_NAMESPACE
