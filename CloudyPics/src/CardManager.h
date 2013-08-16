/* Copyright (c) 2012 Research In Motion Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * CardManager.h
 *
 *  Created on: Apr 18, 2013
 *      Author: pbernhardt
 */

#ifndef CARDMANAGER_H_
#define CARDMANAGER_H_

#include <bb/system/CardDoneMessage.hpp>
#include <bb/system/ApplicationStartupMode>
#include <bb/system/InvokeManager.hpp>
#include <bb/system/InvokeRequest.hpp>
#include <bb/cascades/multimedia/Camera>

namespace bb {
namespace community {
namespace camera {

using namespace bb::system;
using namespace bb::cascades::multimedia;

class CardManager: public QObject {
	Q_OBJECT

public:
	CardManager(QObject *parent =0);
	virtual ~CardManager();

public slots:
	//Handle the various invocation signals
	void onInvoked(const bb::system::InvokeRequest&);
	void onCardPooled(const bb::system::CardDoneMessage&);

	// Send a signal back with the image path
	void triggerCardDone(const QString& data);

	InvokeManager* getInvokeManager();
	void setCamera(Camera *camera);

private:
	InvokeManager *_invokeManager;
	bool _invoked;
	bool _pooled;
	Camera *_camera;

};

} /* namespace camera */
} /* namespace community */
} /* namespace bb */
#endif /* CARDMANAGER_H_ */
