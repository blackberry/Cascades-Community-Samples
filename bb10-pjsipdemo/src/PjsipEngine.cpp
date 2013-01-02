/*
 * PjsipEngine.cpp
 *
 *  Created on: 24 Nov 2012
 *      Author: Robert Cripps
 *  Demonstration of the basics of PJSIP integration into cascades
 *  'C' logic from the PJSIP project samples
 *  The PJSIP project samples demonstrate how to make calls and configure all callbacks.
 *
 */

#include "PjsipEngine.hpp"
#include <string>

std::string sipDomain = "someserver.com";
std::string sipUser = "sipuser";
std::string sipPassword = "sippassword";
std::string atSymbol = "@";
std::string sipPrefix = "sip:";
std::string digest = "digest";

// For static class method callbacks to access the class instance methods
static PjsipEngine* g_This = NULL;

PjsipEngine::PjsipEngine(Application& app) {
	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
	qml->setContextProperty("pjsipEngine", this);

	// create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();
	// set created root object as a scene
	app.setScene(root);
	g_This = this;
}

PjsipEngine::~PjsipEngine() {
}

/* Display error and exit application */
static void handleError(const char *title, pj_status_t status) {
	qDebug() << title << "code = " << status;
	pjsua_destroy();
}

int PjsipEngine::start() {
	pjsua_acc_id acc_id;
	pj_status_t status;
	status = pjsua_create();

	if (status != PJ_SUCCESS) {
		handleError("Error creating pjsua", status);
		return status;
	}

	{
		pjsua_config cfg;
		pjsua_logging_config log_cfg;

		pjsua_config_default(&cfg);

		// Set up the two static callbacks for session up session down
		// This is where all callbacks are initialized. See PJSIP documentation
		// session up
		cfg.cb.on_reg_started = &on_reg_started;
		// traps session down
		cfg.cb.on_transport_state = &on_tp_state_changed;

		pjsua_logging_config_default(&log_cfg);
		log_cfg.console_level = 4;

		pjsua_media_config media_cfg;

		pjsua_media_config_default(&media_cfg);
		status = pjsua_init(&cfg, &log_cfg, &media_cfg);

		if (status != PJ_SUCCESS) {
			handleError("Error initializing pjsua", status);
			return status;
		}

	}

	/* Add UDP transport. */
	{
		pjsua_transport_config cfg;

		pjsua_transport_config_default(&cfg);
		cfg.port = 5060;
		status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
		if (status != PJ_SUCCESS) {
			handleError("Error creating transport", status);
			return status;
		}
	}

	/* Initialization is done, now start pjsua */
	status = pjsua_start();
	if (status != PJ_SUCCESS) {
		handleError("Error starting pjsua", status);
		return status;
	}

	/* Register to SIP server by creating SIP account. */
	{
		pjsua_acc_config cfg;

		pjsua_acc_config_default(&cfg);

		std::string id(sipPrefix + sipUser + atSymbol + sipDomain);
		cfg.id = pj_str((char *) id.c_str());
		std::string uri(sipPrefix + sipDomain);
		cfg.reg_uri = pj_str((char*) uri.c_str());
		cfg.cred_count = 1;
		cfg.cred_info[0].realm = pj_str((char *) sipDomain.c_str());
		cfg.cred_info[0].scheme = pj_str((char*) digest.c_str());
		cfg.cred_info[0].username = pj_str((char*) sipUser.c_str());
		cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
		cfg.cred_info[0].data = pj_str((char *) sipPassword.c_str());

		status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
		if (status != PJ_SUCCESS) {
			handleError("Error adding account", status);
			return status;
		}
	}

	return status;
}

void PjsipEngine::stop() {
	pjsua_destroy();
}

void PjsipEngine::on_reg_started(pjsua_acc_id acc_id, pj_bool_t renew){
	g_This->onRegStarted(acc_id,renew);
}

void PjsipEngine::on_tp_state_changed(pjsip_transport *tp, pjsip_transport_state state, const pjsip_transport_state_info *info){
	g_This->onTpStateChanged(tp,state,info);
}

void PjsipEngine::onRegStarted(pjsua_acc_id acc_id, pj_bool_t renew){
	qDebug() << "PjsipEngine::onRegStarted()";
}
void PjsipEngine::onTpStateChanged(pjsip_transport *tp, pjsip_transport_state state, const pjsip_transport_state_info *info){
	qDebug() << "PjsipEngine::onTpStateChanged() = " << state;
	if (state == PJSIP_TP_STATE_DISCONNECTED) {
		qDebug() << "PjsipEngine::onTpStateChanged() Session Down";
	}
}
