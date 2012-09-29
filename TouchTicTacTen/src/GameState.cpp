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
#include "GameState.hpp"
#include <qdebug>

GameState* GameState::_instance;

GameState::GameState() {
	qDebug() << "XXXX Constructing GameState";
	qDebug() << "XXXX Done constructing GameState";
}

GameState* GameState::getInstance() {
	if (_instance == 0) {
		_instance = new GameState;
	}
	return _instance;
}

char GameState::gameState() {
	return _game_state;
}

void GameState::setGameState(char game_state) {
	_game_state = game_state;
}

char GameState::mySymbol() {
	return _my_symbol;
}

void GameState::setMySymbol(char my_symbol) {
	_my_symbol = my_symbol;
}

char GameState::playerNumber() {
	return _player_number;
}

void GameState::setPlayerNumber(char player_number) {
	_player_number = player_number;
}

char GameState::lastSelection() {
	return _last_selection;
}

void GameState::setLastSelection(char selection) {
	_last_selection = selection;
}

int GameState::tileState(char tile) {
	return _tile_state[tile];
}

void GameState::setTileState(char tile, int state) {
	_tile_state[tile] = state;
	int t = tile;
}

int GameState::myBid() {
	return _my_bid;
}

void GameState::setMyBid(int my_bid) {
	_my_bid = my_bid;
}

int GameState::theirBid() {
	return _their_bid;
}

void GameState::setTheirBid(int their_bid) {
	_their_bid = their_bid;
}

char GameState::tileChanged() {
	return _tile_changed;
}

void GameState::setTileChanged(char tile) {
	_tile_changed = tile;
}

char GameState::symbolChanged() {
	return _symbol_changed;
}

void GameState::setSymbolChanged(char symbol) {
	_symbol_changed = symbol;
}
