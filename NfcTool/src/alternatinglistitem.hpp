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
#ifndef ALTERNATINGLISTITEM_H_
#define ALTERNATINGLISTITEM_H_

#include <QMutex>
#include <bb/cascades/CustomControl>
#include <bb/cascades/ListItemListener>

class AlternatingListItem: public bb::cascades::CustomControl {

	Q_OBJECT
	Q_PROPERTY( QString title READ title NOTIFY titleChanged )
	Q_PROPERTY( QString description READ description NOTIFY descriptionChanged )
	Q_PROPERTY( QString itemLabel READ itemLabel NOTIFY itemLabelChanged )
	Q_PROPERTY( int ordinal READ ordinal NOTIFY ordinalChanged )

public:
	AlternatingListItem(const QString& qmlFile);
	virtual ~AlternatingListItem();

	void setTitle(const QString& title);
	QString title() const;

	void setDescription(const QString& desc);
	QString description() const;

	void setItemLabel(const QString& itemLabel);
	QString itemLabel() const;

	void setOrdinal(const int ordinal);
	int ordinal() const;

signals:
	void ordinalChanged();
	void titleChanged();
	void descriptionChanged();
	void itemLabelChanged();

private:
	void setup(const QString& qmlFile);

private:
	int m_nextOrdinal;
	QString m_title;
	QString m_description;
	QString m_itemLabel;
};

#endif /* ALTERNATINGLISTITEM_H_ */
