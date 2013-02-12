#!/bin/bash

for f in app/*.pyc; do rm $f; done
rm -rf app/__pycache__
for f in app/*.py; do python3 cp.py $f; done
cp app/__pycache__/* app/
cd app/
for f in *.pyc; do mv $f `basename $f .cpython-32.pyc`.pyc; done
cd ..

TART=blackberry-py/tart
blackberry-nativepackager -package bbpypulltorefresh-release.bar \
    -configuration Device-Release \
    -target bar \
    -env PYTHONPATH=app/native \
    -env PYTHONDONTWRITEBYTECODE=1 \
    -arg -qml -arg app/native/assets/main.qml \
    -arg app/native/blackberry_tart.py \
    bar-descriptor.xml \
    icon.png \
    app/*.pyc \
    assets/ \
    -e requests/requests/ requests/ \
    adn/ \
    -C $TART/entry $TART/entry/TartStart.so \
    -C $TART/js $TART/js/*.js \
    -C $TART/python $TART/python
