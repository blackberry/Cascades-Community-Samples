/*
 * Copyright (c) 2012 by BGmot <ey@tm-k.com>
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
#include <QtCore>
#include <QtGui>
#include <QtGui/QApplication>
#include <QFontDatabase>
#include <QTreeWidget>
#include <QPushButton>
#include <QFont>
#include "myedit.h"

QMyEdit::QMyEdit(QWidget *w) :
QTextEdit(w)
{

}
void QMyEdit::setNewFont( QTreeWidgetItem * current, QTreeWidgetItem * previous){
	// User has changed selection in our font tree
	// We want to show font sample only if a line with Font Size selected
	QString fontSize = current->text(0);
	QTreeWidgetItem *styleItem = current->parent();
	if (styleItem != NULL){
		QString fontStyle = styleItem->text(0);
		QTreeWidgetItem *familyItem = styleItem->parent();

		if (familyItem != NULL){
			QString fontFamily = familyItem->text(0);
			bool ok;
			QFont font((fontFamily), fontSize.toInt(&ok, 10));
			setText(fontFamily + QString(" ") + fontStyle + QString(" Size:") + fontSize);

			// Apply appropriate style to our text sample
			if (fontStyle.contains("Italic",Qt::CaseInsensitive)) {font.setStyle(QFont::StyleItalic);}
			if (fontStyle.contains("Normal",Qt::CaseInsensitive)) {font.setStyle(QFont::StyleNormal);}
			if (fontStyle.contains("Oblique",Qt::CaseInsensitive)) {font.setStyle(QFont::StyleOblique);}

			if (fontStyle.contains("Bold",Qt::CaseInsensitive)) {font.setWeight(QFont::Bold);}
			if (fontStyle.contains("Normal",Qt::CaseInsensitive)) {font.setWeight(QFont::Normal);}
			if (fontStyle.contains("Light",Qt::CaseInsensitive)) {font.setWeight(QFont::Light);}
			if (fontStyle.contains("DemiBold",Qt::CaseInsensitive)) {font.setWeight(QFont::DemiBold);}
			if (fontStyle.contains("Black",Qt::CaseInsensitive)) {font.setWeight(QFont::Black);}

			setFont(font);
		}
	}
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QFontDatabase database;

	QMainWindow *mainWindow = new QMainWindow();
	QRect r = QApplication::desktop()->screenGeometry(0);
	mainWindow->resize(r.width()+1, r.height()+1);

	QWidget centralWidget;
	mainWindow->setCentralWidget(&centralWidget);

	// Create Fonts Tree Widget
	QTreeWidget fontTree(&centralWidget);
	fontTree.setColumnCount(1);
	fontTree.setHeaderLabels(QStringList() << "Font");

	QStringList families = database.families(); // Acquire all available font families
	QString family;
	QString style;
	QString size;
	for (int i=0; i < families.size();i++ ) {
		family = families.at(i);
		QTreeWidgetItem *familyItem = new QTreeWidgetItem(&fontTree);
		familyItem->setText(0, family);
		familyItem->setExpanded(true);

		QStringList styles = database.styles(family); // Acquire all available styles for given families
		for (int j=0; j < styles.size(); j++){
			style = styles.at(j);
			QTreeWidgetItem *styleItem = new QTreeWidgetItem(familyItem);
			styleItem->setText(0, style);
			styleItem->setExpanded(false);

			QList<int> sizes = database.smoothSizes(family, style); // Acquire all available sizes for given family/style
			for (int k=0; k < sizes.size(); k++ ){
				size =QString::number(sizes.at(k));
				QTreeWidgetItem *sizeItem = new QTreeWidgetItem(styleItem);
				sizeItem->setText(0, size);
			}
		}
	}
	fontTree.setGeometry(10, 10, r.width()-20, r.height()-150);

	// Create a window where Font's samples will be shown
	QMyEdit textEdit(&centralWidget);
	textEdit.setGeometry(10, r.height()-140, r.width()-20, 130);
	textEdit.setText(QString("Select font"));
	textEdit.setReadOnly(true);

	// React if selection changes
	QObject::connect(&fontTree, SIGNAL(currentItemChanged (QTreeWidgetItem*, QTreeWidgetItem*)), &textEdit, SLOT(setNewFont(QTreeWidgetItem*, QTreeWidgetItem*)));

	mainWindow->show();
    return app.exec();

	return 0;
}
