// Default empty project template
#include "Pjsipdemo.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include "PjsipEngine.hpp"

using namespace bb::cascades;

Pjsipdemo::Pjsipdemo(bb::cascades::Application *app) :
		QObject(app) {
	m_iPjsipEngine = new PjsipEngine(*app);
	m_iPjsipEngine->start();
}

Pjsipdemo::~Pjsipdemo(){
	if(m_iPjsipEngine != NULL){
		m_iPjsipEngine->stop();
		delete m_iPjsipEngine;
	}
}
