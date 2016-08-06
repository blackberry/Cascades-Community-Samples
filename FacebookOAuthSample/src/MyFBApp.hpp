#ifndef MYFBAPP_H
#define MYFBAPP_H

#include <QObject>
#include <bb/cascades/controls/navigationpane.h>
#include <bb/cascades/controls/customdialog.h>
#include <bb/cascades/controls/textarea.h>

#include "BB10FBManager.hpp"

using namespace bb::cascades;

/*!
 * @brief Application GUI object
 */
class MyFBApp : public QObject
{
    Q_OBJECT
public:
    MyFBApp();

public slots:
	Q_INVOKABLE void login();
	Q_INVOKABLE void logout();
	Q_INVOKABLE void showUserProfile();
	Q_INVOKABLE void publishPost();
	Q_INVOKABLE void showUserFriends();

	void onUserProfileRetrieved(User*);
	void onUserFriendsRetrieved(QList<User>*);
	void onPublishPostSuccess();
	void onLoginSuccess();
	void onLogoutSuccess();
	void onErrorEvent(int, QString);

private:
    CustomDialog* authDialog;
    TextArea* status;
    BB10FBManager* fbManager;
    User* _myself;
};

#endif // ifndef MYFBAPP_H
