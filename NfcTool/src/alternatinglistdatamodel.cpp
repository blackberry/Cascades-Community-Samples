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
#include <bb/cascades/ItemGrouping>
#include <QStringList>

using namespace bb::cascades;

AlternatingListDataModel::AlternatingListDataModel() :
		GroupDataModel(), m_nextOrdinal(1) {
	setGrouping(ItemGrouping::None);
	QStringList sortKeys;
	sortKeys << "ordinal";
	setSortingKeys(sortKeys);
	setSortedAscending(false);
}

QVariant AlternatingListDataModel::data(const QVariantList &indexPath) {
	return GroupDataModel::data(indexPath);
}

void AlternatingListDataModel::append(const QString& title,
		const QString& itemLabel, const QString& desc) {
	QVariantMap map;

	map["ordinal"] = m_nextOrdinal++;
	map["itemLabel"] = itemLabel;
	map["title"] = title;
	map["description"] = desc;
	this->insert(map);
}

void AlternatingListDataModel::reset() {
	m_nextOrdinal = 1;
	this->clear();
}

void AlternatingListDataModel::ascendingSortOrder(bool ascending) {
	this->setSortedAscending(ascending);
}

void AlternatingListDataModel::dumpModel() {
	for (QVariantList indexPath = first(); !indexPath.isEmpty(); indexPath =
			after(indexPath)) {
		QVariant item = data(indexPath);
		QVariantMap bar = item.toMap();
	}
}
