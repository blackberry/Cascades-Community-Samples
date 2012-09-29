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

#include "app.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/StackLayoutProperties>
#include <stdio.h>
#include <stdlib.h>

using namespace bb::cascades;

App* App::_instance;

App::~App() {
}

App::App(Application *app) {
	// Create and Load the QMLDocument.

	_state = GameState::getInstance();
	qDebug() << "XXXX GameState instance:" << _state;

	_app = app;
	_instance = this;

	qDebug() << "XXXX _app:" << _app;

	QmlDocument *qml = QmlDocument::create("asset:///main.qml");

	qDebug() << "XXXX QmlDocument:" << qml;

	if (!qml->hasErrors()) {

		setNumMoves(0);
		mNumTiles = 3;

		// Setting the context property we want to use from inside the QML. functions exposed
		// via Q_INVOKABLE will be found with the property and the name of the function.
		qml->setContextProperty("ttt", this);
		qDebug() << "XXXX setContextProperty done";

		// The application Page is created from QML.
		mAppPage = qml->createRootObject<Page>();
		qDebug() << "XXXX createRootObject done";
		findPlayAreaAndInitialize(mAppPage);

		if (mAppPage) {
			// Finally the main scene for the application is set the Page.
			_app->setScene(mAppPage);
		}
	}
}

App* App::getInstance() {
	return _instance;
}

void App::resetTiles() {
	// Resetting the tiles.
	setNumMoves(0);
	findPlayAreaAndInitialize(mAppPage);
}

bool App::hasWon(char symbol) {
	int s = symbol;
	qDebug() << "XXXX has this player won? symbol=" << s;

	for (int i=0;i<9;i++) {
		int state = _state->tileState(i);
	}

	// horizontal lines
	if (_state->tileState(0) == symbol && _state->tileState(1) == symbol && _state->tileState(2) == symbol) {
		return true;
	}
	if (_state->tileState(3) == symbol && _state->tileState(4) == symbol && _state->tileState(5) == symbol) {
		return true;
	}
	if (_state->tileState(6) == symbol && _state->tileState(7) == symbol && _state->tileState(8) == symbol) {
		return true;
	}

	// vertical lines
	if (_state->tileState(0) == symbol && _state->tileState(3) == symbol && _state->tileState(6) == symbol) {
		return true;
	}
	if (_state->tileState(1) == symbol && _state->tileState(4) == symbol && _state->tileState(7) == symbol) {
		return true;
	}
	if (_state->tileState(2) == symbol && _state->tileState(5) == symbol && _state->tileState(8) == symbol) {
		return true;
	}

	// diagonal lines
	if (_state->tileState(0) == symbol && _state->tileState(4) == symbol && _state->tileState(8) == symbol) {
		return true;
	}

	if (_state->tileState(2) == symbol && _state->tileState(4) == symbol && _state->tileState(6) == symbol) {
		return true;
	}

	return false;
}

void App::getWinningLine(char symbol, char line[]) {

	// horizontal lines
	if (_state->tileState(0) == symbol && _state->tileState(1) == symbol && _state->tileState(2) == symbol) {
		line[0] = 0;
		line[1] = 1;
		line[2] = 2;
	}
	if (_state->tileState(3) == symbol && _state->tileState(4) == symbol && _state->tileState(5) == symbol) {
		line[0] = 3;
		line[1] = 4;
		line[2] = 5;
	}
	if (_state->tileState(6) == symbol && _state->tileState(7) == symbol && _state->tileState(8) == symbol) {
		line[0] = 6;
		line[1] = 7;
		line[2] = 8;
	}

	// vertical lines
	if (_state->tileState(0) == symbol && _state->tileState(3) == symbol && _state->tileState(6) == symbol) {
		line[0] = 0;
		line[1] = 3;
		line[2] = 6;
	}
	if (_state->tileState(1) == symbol && _state->tileState(4) == symbol && _state->tileState(7) == symbol) {
		line[0] = 1;
		line[1] = 4;
		line[2] = 7;
	}
	if (_state->tileState(2) == symbol && _state->tileState(5) == symbol && _state->tileState(8) == symbol) {
		line[0] = 2;
		line[1] = 5;
		line[2] = 8;
	}

	// diagonal lines
	if (_state->tileState(0) == symbol && _state->tileState(4) == symbol && _state->tileState(8) == symbol) {
		line[0] = 0;
		line[1] = 4;
		line[2] = 8;
	}

	if (_state->tileState(2) == symbol && _state->tileState(4) == symbol && _state->tileState(6) == symbol) {
		line[0] = 2;
		line[1] = 4;
		line[2] = 6;
	}
}

bool App::isBoardFull() {
	for (int i = 0; i < 9; i++) {
		if (_state->tileState(i) == 0) {
			return false;
		}
	}
	return true;
}

void App::findPlayAreaAndInitialize(Page *appPage) {

	qDebug() << "XXXX findPlayAreaAndInitialize";

	_nfcManager = NfcManager::getInstance();

	_state->setGameState(0); // no current game

	if (_state->gameState() == 0) {
		_nfcManager->startEventProcessing();
	}

	_state->setLastSelection(-1); // -1 means no move yet made

	for (int i = 0; i < 9; i++) {
		_state->setTileState(i, 0);
	}

	// Find where we want to add our tiles.
	mPlayAreaContainer = appPage->findChild<Container*>("playArea");

	// Reset the play area by removing all the tile images.
	mPlayAreaContainer->removeAll();

	// As long as nothing is broken.
	if (mPlayAreaContainer) {

		// Calculate the size of the tiles.
		mWantedSize = mPlayAreaContainer->preferredWidth() / mNumTiles - 4;
		qDebug() << "XXXX mWantedSize:" << mWantedSize;

		// Iterate through all the tiles.
		for (int r = 0; r < mNumTiles; r++) {

			Container* columnContainer = new Container();

			columnContainer->setLayoutProperties(StackLayoutProperties::create().spaceQuota(.3));

			for (int c = 0; c < mNumTiles; c++) {

				// Create an ImageView with a blank tile image.
				ImageView *imageView = ImageView::create().bottomMargin(2).topMargin(2).leftMargin(2).rightMargin(2);
				imageView->setPreferredSize(mWantedSize, mWantedSize);
				QString imageSource = QString("asset:///images/blank.png");
				imageView->setImage(Image(imageSource));

				// And keep track of the ImageView.
				mPlayField[r][c] = imageView;
				mPlayField[r][c]->setObjectName(imageSource);

				// We are connecting all our tiles to the same slot, we can later identify them by sender().
				connect(imageView, SIGNAL(touch(bb::cascades::TouchEvent *)), this, SLOT(onTileTouch(bb::cascades::TouchEvent *)));

				columnContainer->add(imageView);
			}
			mPlayAreaContainer->add(columnContainer);
		}
	}

	QObject::connect(this, SIGNAL(message(QVariant)), this, SLOT(setUiMessage(QVariant)), Qt::QueuedConnection);

}

void App::onTileTouch(bb::cascades::TouchEvent *event) {

	//if it's not our turn, ignore

	if (_state->gameState() == 2) {
		emit message("It's not your turn!");
		return;
	}

	if (_state->gameState() == 0) {
		return;
	}

	// If we have a touch down on a tile.
	if (event->isDown()) {

		// We find who sent it...
		ImageView* senderImage = dynamic_cast<ImageView*>(sender());

		if (senderImage != NULL) {

			// If that container is real we iterate through all the tiles.
			for (int r = 0; r < mNumTiles; r++) {

				for (int c = 0; c < mNumTiles; c++) {

					int tile_inx = c * 3 + r;

					if (_state->tileState(tile_inx) != 0) {
						continue;
					}

					// We try and find our saved pointer to a tile.
					if (mPlayField[r][c] == senderImage) {

						qDebug() << "XXXX touched tile[" << r << "," << c << "]";

						// We're getting the name so we can check if it's turned.
						QVariant v = mPlayField[r][c]->objectName();

						if (v.canConvert<QString>()) {
							QString objName = v.value<QString>();

							if (!objName.contains("isSet", Qt::CaseInsensitive)) {

								// deselect previously selected tile this turn
								if (_state->lastSelection() != -1) {
									int row = _state->lastSelection() / 3;
									int col = _state->lastSelection() - (row * 3);
									mPlayField[col][row]->setObjectName("");
									mPlayField[col][row]->setImage(Image(QUrl("asset:///images/blank.png")));
								}

								mPlayField[r][c]->setObjectName("isSet");
								if (_state->mySymbol() == 1) {
									senderImage->setImage(Image(QUrl("asset:///images/nought.png")));
								} else {
									senderImage->setImage(Image(QUrl("asset:///images/cross.png")));
								}

								qDebug() << "XXXX checking game state";
								// temporarily reflect this selection in the tile_state array so we can check for a possible win etc
								// we don't normally commit values into this array until the user has indicated their turn is over by passing to the other
								// player so we'll undo this update if it's not a win or stale mate
								_state->setTileState(tile_inx,_state->mySymbol());
								if (hasWon(_state->mySymbol())) {
									qDebug() << "XXXX has won";
									char winning_line[] = {-1, -1, -1};
									getWinningLine(_state->mySymbol(),winning_line);
									_state->setGameState(0);
									mNumMoves++;
									emit numMovesChanged();
									emit message("YOU HAVE WON!!!");
									for (int i=0;i<3;i++) {
										int row = winning_line[i] / 3;
										int col = winning_line[i] - (row * 3);
										qDebug() << "XXXX winning line: row:" << row << ",col:" << col;
										mPlayField[col][row]->setOpacity(0.5);
									}
									return;
								}

								if (isBoardFull()) {
									_state->setGameState(0);
									emit message("STALE MATE");
									return;
								}

								// undo temporary state change
								_state->setTileState(tile_inx,0);

								_state->setLastSelection(tile_inx);

								qDebug() << "XXXX last selection set to " << tile_inx;

								_nfcManager->prepareToPassTurn();

							} else {
								mPlayField[r][c]->setObjectName("");
								senderImage->setImage(Image(QUrl("asset:///images/blank.png")));
								_state->setLastSelection(-1);
							}
						}
					}
				}
			}
		}
	}
}

void App::setNumMoves(const int i) {
	if (i != mNumMoves) {
		mNumMoves = i;
		emit numMovesChanged();
	}
}

int App::numMoves() {
	return mNumMoves;
}

QVariant App::uiMessage() {
	return _ui_message;
}

void App::setUiMessage(QVariant msg) {
	_ui_message = msg;
	emit uiMessageChanged();
}

void App::ourTurnNow() {
	int tc = _state->tileChanged();
	qDebug() << "XXXX it's our turn now. Other player set tile " << tc;
	setTile(_state->tileChanged(), _state->symbolChanged());
	_state->setLastSelection(-1);
}

void App::ourTurnDone() {
	qDebug() << "XXXX it's our turn done";
	mNumMoves++;
	emit numMovesChanged();
}

void App::setTile(int tile, int symbol) {

	int c = tile / 3;
	int r = tile - (c * 3);

	mPlayField[r][c]->setObjectName("isSet");
	if (symbol == 1) {
		mPlayField[r][c]->setImage(Image(QUrl("asset:///images/nought.png")));
	} else {
		mPlayField[r][c]->setImage(Image(QUrl("asset:///images/cross.png")));
	}

	_state->setTileState(tile, symbol); // occupied

}
