/* Copyright (c) 2013 BlackBerry.
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
#include "KeepAliveThread.h"
#include "BestCam.hpp"

extern BestCam * gMainBestCam;
extern int gMove;

KeepAliveThread::KeepAliveThread(QObject *parent) :
        QThread(parent)
{

}

void KeepAliveThread::run()
{
    //writing an A to keep alive, should reieve a H from the arduino-code

    while (true) {
        char buffer[80] = "A";
        write(m_sppFD, buffer, strlen(buffer));
        sleep(5);
    }

}

void KeepAliveThread::init(int fd)
{
    printf("KeepAliveThread::init()\n");
    fflush(stdout);
    m_sppFD = fd;
}
