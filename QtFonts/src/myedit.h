#ifndef MYEDIT_H
#define MYEDIT_H
#include <QtGui>
#include <QObject>
class QMyEdit : public QTextEdit{
	Q_OBJECT

public:
	QMyEdit(QWidget *w);

public slots:
	void setNewFont(QTreeWidgetItem*, QTreeWidgetItem*);
};
#endif
