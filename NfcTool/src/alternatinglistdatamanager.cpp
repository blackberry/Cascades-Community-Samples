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
#include "alternatinglistdatamodel.hpp"
#include "alternatinglistdatamanager.hpp"
#include "alternatinglistitem.hpp"
#include <bb/cascades/Header>
#include <bb/cascades/StandardListItem>

using namespace bb::cascades;

AlternatingListDataManager::AlternatingListDataManager(const QString& qmlFile) :
		m_qmlFile(qmlFile) {
}

AlternatingListDataManager::~AlternatingListDataManager() {
}

bb::cascades::VisualNode* AlternatingListDataManager::createItem(
		bb::cascades::ListView* list, const QString &type) {
	Q_UNUSED(list);

	if (type.compare(GroupDataModel::Header) == 0)
		return new Header();
	else
		return new AlternatingListItem(m_qmlFile);
}

void AlternatingListDataManager::updateItem(bb::cascades::ListView* list,
		bb::cascades::VisualNode *listItem, const QString &type,
		const QVariantList &indexPath, const QVariant &data) {
	Q_UNUSED(list);
	Q_UNUSED(type);
	Q_UNUSED(indexPath);

	QVariantMap map = data.value<QVariantMap>();

	const int ordinal = map["ordinal"].value<int>();
	const QString itemLabel = map["itemLabel"].value<QString>();
	const QString title = map["title"].value<QString>();
	const QString description = map["description"].value<QString>();

	if (!((itemLabel.isEmpty() or itemLabel.isNull())
			and (title.isEmpty() or title.isNull())
			and (description.isEmpty() or description.isNull()))) {

		AlternatingListItem* item = dynamic_cast<AlternatingListItem*>(listItem);

		item->setOrdinal(ordinal);
		item->setItemLabel(itemLabel);
		item->setTitle(title);
		item->setDescription(description);

	} else {
		qDebug() << "XXXX Empty entries so not adding to list";
	}
}
