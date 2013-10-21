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
#ifndef IDSWRAPPER_HPP_
#define IDSWRAPPER_HPP_

#include <QObject>
#include <ids.h>

/**
 * IDSWrapper provides common functionality for the BlackBerryID and BlackBerryProfile classes.
 */
class IDSWrapper: public QObject {
Q_OBJECT
protected:
	// Use this constructor to register an IDS provider.
	IDSWrapper(const char * name, QObject * owner = NULL);

public:
	virtual ~IDSWrapper();

signals:
	// Will be emitted when an error has occurred trying to use the IDS.
	void error(QString message, int error);

protected:
	// Call this for lazy checking of return values. ;)
	void failCheck(ids_result_t result, const char * message);

	// Provider instance and file descriptor that is handling
	// its data.
	ids_provider_t * m_idsProvider;
	int m_idsFd;

	// A request ID is used for tracking requests. This code
	// is so basic that the value is being ignored...
	ids_request_id_t m_requestId;

	// Static function which will take instance as last param,
	// simply calls the instance version.
	static void failure(ids_request_id_t, ids_result_t result,
			const char * info, void *);
	// Function called by IDS when a failure occurs.
	void failure(ids_request_id_t, ids_result_t, const char *);

private:
	// Static function for callback.
	static int ids_io_handler(int fd, int flags, void * data);
};

#endif /* IDSWRAPPER_HPP_ */
