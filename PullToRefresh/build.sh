#!/bin/bash
TART=blackberry-py/tart
blackberry-nativepackager -package bbpypulltorefresh-debug.bar \
    -configuration Device-Debug \
    -devMode \
    -target bar \
    -env PYTHONPATH=app/native \
    -env PYTHONDONTWRITEBYTECODE=1 \
    -arg -qml -arg app/native/assets/main.qml \
    -arg app/native/blackberry_tart.py \
    bar-descriptor.xml \
    icon.png \
    app/ \
    assets/ \
    adn/ \
    -e requests/requests/ requests/ \
    -C $TART/entry $TART/entry/TartStart \
    -C $TART/js $TART/js/*.js \
    -C $TART/python $TART/python \
    -debugToken debugToken.bar
