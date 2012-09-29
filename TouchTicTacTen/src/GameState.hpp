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
#ifndef GAMESTATE_HPP_
#define GAMESTATE_HPP_

class GameState {

public:
	static GameState* getInstance();
	char gameState();
	void setGameState(char game);
	char mySymbol();
	void setMySymbol(char my_symbol);
	char playerNumber();
	void setPlayerNumber(char player_number);
	char lastSelection();
	void setLastSelection(char selection);
	int tileState(char tile);
	void setTileState(char tile, int state);
	int myBid();
	void setMyBid(int my_bid);
	int theirBid();
	void setTheirBid(int their_bid);
	char tileChanged();
	void setTileChanged(char tile);
	char symbolChanged();
	void setSymbolChanged(char symbol);

private:
	GameState();
	static GameState* _instance;
	char _last_selection;
	// 0 : available,  1 : contains nought, 2 : contains cross
	char _tile_state[9];
	// 0 : no current game, 1 : my turn, 2 : your turn
	char _game_state;
	// 1 : nought, 2 : cross
	char _my_symbol;
	// player 1 is always noughts, player 2 is crosses
	char _player_number;
	int _my_bid;
	int _their_bid;
	char _tile_changed;
	char _symbol_changed;

};

#endif /* GAMESTATE_HPP_ */
