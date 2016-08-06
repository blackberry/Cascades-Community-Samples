/****************************************************************************
** Meta object code from reading C++ file 'ODataNetworkManager.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ODataNetworkManager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ODataNetworkManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ODataNetworkManager[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: signature, parameters, type, tag, flags
      30,   21,   20,   20, 0x05,
      50,   21,   20,   20, 0x05,
      70,   21,   20,   20, 0x05,
      89,   21,   20,   20, 0x05,
     115,   21,   20,   20, 0x05,
     141,   21,   20,   20, 0x05,
     166,   20,   20,   20, 0x05,
     185,   20,   20,   20, 0x05,
     204,   20,   20,   20, 0x05,
     236,  223,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
     262,   20,   20,   20, 0x0a,
     276,   20,   20,   20, 0x0a,
     292,   20,   20,   20, 0x0a,
     308,   20,   20,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ODataNetworkManager[] = {
    "ODataNetworkManager\0\0response\0"
    "jsonReady(QVariant)\0atomReady(QVariant)\0"
    "xmlReady(QVariant)\0createJsonReady(QVariant)\0"
    "createAtomReady(QVariant)\0"
    "createXmlReady(QVariant)\0createSuccessful()\0"
    "updateSuccessful()\0deleteSuccessful()\0"
    "code,message\0networkError(int,QString)\0"
    "onReadReply()\0onCreateReply()\0"
    "onUpdateReply()\0onDeleteReply()\0"
};

void ODataNetworkManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ODataNetworkManager *_t = static_cast<ODataNetworkManager *>(_o);
        switch (_id) {
        case 0: _t->jsonReady((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 1: _t->atomReady((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 2: _t->xmlReady((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 3: _t->createJsonReady((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 4: _t->createAtomReady((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 5: _t->createXmlReady((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 6: _t->createSuccessful(); break;
        case 7: _t->updateSuccessful(); break;
        case 8: _t->deleteSuccessful(); break;
        case 9: _t->networkError((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 10: _t->onReadReply(); break;
        case 11: _t->onCreateReply(); break;
        case 12: _t->onUpdateReply(); break;
        case 13: _t->onDeleteReply(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ODataNetworkManager::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ODataNetworkManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ODataNetworkManager,
      qt_meta_data_ODataNetworkManager, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ODataNetworkManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ODataNetworkManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ODataNetworkManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ODataNetworkManager))
        return static_cast<void*>(const_cast< ODataNetworkManager*>(this));
    return QObject::qt_metacast(_clname);
}

int ODataNetworkManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void ODataNetworkManager::jsonReady(QVariant _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ODataNetworkManager::atomReady(QVariant _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ODataNetworkManager::xmlReady(QVariant _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ODataNetworkManager::createJsonReady(QVariant _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ODataNetworkManager::createAtomReady(QVariant _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ODataNetworkManager::createXmlReady(QVariant _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ODataNetworkManager::createSuccessful()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void ODataNetworkManager::updateSuccessful()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void ODataNetworkManager::deleteSuccessful()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void ODataNetworkManager::networkError(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_END_MOC_NAMESPACE
