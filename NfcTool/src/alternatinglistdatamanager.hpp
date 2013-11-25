/* Copyright (c) 2013 BlackBerry Limited.
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
#ifndef ALTERNATINGLISTDATAMANAGER_H_
#define ALTERNATINGLISTDATAMANAGER_H_

#include <bb/cascades/controls/listitemprovider.h>
#include <bb/cascades/QmlDocument>

class AlternatingListDataManager: public bb::cascades::ListItemProvider {

	Q_OBJECT

public:
	AlternatingListDataManager(const QString& qmlFile);
	virtual ~AlternatingListDataManager();

	bb::cascades::VisualNode * createItem(bb::cascades::ListView* list,
			const QString &type);
	void updateItem(bb::cascades::ListView* list,
			bb::cascades::VisualNode *listItem, const QString &type,
			const QVariantList &indexPath, const QVariant &data);

private:
	QString m_qmlFile;
};

#endif /* ALTERNATINGLISTDATAMANAGER_H_ */
