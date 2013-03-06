/*
*  This sample used the original Filepicker sample and added a share button
*
*  Developed by Steven Kader
*  The Jared Compoany
*
*  You can use this sample for anything that you want in part or in whole
*/

#ifndef MAINAPP_HPP_
#define MAINAPP_HPP_
#include <QtCore/QObject>
#include <QtCore/QMetaType>
#include <QObject>
class MainApp : public QObject {
Q_OBJECT
public:
	MainApp(QObject *parent = 0);

	virtual ~MainApp();

public Q_SLOTS:
	void  shareFile(QString fileName);
	void  onArmed();
};

#endif /* MAINAPP_HPP_ */
