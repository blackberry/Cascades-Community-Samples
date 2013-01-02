/*
 * PjsipEngine.hpp
 *
 *  Created on: 24 Nov 2012
 *      Author: bcripps
 */

#ifndef PJSIPENGINE_HPP_
#define PJSIPENGINE_HPP_

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <pjsua-lib/pjsua.h>

using namespace bb::cascades;


class PjsipEngine : public QObject{
    Q_OBJECT
public:
	int start();
	void stop();
	PjsipEngine(Application& app);
	~PjsipEngine();
	void onRegStarted(pjsua_acc_id acc_id, pj_bool_t renew);
	void onTpStateChanged(pjsip_transport *tp, pjsip_transport_state state, const pjsip_transport_state_info *info);
private:
	static void on_reg_started(pjsua_acc_id acc_id, pj_bool_t renew);
	static void on_tp_state_changed(pjsip_transport *tp, pjsip_transport_state state, const pjsip_transport_state_info *info);
private:
};


#endif /* PJSIPENGINE_HPP_ */
