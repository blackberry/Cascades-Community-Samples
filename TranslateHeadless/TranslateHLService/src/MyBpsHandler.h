/*
 * MyBpsHandler.h
 *
 *  Created on: 2015-04-13
 *      Author: Roger
 */

#ifndef MYBPSHANDLER_H_
#define MYBPSHANDLER_H_

#include <QObject>
#include <bps/locale.h>
#include <bb/AbstractBpsEventHandler>

class MyBpsHandler: public QObject, public bb::AbstractBpsEventHandler
{
    Q_OBJECT
public:
    MyBpsHandler();
    virtual ~MyBpsHandler();

    virtual void event(bps_event_t *event);

signals:
    void localeUpdated(const QString& newLocale);
};

#endif /* MYBPSHANDLER_H_ */
