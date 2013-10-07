A template for building a cascades library and an application that shows how to use and package the library. Now with assets!

How to Rename the Library Project:
- Rename the project in Momentics
- Edit Makefile:
  - set QMAKE_TARGET to the new name
- Edit mylibrary.prf:
  - rename mylibrary to the new name with no capitals or spaces
  - set LIBTARGET to the new name
- Rename MyLibrary.pro to <new name>.pro
- Rename mylibrary.prf to <new name: no caps or spaces>.prf

Note: There is a known issue with the Momentics IDE overwriting the contents of the bar-descriptor.xml file in certain circumstances. To avoid this issue, import the library before importing the application.
