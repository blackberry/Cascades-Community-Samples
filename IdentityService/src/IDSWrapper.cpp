/**
 Copyright (c) 2013 BlackBerry Limited.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 **/
#include "IDSWrapper.hpp"
#include <QDebug>
#include <ids.h>
#include <bps/bps.h>
#include <fcntl.h>
#include <errno.h>

IDSWrapper::IDSWrapper(const char * name, QObject * owner) :
		QObject(owner), m_idsFd(-1) {
	// Register the required provider.
	ids_result_t result = ids_register_provider(name, &m_idsProvider, &m_idsFd);
	if (result == IDS_SUCCESS) {
		// Make sure BPS is 'listening' on that file for events.
		int bpsResult = bps_add_fd(m_idsFd, BPS_IO_INPUT, ids_io_handler, NULL);
		if (bpsResult != BPS_SUCCESS) {
			qDebug() << "Could not listen for data on the FD!";
		}
	} else {
		qDebug() << "Ouch! Could not register the provider! (this is bad...)";
	}
}

IDSWrapper::~IDSWrapper() {
	// how do we unregister? looks like there's no API for that... lets close the file just for fun.
	if (m_idsFd!=-1) {
		close(m_idsFd);
		m_idsFd = -1;
	}
}

int IDSWrapper::ids_io_handler(int fd, int, void *) {
	// process the message that's now available on that FD.
	if (ids_process_msg(fd) != IDS_SUCCESS) {
		return BPS_FAILURE;
	}
	return BPS_SUCCESS;
}

void IDSWrapper::failure(ids_request_id_t request_id, ids_result_t result,
		const char* info, void* cb_data) {
	if (cb_data!=NULL) {
		((IDSWrapper*)cb_data)->failure(request_id, result, info);
	} else {
		qDebug() << "Did we forget to set the callback data???";
	}
}

void IDSWrapper::failure(ids_request_id_t, ids_result_t result,
		const char * info) {
	emit error(info, result);
}

void IDSWrapper::failCheck(ids_result_t result, const char * message) {
	if (result == IDS_FAILURE) {
		emit error(message, errno);
	} else {
		qDebug() << "failCheck (pass)" << result << message;
	}
}

