/****************************************************************************
** Meta object code from reading C++ file 'ODataService.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ODataService.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ODataService.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ODataService[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       1,   54, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      30,   13,   13,   13, 0x05,
      48,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      73,   64,   13,   13, 0x0a,
     110,   64,   13,   13, 0x0a,
     146,   64,   13,   13, 0x0a,

 // methods: signature, parameters, type, tag, flags
     186,   13,  177,   13, 0x02,
     209,   13,  177,   13, 0x02,

 // properties: name, type, flags
     231,  223, 0x0a495103,

 // properties: notify_signal_id
       0,

       0        // eod
};

static const char qt_meta_stringdata_ODataService[] = {
    "ODataService\0\0sourceChanged()\0"
    "definitionReady()\0metadataReady()\0"
    "response\0jsonDefinitionReadComplete(QVariant)\0"
    "xmlDefinitionReadComplete(QVariant)\0"
    "metadataReadComplete(QVariant)\0QVariant\0"
    "getServiceDefinition()\0getMetadata()\0"
    "QString\0source\0"
};

void ODataService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ODataService *_t = static_cast<ODataService *>(_o);
        switch (_id) {
        case 0: _t->sourceChanged(); break;
        case 1: _t->definitionReady(); break;
        case 2: _t->metadataReady(); break;
        case 3: _t->jsonDefinitionReadComplete((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 4: _t->xmlDefinitionReadComplete((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 5: _t->metadataReadComplete((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 6: { QVariant _r = _t->getServiceDefinition();
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        case 7: { QVariant _r = _t->getMetadata();
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ODataService::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ODataService::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ODataService,
      qt_meta_data_ODataService, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ODataService::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ODataService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ODataService::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ODataService))
        return static_cast<void*>(const_cast< ODataService*>(this));
    return QObject::qt_metacast(_clname);
}

int ODataService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getSource(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSource(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ODataService::sourceChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ODataService::definitionReady()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void ODataService::metadataReady()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
