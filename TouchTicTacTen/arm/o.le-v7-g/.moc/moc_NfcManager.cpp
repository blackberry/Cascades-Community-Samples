/****************************************************************************
** Meta object code from reading C++ file 'NfcManager.hpp'
**
** Created: Tue 25. Sep 02:34:53 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/NfcManager.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NfcManager.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NfcManager[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      32,   11,   11,   11, 0x0a,
      53,   48,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NfcManager[] = {
    "NfcManager\0\0nfcManagerStopped()\0"
    "workerStopped()\0text\0message(QVariant)\0"
};

void NfcManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NfcManager *_t = static_cast<NfcManager *>(_o);
        switch (_id) {
        case 0: _t->nfcManagerStopped(); break;
        case 1: _t->workerStopped(); break;
        case 2: _t->message((*reinterpret_cast< const QVariant(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NfcManager::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NfcManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NfcManager,
      qt_meta_data_NfcManager, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NfcManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NfcManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NfcManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NfcManager))
        return static_cast<void*>(const_cast< NfcManager*>(this));
    return QObject::qt_metacast(_clname);
}

int NfcManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void NfcManager::nfcManagerStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
