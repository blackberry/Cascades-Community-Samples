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
#include <QDebug>

StateManager* StateManager::_instance;

StateManager::StateManager() {
	qDebug() << "XXXX Constructing StateManager";
	setReadState(false);
	setDetectAndWriteState(false);
	qDebug() << "XXXX Done constructing StateManager";
}

StateManager* StateManager::getInstance() {
	if (_instance == 0) {
		_instance = new StateManager;
	}
	return _instance;
}

bool StateManager::inReadState() const {
	qDebug() << "XXXX StateManager::inReadState=" << m_inReadState;
	return m_inReadState;
}

void StateManager::setReadState(bool read_state) {
	qDebug() << "XXXX StateManager::setReadState=" << read_state;
	m_inReadState = read_state;
	emit inReadStateChanged();
}

bool StateManager::inDetectAndWriteState() const {
	qDebug() << "XXXX StateManager::inDetectAndWriteState="
			<< m_inDetectAndWriteState;
	return m_inDetectAndWriteState;
}

void StateManager::setDetectAndWriteState(bool detect_and_write_state) {
	qDebug() << "XXXX StateManager::setDetectAndWriteState="
			<< detect_and_write_state;
	m_inDetectAndWriteState = detect_and_write_state;
	emit inDetectAndWriteStateChanged();
}

bool StateManager::inNoNfcState() const {
	bool no_nfc_state = !(m_inReadState || m_inDetectAndWriteState
			|| m_inNdefPushState || m_inTagEmulationState);
	qDebug() << "XXXX StateManager::inNoNfcState=" << no_nfc_state;
	return no_nfc_state;
}

void StateManager::setNoNfcState() {
	qDebug() << "XXXX StateManager::setNoNfcState";
	m_inReadState = false;
	m_inDetectAndWriteState = false;
	m_inNdefPushState = false;
	m_inTagEmulationState = false;
	emit inReadStateChanged();
	emit inDetectAndWriteStateChanged();
	emit inNdefPushStateChanged();
}

bool StateManager::inDefaultState() const {
	bool default_state = m_inReadState
			&& !(m_inDetectAndWriteState || m_inNdefPushState || m_inTagEmulationState);
	qDebug() << "XXXX StateManager::inDefaultState=" << default_state;
	return default_state;
}

void StateManager::setDefaultState() {
	qDebug() << "XXXX StateManager::setDefaultState";
	m_inReadState = true;
	m_inDetectAndWriteState = false;
	m_inNdefPushState = false;
	m_inTagEmulationState = false;
	emit inDetectAndWriteStateChanged();
	emit inNdefPushStateChanged();
}

bool StateManager::inNdefPushState() const {
	qDebug() << "XXXX StateManager::inNdefPushState=" << m_inNdefPushState;
	return m_inNdefPushState;
}

void StateManager::setNdefPushState(bool ndef_push_state) {
	qDebug() << "XXXX StateManager::setNdefPushState=" << ndef_push_state;
	m_inNdefPushState = ndef_push_state;
	emit inNdefPushStateChanged();
}

bool StateManager::isEventLogShowing() const {
	qDebug() << "XXXX StateManager::isEventLogShowing="
			<< m_event_log_is_showing;
	return m_event_log_is_showing;
}

void StateManager::setEventLogShowing(bool event_log_showing) {
	qDebug() << "XXXX StateManager::setEventLogShowing=" << event_log_showing;
	m_event_log_is_showing = event_log_showing;
	emit inEventLogShowingStateChanged();
}

bool StateManager::inTagEmulationState() const {
	qDebug() << "XXXX StateManager::inTagEmulationState=" << m_inTagEmulationState;
	return m_inTagEmulationState;
}

void StateManager::setTagEmulationState(bool tag_emulation_state) {
	qDebug() << "XXXX StateManager::setTagEmulationState=" << tag_emulation_state;
	m_inTagEmulationState = tag_emulation_state;
	emit inTagEmulationStateChanged();
}
