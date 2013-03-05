/****************************************************************************
** Meta object code from reading C++ file 'SceneViewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SceneViewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SceneViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_koregui__SceneViewer_t {
    QByteArrayData data[5];
    char stringdata[63];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_koregui__SceneViewer_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_koregui__SceneViewer_t qt_meta_stringdata_koregui__SceneViewer = {
    {
QT_MOC_LITERAL(0, 0, 20),
QT_MOC_LITERAL(1, 21, 6),
QT_MOC_LITERAL(2, 28, 0),
QT_MOC_LITERAL(3, 29, 7),
QT_MOC_LITERAL(4, 37, 24)
    },
    "koregui::SceneViewer\0zoomIn\0\0zoomOut\0"
    "addSelectionToRenderview\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_koregui__SceneViewer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x0a,
       3,    0,   30,    2, 0x0a,
       4,    0,   31,    2, 0x0a,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void koregui::SceneViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SceneViewer *_t = static_cast<SceneViewer *>(_o);
        switch (_id) {
        case 0: _t->zoomIn(); break;
        case 1: _t->zoomOut(); break;
        case 2: _t->addSelectionToRenderview(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject koregui::SceneViewer::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_koregui__SceneViewer.data,
      qt_meta_data_koregui__SceneViewer,  qt_static_metacall, 0, 0}
};


const QMetaObject *koregui::SceneViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *koregui::SceneViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_koregui__SceneViewer.stringdata))
        return static_cast<void*>(const_cast< SceneViewer*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int koregui::SceneViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
