/*
 * MyBpsHandler.cpp
 *
 *  Created on: 2015-04-13
 *      Author: Roger
 */

#include <src/MyBpsHandler.h>
#include <bps/bps.h>
#include <QDebug>

MyBpsHandler::MyBpsHandler()
{
    // Initialize BPS library
    if (bps_initialize() != BPS_SUCCESS) {
        qDebug() << "Failure to initialize BPS library";
    }

    // Subscribe to locale BPS
    subscribe(locale_get_domain());

    // Request all locale events. This will allow this class to be notified
    // of any locale data changes.
    if (locale_request_events(0) != BPS_SUCCESS) {
        qDebug() << "Failed to request Locale events";
    }
}

MyBpsHandler::~MyBpsHandler()
{
    // Close the BPS library
    bps_shutdown();
}

void MyBpsHandler::event(bps_event_t *event)
{
    const char* locale = "";

    if (bps_event_get_domain(event) == locale_get_domain()) {
        if (LOCALE_INFO == bps_event_get_code(event)) {
            // Read the locale and emit the localeUpdated signal
            locale = locale_event_get_locale(event);
            emit localeUpdated(locale);
        }
    }
}
