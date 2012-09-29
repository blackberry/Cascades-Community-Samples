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
#include "alternatinglistitem.hpp"
#include <bb/cascades/QmlDocument>

using namespace bb::cascades;

AlternatingListItem::AlternatingListItem(const QString& qmlFile) :
		CustomControl() {
	setup(qmlFile);
}

AlternatingListItem::~AlternatingListItem() {
}

void AlternatingListItem::setup(const QString& qmlFile) {
	QmlDocument* document = QmlDocument::create(qmlFile);
	QDeclarativeContext *derivedContext = new QDeclarativeContext(
			document->documentContext(), this);
	derivedContext->setContextProperty("_item", this);

	setRoot(document->createRootObject<Control>(derivedContext));
}

void AlternatingListItem::setTitle(const QString& title) {
	if (title != m_title) {
		m_title = title;
		emit titleChanged();
	}
}

QString AlternatingListItem::title() const {
	return m_title;
}

void AlternatingListItem::setDescription(const QString& desc) {
	if (desc != m_description) {
		m_description = desc;
		emit descriptionChanged();
	}
}

QString AlternatingListItem::description() const {
	return m_description;
}

void AlternatingListItem::setItemLabel(const QString& itemLabel) {
	if (itemLabel != m_itemLabel) {
		m_itemLabel = itemLabel;
		emit itemLabelChanged();
	}
}

int AlternatingListItem::ordinal() const {
	return m_nextOrdinal;
}

void AlternatingListItem::setOrdinal(const int ordinal) {
	if (ordinal != m_nextOrdinal) {
		m_nextOrdinal = ordinal;
		emit ordinalChanged();
	}
}

QString AlternatingListItem::itemLabel() const {
	return m_itemLabel;
}

