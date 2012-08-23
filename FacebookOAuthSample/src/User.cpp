/*
 * User.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: alawong
 */

#include "User.hpp"

User::User() {
	// TODO Auto-generated constructor stub

}

User::~User() {
	// TODO Auto-generated destructor stub
}

User::User(QString id, QString name, QString first_name, QString last_name, QString link, QString username) {
	_id = id;
	_name = name;
	_first_name = first_name;
	_last_name = last_name;
	_link = link;
	_username = username;
}

