#include "MyFBApp.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

MyFBApp::MyFBApp()
{
    QmlDocument *qml = QmlDocument::create("main.qml");
    qml->setContextProperty("app", this);
    
    AbstractPane *root = qml->createRootNode<AbstractPane>();
    authDialog = root->findChild<CustomDialog*>("authDialog");
    status = root->findChild<TextArea*>("status");

    if (authDialog == NULL) {
    	qDebug() << "authPage init control failed";
    }

    Application::instance()->setScene(root);

    //SET FACEBOOK APP ID HERE
    QString myAppId = "";

    //Initialize manager and connect to its signals
    fbManager = new BB10FBManager(myAppId, authDialog);
    connect(fbManager, SIGNAL(onUserProfileRetrieved(User* )), this, SLOT(onUserProfileRetrieved(User*)));
    connect(fbManager, SIGNAL(onUserFriendsRetrieved(QList<User>*)), this, SLOT(onUserFriendsRetrieved(QList<User>*)));
    connect(fbManager, SIGNAL(onPublishPostSuccess()), this, SLOT(onPublishPostSuccess()));
    connect(fbManager, SIGNAL(onLoginSuccess()), this, SLOT(onLoginSuccess()));
    connect(fbManager, SIGNAL(onLogoutSuccess()), this, SLOT(onLogoutSuccess()));
    connect(fbManager, SIGNAL(onErrorEvent(int, QString)), this, SLOT(onErrorEvent(int, QString)));
}

void MyFBApp::login() {
	qDebug() << "Logging In...";
	fbManager->login();
}

void MyFBApp::logout() {
	qDebug() << "Logging Out...";
	fbManager->logout();
}
void MyFBApp::showUserProfile() {
	qDebug() << "Getting User Profile";
	fbManager->getUserProfile();
}

void MyFBApp::publishPost() {
	qDebug() << "Publishing to wall";
	if (_myself == 0) {
		fbManager->getUserProfile();
	}
	fbManager->publishPost(_myself->getId(), "Test Message");
	//fbManager->publishPostWithImage(_myself->getId(), "Test Image", "http://www.indraneelghosh.com/img/sport/sport_318_test-201.gif");
	//fbManager->publishPostWithLink(_myself->getId(), "Test Link", "https://developer.blackberry.com/cascades/", "Cascades Microsite", "Fun with Cascades!", "All About Cascades Development");
	//fbManager->publishPostWithVideo(_myself->getId(), "Test Video", "http://www.youtube.com/watch?v=bAKu0aVBHgQ");
}

void MyFBApp::showUserFriends() {
	qDebug() << "Calling getUserFriends()";
	fbManager->getUserFriends();
}

void MyFBApp::onUserProfileRetrieved(User* myself) {
	_myself = myself;
	QString result = "ID: " + myself->getId() +
					 "\nName: " + myself->getName() +
					 "\nFirst Name: " + myself->getFirstName() +
					 "\nLast Name: " + myself->getLastName() +
					 "\nLink: " + myself->getLink() +
					 "\nUser Name: " + myself->getUsername();

	status->setText(result);
	qDebug() << "User Profile Set";
}

void MyFBApp::onUserFriendsRetrieved(QList<User>* friends_list) {
	//Iterate through list of users, and collect name + username as output string

	QList<User>::iterator i;

	QString output;
	for (i = friends_list->begin(); i != friends_list->end(); ++i) {
		output += "Name: " + i->getName() + ", Username: "+i->getUsername() + "\n";
	}

	status->setText(output);
	qDebug() << "User Friends set";
}

void MyFBApp::onPublishPostSuccess() {
	status->setText("Post Published Successfully");
	qDebug() << "Post Successful";
}

void MyFBApp::onLoginSuccess() {
	status->setText("Logged In!");
	qDebug() << "Login Successful!";
}

void MyFBApp::onLogoutSuccess() {
	status->setText("Logged Out!");
	qDebug() << "Logout Successful!";
}

void MyFBApp::onErrorEvent(int error_type, QString error_msg) {
	switch (error_type) {
		case ERROR_LOGIN:
			qDebug() << "ERROR_LOGIN: " << error_msg;
			break;
		default:
			qDebug() << "ERROR_TYPE: " << error_type << ", ERROR_MSG: " << error_msg;
			break;
	}
}
