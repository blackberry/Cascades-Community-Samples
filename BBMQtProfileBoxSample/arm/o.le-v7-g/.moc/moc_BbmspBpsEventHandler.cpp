/****************************************************************************
** Meta object code from reading C++ file 'BbmspBpsEventHandler.hpp'
**
** Created: Thu Sep 6 13:35:59 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/BbmspBpsEventHandler.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BbmspBpsEventHandler.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BbmspBpsEventHandler[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      28,   22,   21,   21, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_BbmspBpsEventHandler[] = {
    "BbmspBpsEventHandler\0\0event\0"
    "bbmsp_register_callback(bbmsp_event_t*)\0"
};

void BbmspBpsEventHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BbmspBpsEventHandler *_t = static_cast<BbmspBpsEventHandler *>(_o);
        switch (_id) {
        case 0: _t->bbmsp_register_callback((*reinterpret_cast< bbmsp_event_t*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData BbmspBpsEventHandler::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BbmspBpsEventHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BbmspBpsEventHandler,
      qt_meta_data_BbmspBpsEventHandler, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BbmspBpsEventHandler::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BbmspBpsEventHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BbmspBpsEventHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BbmspBpsEventHandler))
        return static_cast<void*>(const_cast< BbmspBpsEventHandler*>(this));
    if (!strcmp(_clname, "bb::AbstractBpsEventHandler"))
        return static_cast< bb::AbstractBpsEventHandler*>(const_cast< BbmspBpsEventHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int BbmspBpsEventHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void BbmspBpsEventHandler::bbmsp_register_callback(bbmsp_event_t * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
