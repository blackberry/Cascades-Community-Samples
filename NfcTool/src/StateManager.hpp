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
#ifndef STATEMANAGER_HPP_
#define STATEMANAGER_HPP_

#include <QtCore/QObject>
#include <bb/cascades/QmlDocument>

class StateManager: public QObject {

	Q_OBJECT
	Q_PROPERTY(bool inReadState READ inReadState WRITE setReadState NOTIFY inReadStateChanged)
	Q_PROPERTY(bool inDetectAndWriteState READ inDetectAndWriteState WRITE setDetectAndWriteState NOTIFY inDetectAndWriteStateChanged)
	Q_PROPERTY(bool inNdefPushState READ inNdefPushState WRITE setNdefPushState NOTIFY inNdefPushStateChanged)
	Q_PROPERTY(bool inEventLogShowingState READ isEventLogShowing WRITE setEventLogShowing NOTIFY inEventLogShowingStateChanged)
	Q_PROPERTY(bool inTagEmulationState READ inTagEmulationState WRITE setTagEmulationState NOTIFY inTagEmulationStateChanged)

public:
	static StateManager* getInstance();
	bool inDefaultState() const;
	void setDefaultState();
	bool inReadState() const;
	void setReadState(bool read_state);
	bool inDetectAndWriteState() const;
	void setDetectAndWriteState(bool detect_and_write_state);
	bool inNoNfcState() const;
	void setNoNfcState();
	bool inNdefPushState() const;
	void setNdefPushState(bool ndef_push_state);
	bool isEventLogShowing() const;
	void setEventLogShowing(bool event_log_showing);
	bool inTagEmulationState() const;
	void setTagEmulationState(bool tag_emulation_state);

signals:
	void inReadStateChanged();
	void inDetectAndWriteStateChanged();
	void inNdefPushStateChanged();
	void inTagEmulationStateChanged();
	void inEventLogShowingStateChanged();

private:
	StateManager();
	static StateManager* _instance;
	bool m_inReadState;
	bool m_inDetectAndWriteState;
	bool m_inNdefPushState;
	bool m_event_log_is_showing;
	bool m_inTagEmulationState;
};

#endif /* STATEMANAGER_HPP_ */
