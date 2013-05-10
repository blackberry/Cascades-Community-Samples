/****************************************************************************
** Meta object code from reading C++ file 'ODataObjectModel.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ODataObjectModel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ODataObjectModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ODataObjectModel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       2,  104, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      31,   17,   17,   17, 0x05,
      47,   17,   17,   17, 0x05,
      62,   17,   17,   17, 0x05,
      77,   17,   17,   17, 0x05,
      92,   17,   17,   17, 0x05,
     120,  107,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     153,  144,   17,   17, 0x0a,
     180,  144,   17,   17, 0x0a,
     207,   17,   17,   17, 0x0a,
     224,   17,   17,   17, 0x0a,
     241,   17,   17,   17, 0x0a,
     258,  107,   17,   17, 0x0a,

 // methods: signature, parameters, type, tag, flags
     277,   17,   17,   17, 0x02,
     323,  305,  292,   17, 0x02,
     390,  359,   17,   17, 0x02,
     469,  439,   17,   17, 0x02,
     518,   17,   17,   17, 0x02,

 // properties: name, type, flags
     540,  532, 0x0a495103,
     556,  547, 0xff495103,

 // properties: notify_signal_id
       1,
       2,

       0        // eod
};

static const char qt_meta_stringdata_ODataObjectModel[] = {
    "ODataObjectModel\0\0modelReady()\0"
    "sourceChanged()\0modelChanged()\0"
    "modelDeleted()\0modelCreated()\0"
    "modelUpdated()\0code,message\0"
    "modelError(int,QString)\0response\0"
    "jsonReadComplete(QVariant)\0"
    "atomReadComplete(QVariant)\0deleteComplete()\0"
    "createComplete()\0updateComplete()\0"
    "error(int,QString)\0refreshModel()\0"
    "QVariantList\0modelKey,metadata\0"
    "getModelStructure(QString,QVariant)\0"
    "postUrl,category,content,links\0"
    "createModel(QString,QString,QVariant,QByteArray)\0"
    "putUrl,category,content,links\0"
    "updateModel(QString,QString,QVariant,QByteArray)\0"
    "deleteModel()\0QString\0source\0QVariant\0"
    "model\0"
};

void ODataObjectModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ODataObjectModel *_t = static_cast<ODataObjectModel *>(_o);
        switch (_id) {
        case 0: _t->modelReady(); break;
        case 1: _t->sourceChanged(); break;
        case 2: _t->modelChanged(); break;
        case 3: _t->modelDeleted(); break;
        case 4: _t->modelCreated(); break;
        case 5: _t->modelUpdated(); break;
        case 6: _t->modelError((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 7: _t->jsonReadComplete((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 8: _t->atomReadComplete((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 9: _t->deleteComplete(); break;
        case 10: _t->createComplete(); break;
        case 11: _t->updateComplete(); break;
        case 12: _t->error((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 13: _t->refreshModel(); break;
        case 14: { QVariantList _r = _t->getModelStructure((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 15: _t->createModel((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3])),(*reinterpret_cast< QByteArray(*)>(_a[4]))); break;
        case 16: _t->updateModel((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3])),(*reinterpret_cast< QByteArray(*)>(_a[4]))); break;
        case 17: _t->deleteModel(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ODataObjectModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ODataObjectModel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ODataObjectModel,
      qt_meta_data_ODataObjectModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ODataObjectModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ODataObjectModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ODataObjectModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ODataObjectModel))
        return static_cast<void*>(const_cast< ODataObjectModel*>(this));
    return QObject::qt_metacast(_clname);
}

int ODataObjectModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getSource(); break;
        case 1: *reinterpret_cast< QVariant*>(_v) = getModel(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSource(*reinterpret_cast< QString*>(_v)); break;
        case 1: setModel(*reinterpret_cast< QVariant*>(_v)); break;
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
void ODataObjectModel::modelReady()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ODataObjectModel::sourceChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void ODataObjectModel::modelChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void ODataObjectModel::modelDeleted()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void ODataObjectModel::modelCreated()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void ODataObjectModel::modelUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void ODataObjectModel::modelError(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
