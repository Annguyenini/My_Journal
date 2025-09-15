/****************************************************************************
** Meta object code from reading C++ file 'button.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../include/button.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'button.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_Buttons_t {
    uint offsetsAndSizes[18];
    char stringdata0[8];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[11];
    char stringdata4[8];
    char stringdata5[14];
    char stringdata6[9];
    char stringdata7[11];
    char stringdata8[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_Buttons_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_Buttons_t qt_meta_stringdata_Buttons = {
    {
        QT_MOC_LITERAL(0, 7),  // "Buttons"
        QT_MOC_LITERAL(8, 9),  // "callAlbum"
        QT_MOC_LITERAL(18, 0),  // ""
        QT_MOC_LITERAL(19, 10),  // "callCamera"
        QT_MOC_LITERAL(30, 7),  // "callLog"
        QT_MOC_LITERAL(38, 13),  // "callReloadMap"
        QT_MOC_LITERAL(52, 8),  // "callSnap"
        QT_MOC_LITERAL(61, 10),  // "callRecord"
        QT_MOC_LITERAL(72, 16)   // "callPhotoSetting"
    },
    "Buttons",
    "callAlbum",
    "",
    "callCamera",
    "callLog",
    "callReloadMap",
    "callSnap",
    "callRecord",
    "callPhotoSetting"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_Buttons[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x06,    1 /* Public */,
       3,    0,   57,    2, 0x06,    2 /* Public */,
       4,    0,   58,    2, 0x06,    3 /* Public */,
       5,    0,   59,    2, 0x06,    4 /* Public */,
       6,    0,   60,    2, 0x06,    5 /* Public */,
       7,    0,   61,    2, 0x06,    6 /* Public */,
       8,    0,   62,    2, 0x06,    7 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject Buttons::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Buttons.offsetsAndSizes,
    qt_meta_data_Buttons,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_Buttons_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Buttons, std::true_type>,
        // method 'callAlbum'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'callCamera'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'callLog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'callReloadMap'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'callSnap'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'callRecord'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'callPhotoSetting'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Buttons::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Buttons *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->callAlbum(); break;
        case 1: _t->callCamera(); break;
        case 2: _t->callLog(); break;
        case 3: _t->callReloadMap(); break;
        case 4: _t->callSnap(); break;
        case 5: _t->callRecord(); break;
        case 6: _t->callPhotoSetting(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Buttons::*)();
            if (_t _q_method = &Buttons::callAlbum; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Buttons::*)();
            if (_t _q_method = &Buttons::callCamera; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Buttons::*)();
            if (_t _q_method = &Buttons::callLog; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Buttons::*)();
            if (_t _q_method = &Buttons::callReloadMap; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Buttons::*)();
            if (_t _q_method = &Buttons::callSnap; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Buttons::*)();
            if (_t _q_method = &Buttons::callRecord; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Buttons::*)();
            if (_t _q_method = &Buttons::callPhotoSetting; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
    }
    (void)_a;
}

const QMetaObject *Buttons::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Buttons::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Buttons.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Buttons::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Buttons::callAlbum()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Buttons::callCamera()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Buttons::callLog()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Buttons::callReloadMap()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Buttons::callSnap()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void Buttons::callRecord()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void Buttons::callPhotoSetting()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
