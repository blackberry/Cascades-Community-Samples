/****************************************************************************
** Meta object code from reading C++ file 'ODataListModel.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ODataListModel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ODataListModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ODataListModel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       2,   64, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      32,   15,   15,   15, 0x05,
      63,   50,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      96,   87,   15,   15, 0x0a,
     120,   87,   15,   15, 0x0a,
     144,   50,   15,   15, 0x0a,

 // methods: signature, parameters, type, tag, flags
     182,  172,  163,   15, 0x02,
     205,  172,  201,   15, 0x02,
     235,  172,  230,   15, 0x02,
     261,   15,   15,   15, 0x02,

 // properties: name, type, flags
     279,  271, 0x0a495103,
     286,  201, 0x02495103,

 // properties: notify_signal_id
       0,
       1,

       0        // eod
};

static const char qt_meta_stringdata_ODataListModel[] = {
    "ODataListModel\0\0sourceChanged()\0"
    "pageSizeChanged()\0code,message\0"
    "modelError(int,QString)\0response\0"
    "jsonCompleted(QVariant)\0atomCompleted(QVariant)\0"
    "error(int,QString)\0QVariant\0indexPath\0"
    "data(QVariantList)\0int\0childCount(QVariantList)\0"
    "bool\0hasChildren(QVariantList)\0refresh()\0"
    "QString\0source\0pageSize\0"
};

void ODataListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ODataListModel *_t = static_cast<ODataListModel *>(_o);
        switch (_id) {
        case 0: _t->sourceChanged(); break;
        case 1: _t->pageSizeChanged(); break;
        case 2: _t->modelError((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: _t->jsonCompleted((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 4: _t->atomCompleted((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 5: _t->error((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 6: { QVariant _r = _t->data((*reinterpret_cast< const QVariantList(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        case 7: { int _r = _t->childCount((*reinterpret_cast< const QVariantList(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 8: { bool _r = _t->hasChildren((*reinterpret_cast< const QVariantList(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 9: _t->refresh(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ODataListModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ODataListModel::staticMetaObject = {
    { &bb::cascades::DataModel::staticMetaObject, qt_meta_stringdata_ODataListModel,
      qt_meta_data_ODataListModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ODataListModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ODataListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ODataListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ODataListModel))
        return static_cast<void*>(const_cast< ODataListModel*>(this));
    typedef bb::cascades::DataModel QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int ODataListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef bb::cascades::DataModel QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getSource(); break;
        case 1: *reinterpret_cast< int*>(_v) = getPageSize(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSource(*reinterpret_cast< QString*>(_v)); break;
        case 1: setPageSize(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ODataListModel::sourceChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ODataListModel::pageSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void ODataListModel::modelError(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
