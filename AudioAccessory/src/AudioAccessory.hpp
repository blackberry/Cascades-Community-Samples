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
#ifndef AudioAccessory_HPP_
#define AudioAccessory_HPP_

#include <QObject>

#include "AudioService.hpp"

namespace bb { namespace cascades { class Application; }}
namespace bb { namespace cascades { class QmlDocument; }}
namespace bb { namespace cascades { class AbstractPane; }}

using namespace bb::cascades;

class AudioAccessory : public QObject
{
    Q_OBJECT

public:
    AudioAccessory(bb::cascades::Application *app);
    virtual ~AudioAccessory() {}

private:
    QmlDocument  *_qml;
    AbstractPane *_root;
    AudioService *_audioService;

signals:
	void message(const QVariant &);
};

#endif /* AudioAccessory_HPP_ */
