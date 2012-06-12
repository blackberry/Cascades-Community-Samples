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
#include "StateManager.hpp"
#include <qdebug>

StateManager* StateManager::_instance;

StateManager::StateManager() {
	qDebug() << "Constructing StateManager";
	setReadState(false);
	setDetectAndWriteState(false);
	qDebug() << "Done constructing StateManager";
}

StateManager* StateManager::getInstance() {
	if (_instance == 0 ) {
		_instance = new StateManager;
	}
	return _instance;
}

bool StateManager::inReadState() const {
	qDebug() << "StateManager::inReadState=" << m_inReadState;
	return m_inReadState;
}

void StateManager::setReadState(bool read_state) {
	qDebug() << "StateManager::setReadState=" << read_state;
	m_inReadState = read_state;
	emit inReadStateChanged();
}

bool StateManager::inDetectAndWriteState() const {
	qDebug() << "StateManager::inDetectAndWriteState=" << m_inDetectAndWriteState;
	return m_inDetectAndWriteState;
}

void StateManager::setDetectAndWriteState(bool detect_and_write_state) {
	qDebug() << "StateManager::setDetectAndWriteState=" << detect_and_write_state;
	m_inDetectAndWriteState = detect_and_write_state;
	emit inDetectAndWriteStateChanged();
}

bool StateManager::inNoNfcState() const {
	bool no_nfc_state = !(m_inReadState || m_inDetectAndWriteState || m_inNdefPushState);
	qDebug() << "StateManager::inNoNfcState=" << no_nfc_state;
	return no_nfc_state;
}

void StateManager::setNoNfcState() {
	qDebug() << "StateManager::setNoNfcState";
	m_inReadState = false;
	m_inDetectAndWriteState = false;
	emit inReadStateChanged();
	emit inDetectAndWriteStateChanged();
}

bool StateManager::inNdefPushState() const {
	qDebug() << "StateManager::inNdefPushState=" << m_inNdefPushState;
	return m_inNdefPushState;
}

void StateManager::setNdefPushState(bool ndef_push_state) {
	qDebug() << "StateManager::setNdefPushState=" << ndef_push_state;
	m_inNdefPushState = ndef_push_state;
	emit inNdefPushStateChanged();
}

