/****************************************************************************
** Meta object code from reading C++ file 'FrameBufferEditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../FrameBufferEditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FrameBufferEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_koregui__FrameBufferEditor_t {
    QByteArrayData data[8];
    char stringdata[121];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_koregui__FrameBufferEditor_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_koregui__FrameBufferEditor_t qt_meta_stringdata_koregui__FrameBufferEditor = {
    {
QT_MOC_LITERAL(0, 0, 26),
QT_MOC_LITERAL(1, 27, 17),
QT_MOC_LITERAL(2, 45, 0),
QT_MOC_LITERAL(3, 46, 17),
QT_MOC_LITERAL(4, 64, 20),
QT_MOC_LITERAL(5, 85, 3),
QT_MOC_LITERAL(6, 89, 22),
QT_MOC_LITERAL(7, 112, 7)
    },
    "koregui::FrameBufferEditor\0addNewFramebuffer\0"
    "\0addNewColorTarget\0useDepthbufferTarget\0"
    "use\0useStencilbufferTarget\0refresh\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_koregui__FrameBufferEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x0a,
       3,    0,   40,    2, 0x0a,
       4,    1,   41,    2, 0x0a,
       6,    1,   44,    2, 0x0a,
       7,    0,   47,    2, 0x0a,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void,

       0        // eod
};

void koregui::FrameBufferEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FrameBufferEditor *_t = static_cast<FrameBufferEditor *>(_o);
        switch (_id) {
        case 0: _t->addNewFramebuffer(); break;
        case 1: _t->addNewColorTarget(); break;
        case 2: _t->useDepthbufferTarget((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->useStencilbufferTarget((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->refresh(); break;
        default: ;
        }
    }
}

const QMetaObject koregui::FrameBufferEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_koregui__FrameBufferEditor.data,
      qt_meta_data_koregui__FrameBufferEditor,  qt_static_metacall, 0, 0}
};


const QMetaObject *koregui::FrameBufferEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *koregui::FrameBufferEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_koregui__FrameBufferEditor.stringdata))
        return static_cast<void*>(const_cast< FrameBufferEditor*>(this));
    return QWidget::qt_metacast(_clname);
}

int koregui::FrameBufferEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
