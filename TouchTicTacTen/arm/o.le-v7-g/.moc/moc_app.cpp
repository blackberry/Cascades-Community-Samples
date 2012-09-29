/****************************************************************************
** Meta object code from reading C++ file 'app.hpp'
**
** Created: Tue 25. Sep 02:34:48 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/app.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'app.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_App[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       2,   54, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
       5,    4,    4,    4, 0x05,
      23,    4,    4,    4, 0x05,
      50,   42,    4,    4, 0x05,

 // slots: signature, parameters, type, tag, flags
      74,   68,    4,    4, 0x08,
     117,  113,    4,    4, 0x0a,
     140,    4,    4,    4, 0x0a,
     153,    4,    4,    4, 0x0a,

 // methods: signature, parameters, type, tag, flags
     167,    4,    4,    4, 0x02,

 // properties: name, type, flags
     184,  180, 0x02495103,
      42,  193, 0xff495003,

 // properties: notify_signal_id
       0,
       1,

       0        // eod
};

static const char qt_meta_stringdata_App[] = {
    "App\0\0numMovesChanged()\0uiMessageChanged()\0"
    "message\0message(QVariant)\0event\0"
    "onTileTouch(bb::cascades::TouchEvent*)\0"
    "msg\0setUiMessage(QVariant)\0ourTurnNow()\0"
    "ourTurnDone()\0resetTiles()\0int\0numMoves\0"
    "QVariant\0"
};

void App::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        App *_t = static_cast<App *>(_o);
        switch (_id) {
        case 0: _t->numMovesChanged(); break;
        case 1: _t->uiMessageChanged(); break;
        case 2: _t->message((*reinterpret_cast< const QVariant(*)>(_a[1]))); break;
        case 3: _t->onTileTouch((*reinterpret_cast< bb::cascades::TouchEvent*(*)>(_a[1]))); break;
        case 4: _t->setUiMessage((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 5: _t->ourTurnNow(); break;
        case 6: _t->ourTurnDone(); break;
        case 7: _t->resetTiles(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData App::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject App::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_App,
      qt_meta_data_App, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &App::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *App::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *App::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_App))
        return static_cast<void*>(const_cast< App*>(this));
    return QObject::qt_metacast(_clname);
}

int App::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        case 0: *reinterpret_cast< int*>(_v) = numMoves(); break;
        case 1: *reinterpret_cast< QVariant*>(_v) = uiMessage(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setNumMoves(*reinterpret_cast< int*>(_v)); break;
        case 1: setUiMessage(*reinterpret_cast< QVariant*>(_v)); break;
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
void App::numMovesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void App::uiMessageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void App::message(const QVariant & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
