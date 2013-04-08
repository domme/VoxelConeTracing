/****************************************************************************
** Meta object code from reading C++ file 'ShaderEditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ShaderEditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ShaderEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_koregui__ShaderEditor_t {
    QByteArrayData data[12];
    char stringdata[144];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_koregui__ShaderEditor_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_koregui__ShaderEditor_t qt_meta_stringdata_koregui__ShaderEditor = {
    {
QT_MOC_LITERAL(0, 0, 21),
QT_MOC_LITERAL(1, 22, 16),
QT_MOC_LITERAL(2, 39, 0),
QT_MOC_LITERAL(3, 40, 4),
QT_MOC_LITERAL(4, 45, 19),
QT_MOC_LITERAL(5, 65, 9),
QT_MOC_LITERAL(6, 75, 11),
QT_MOC_LITERAL(7, 87, 7),
QT_MOC_LITERAL(8, 95, 12),
QT_MOC_LITERAL(9, 108, 17),
QT_MOC_LITERAL(10, 126, 3),
QT_MOC_LITERAL(11, 130, 12)
    },
    "koregui::ShaderEditor\0setShaderProgram\0"
    "\0name\0addNewShaderProgram\0addShader\0"
    "nameChanged\0refresh\0applyChanges\0"
    "pathButtonPressed\0row\0removeShader\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_koregui__ShaderEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x0a,
       4,    0,   57,    2, 0x0a,
       5,    0,   58,    2, 0x0a,
       6,    1,   59,    2, 0x0a,
       7,    0,   62,    2, 0x0a,
       8,    0,   63,    2, 0x0a,
       9,    1,   64,    2, 0x0a,
      11,    1,   67,    2, 0x0a,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::Int,   10,

       0        // eod
};

void koregui::ShaderEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ShaderEditor *_t = static_cast<ShaderEditor *>(_o);
        switch (_id) {
        case 0: _t->setShaderProgram((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->addNewShaderProgram(); break;
        case 2: _t->addShader(); break;
        case 3: _t->nameChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->refresh(); break;
        case 5: _t->applyChanges(); break;
        case 6: _t->pathButtonPressed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->removeShader((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject koregui::ShaderEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_koregui__ShaderEditor.data,
      qt_meta_data_koregui__ShaderEditor,  qt_static_metacall, 0, 0}
};


const QMetaObject *koregui::ShaderEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *koregui::ShaderEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_koregui__ShaderEditor.stringdata))
        return static_cast<void*>(const_cast< ShaderEditor*>(this));
    return QWidget::qt_metacast(_clname);
}

int koregui::ShaderEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
