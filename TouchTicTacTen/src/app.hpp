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
#ifndef APP_H
#define APP_H

#include <QObject>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/TouchEvent>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/ImageView>
#include "NfcManager.hpp"
#include "GameState.hpp"

class App : public QObject {

Q_OBJECT

public:
    static App* getInstance();
	App(bb::cascades::Application *app);
	virtual ~App();

	/**
	 * Invokable function used for resetting the tile layout from QML.
	 */
	Q_INVOKABLE
	void resetTiles();

	Q_PROPERTY(int numMoves READ numMoves WRITE setNumMoves NOTIFY numMovesChanged)
	Q_PROPERTY(QVariant message READ uiMessage WRITE setUiMessage NOTIFY uiMessageChanged)

	void setNumMoves(const int i);
	int numMoves();
	QVariant uiMessage();

private slots:

	void onTileTouch(bb::cascades::TouchEvent *event);

public slots:
	void setUiMessage(QVariant msg);
	void ourTurnNow();
	void ourTurnDone();

signals:

	void numMovesChanged();
	void uiMessageChanged();
	void message(const QVariant &message);

private:

	static App* _instance;

	void findPlayAreaAndInitialize(bb::cascades::Page *appPage);
	void checkGameState();
	void setTile(int tile, int symbol);
	bool isBoardFull();
	void getWinningLine(char symbol, char line[]);
	bool hasWon(char symbol);

	bb::cascades::Application *_app;

	// Some Controls that we need to remember.
	bb::cascades::Container *mPlayAreaContainer;
	bb::cascades::ImageView* mPlayField[3][3];
	bb::cascades::Page *mAppPage;

	GameState* _state;
	NfcManager* _nfcManager;
	int mNumMoves;
	QVariant _ui_message;
	float mWantedSize;
	float mNumTiles;
};

#endif // ifndef APP_H
