// Default empty project template
#ifndef CscMonitor_HPP_
#define CscMonitor_HPP_

#include <QObject>
#include <QString>
#include <QSettings>
#include <QFuture>
#include <QMutex>
#include <QFutureWatcher>
#include "BluetoothHandler.hpp"
#include <bb/cascades/TextArea>

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class CscMonitor : public QObject
{
    Q_OBJECT
public:
    CscMonitor(bb::cascades::Application *app);
    virtual ~CscMonitor() {}
	Q_INVOKABLE	void monitor(QString device_addr, QString device_name);
	Q_INVOKABLE	void stopMonitoring();
//	Q_INVOKABLE	int getMinHr();
//	Q_INVOKABLE	int getMaxHr();
//	Q_INVOKABLE	int getAvgHr();
    Q_INVOKABLE void scanForDevices();
	bb::cascades::Application* _app;

private:
	void logServiceNames(char** p);
	BluetoothHandler* _handler;
	QFuture<void> *_future;
	QFutureWatcher<void> *_watcher;
	QMutex _mutex;
    void showTab(int tab_index);
    void startActivityIndicator();
    void stopActivityIndicator();

public slots:
	void logCscData(const QVariant &wheel,const QVariant &crank);
	void cscDevicesFound();
	void finishedReceiving();
};


#endif /* CscMonitor_HPP_ */
