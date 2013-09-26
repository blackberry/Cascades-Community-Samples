// Default empty project template
#include <bb/cascades/Container>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Button>
#include <bb/cascades/Color>
#include <bb/cascades/Label>
#include <bb/cascades/SystemDefaults>

#include "MyLibrary.hpp"

using namespace bb::cascades;

namespace custom {

MyLibrary::MyLibrary(QObject *parent)
	: Page(parent)
{
	// TODO Auto-generated constructor stub
	Label * lbl = Label::create().textStyle(SystemDefaults::TextStyles::bigText())
		.vertical(VerticalAlignment::Center).horizontal(HorizontalAlignment::Center)
		.text("Hello from MyLibrary");

	Button * btn = Button::create().text("Test Library QML").onClicked(this, SLOT(onButtonClicked()));

	Container * c = Container::create().parent(this).background(Color::LightGray).add(lbl).add(btn);
	Page::setContent(c);

	QmlDocument *qml = QmlDocument::create("asset:///lib.MyLibrary/MyPage.qml").parent(this);

	qmlPage = qml->createRootObject<Page>();
}

MyLibrary::~MyLibrary() {
	// TODO Auto-generated destructor stub
}

void MyLibrary::onButtonClicked() {
	NavigationPane * navPane = qobject_cast<NavigationPane*>(parent());
	qDebug() << "onButtonClicked()" << navPane;
	if (navPane)
		navPane->push(qmlPage);
}

} /* end namespace custom */
