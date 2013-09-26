// Default empty project template
#ifndef MYLIBRARY_HPP_
#define MYLIBRARY_HPP_

#include <bb/cascades/Page>
#include <QObject>

namespace custom {

class Q_DECL_EXPORT MyLibrary : public bb::cascades::Page {
	Q_OBJECT
public:
	MyLibrary(QObject * parent = NULL);
	virtual ~MyLibrary();

	Q_SLOT void onButtonClicked();

private:
	bb::cascades::Page *qmlPage;
};

} /* end namespace custom */

#endif /* MYLIBRARY_HPP_ */
