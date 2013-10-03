/* Copyright (c) 2013 Research In Motion Limited.
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

#include "SessionListing.hpp"

SessionListing::SessionListing(QObject *parent) :
        QObject(parent), _model(new bb::cascades::GroupDataModel(QStringList() << "session_id", this)) {
    _model->setSortingKeys(QStringList() << "session_id");
    _model->setSortedAscending(false);
    _model->setGrouping(bb::cascades::ItemGrouping::None);
}

bb::cascades::DataModel* SessionListing::model() {
    qDebug() << "YYYY SessionListing::model() - returning model";
    return _model;
}
