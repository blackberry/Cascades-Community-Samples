/*
 * Copyright (c) 2014 BlackBerry Limited.
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

#include "applicationui.hpp"
#include "common.hpp"
#include "Utilities.hpp"
#include "Settings.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/DropDown>
#include <bb/cascades/Sheet>
#include <bb/system/SystemDialog>
#include <QtNetwork/QLocalSocket>

#include <QTimer>

using namespace bb::cascades;

static AbstractPane *_root = 0;

ApplicationUI::ApplicationUI(bb::cascades::Application *app)
	: QObject(app)
	, _invokeManager(new bb::system::InvokeManager(this))
	, _inbox_is_empty(true)
    , _modelImpl(ModelImpl::getInstance(this))
    , _accountList(bb::pim::account::AccountService().accounts(bb::pim::account::Service::Messages))
{
    qDebug() << "GGGG ApplicationUI constructor - starting";

    _iModel = _modelImpl;

    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(zeroDaysWarning()), this, SLOT(onZeroDaysWarning()))) {
        qWarning() << "GGGG zeroDaysWarning - connect failed " << strerror(errno) << endl;
    }
    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(updateReceived()), this, SLOT(onUpdateReceived()))) {
        qWarning() << "GGGG updateReceived - connect failed " << strerror(errno) << endl;
    }
    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(suggestShowRequestsTab()), this, SLOT(onSuggestShowRequestsTab()))) {
        qWarning() << "GGGG suggestShowRequestsTab - connect failed " << strerror(errno) << endl;
    }
    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(suggestShowInboxTab()), this, SLOT(onSuggestShowInboxTab()))) {
        qWarning() << "GGGG suggestShowInboxTab - connect failed " << strerror(errno) << endl;
    }
    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(syncComplete()), this, SLOT(onSyncComplete()))) {
        qWarning() << "GGGG syncComplete - connect failed " << strerror(errno) << endl;
    }
    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(operationNotSupported()), this, SLOT(onOperationNotSupported()))) {
        qWarning() << "GGGG operationNotSupported - connect failed " << strerror(errno) << endl;
    }
    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(adapterNotConfigured()), this, SLOT(onAdapterNotConfigured()))) {
        qWarning() << "GGGG adapterNotConfigured - connect failed " << strerror(errno) << endl;
    }
    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(inboxEmptyChanged()), this, SIGNAL(signalInboxEmptyChanged()))) {
        qWarning() << "GGGG inboxEmptyChanged - connect failed " << strerror(errno) << endl;
    }

    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(adapterNameChanged()), this, SIGNAL(signalAdapterNameChanged()))) {
        qWarning() << "GGGG adapterNameChanged - connect failed " << strerror(errno) << endl;
    }

    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(adapterVersionChanged()), this, SIGNAL(signalAdapterVersionChanged()))) {
        qWarning() << "GGGG adapterVersionChanged - connect failed " << strerror(errno) << endl;
    }

    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(adapterDescriptionChanged()), this, SIGNAL(signalAdapterDescriptionChanged()))) {
        qWarning() << "GGGG adapterDescriptionChanged - connect failed " << strerror(errno) << endl;
    }

    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(adapterConfiguredChanged()), this, SIGNAL(signalAdapterConfiguredChanged()))) {
        qWarning() << "GGGG adapterConfiguredChanged - connect failed " << strerror(errno) << endl;
    }

    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(totalEntitlementChanged()), this, SIGNAL(signalTotalEntitlementChanged()))) {
        qWarning() << "GGGG totalEntitlementChanged - connect failed " << strerror(errno) << endl;
    }

    if (!QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(currentEntitlementChanged()), this, SIGNAL(signalCurrentEntitlementChanged()))) {
        qWarning() << "GGGG currentEntitlementChanged - connect failed " << strerror(errno) << endl;
    }

    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));

    Settings* settings = Settings::getInstance(this);
    if (!QObject::connect(settings, SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()))) {
        qWarning() << "GGGG settingsChanged - connect failed " << strerror(errno) << endl;
    }

    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    QDeclarativeContext *root_context;
    root_context = qml->defaultDeclarativeEngine()->rootContext();
    root_context->setContextProperty("app", this);

    // Create root object for the UI
    _root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    app->setScene(_root);

    int conn_res=0;
    conn_res = QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(signalRequestsLoaded(const QVariantList &)), this, SLOT(loadRequests(const QVariantList &)));
    conn_res = QObject::connect(dynamic_cast<QObject *>(_iModel), SIGNAL(signalTasksLoaded(const QVariantList &)), this, SLOT(loadTasks(const QVariantList &)));
    conn_res = QObject::connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(onInvoked(const bb::system::InvokeRequest&)));

    populateEmailSettingControls();
    checkAdapterSettings();
    refreshEntitlement(QDate::currentDate().year());
    processInboundQueue();

}

ApplicationUI::~ApplicationUI() {
}

void ApplicationUI::onInvoked(const bb::system::InvokeRequest &request)
{

    qDebug() << "GGGG entering ApplicationUI::onInvoked(const InvokeRequest &request): " << request.action() << endl;

    if (request.action().compare(TARGET_AL_ACTION_INBOUND_QUEUE) == 0) {
        qDebug() << "GGGG TARGET_AL_ACTION_INBOUND_QUEUE action" << endl;
        processInboundQueue();
    } else {
        qDebug() << "GGGG not doing anything special with requested action" << endl;
    }

}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("VacationRequestPrototype_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

void ApplicationUI::loadRequests(const QVariantList& data)
{
    qDebug() << "GGGG loadRequests in ApplicationUI:" << data;
    Q_UNUSED(data);
    QMetaObject::invokeMethod(_root, "loadRequests", Q_ARG(QVariant, QVariant(data)));
}

void ApplicationUI::loadTasks(const QVariantList& data)
{
    qDebug() << "GGGG loadTasks in ApplicationUI:" << data;
    Q_UNUSED(data);
    QMetaObject::invokeMethod(_root, "loadTasks", Q_ARG(QVariant, QVariant(data)));
}

bool ApplicationUI::isValidDateRange(QDateTime from_date, QDateTime to_date, QString range_name)
{
    return _modelImpl->isValidDateRange(from_date, to_date, range_name);
}

void ApplicationUI::submitBookingRequest(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, QVariant day_half, QString note)
{
    qDebug() << "GGGG ApplicationUI::submitBookingRequest: from_date=" << from_date.toString() << " to_date=" << to_date.toString() << " first_day_half=" << first_day_half << " last_day_half="
            << last_day_half << " day_half=" << day_half;

    // validation
    if (to_date < from_date) {
        Utilities::alert("Error: 'To Date' must not be before 'From Date'");
        return;
    }
    if (to_date.date().year() != from_date.date().year()) {
        Utilities::alert("Error: 'From Date' and 'To Date' must be in the same year");
        return;
    }
    int leave_year = to_date.date().year();

    _modelImpl->submitBookingRequest(leave_year, from_date, to_date, first_day_half, last_day_half, day_half, note);

    qDebug() << "GGGG done submitting request";
    showRequests(leave_year);
}

void ApplicationUI::clientInitiatedSync(int leave_year) {
    qDebug() << "GGGG clientInitiatedSync: " << leave_year << endl;
    _modelImpl->clientInitiatedSync(leave_year);
    Utilities::toast(QString("Synchronization for %1 requested").arg(leave_year));
}

void ApplicationUI::cancelBookingRequest(int local_request_id, int current_status)
{
    qDebug() << "GGGG ApplicationUI::cancelBookingRequest(" << local_request_id << "," << current_status + ")";

    _local_request_id = local_request_id;
    _current_status = current_status;

    // confirm

    bb::system::SystemDialog *dial = new bb::system::SystemDialog("Confirm", "Cancel", this);
    dial->setTitle("Confirm Action");

    if (current_status == STATUS_BOOKING_REJECTED || current_status == STATUS_FAILED || current_status == STATUS_CANCELLATION_OK) {
        dial->setBody("Please confirm deletion request");
    } else {
        if (current_status == STATUS_CANCELLATION_PENDING || current_status == STATUS_CANCELLATION_SUBMITTED) {
            dial->setBody("Item is already being cancelled - Please confirm deletion");
        } else {
            dial->setBody("Please confirm cancellation request");
        }
    }
    dial->setDismissAutomatically(true);
    bool success = connect(dial, SIGNAL(finished(bb::system::SystemUiResult::Type)), this, SLOT(onCancelDialogFinished(bb::system::SystemUiResult::Type)));

    if (success) {
        dial->show();
    } else {
        dial->deleteLater();
        Utilities::alert("Error relating to system dialog");
        return;
    }

}

void ApplicationUI::deleteApprovalTask(int task_id)
{
    qDebug() << "GGGG ApplicationUI::deleteApprovalTask(" << task_id + ")";

    _task_id = task_id;

    // confirm

    bb::system::SystemDialog *dial = new bb::system::SystemDialog("Confirm", "Cancel", this);
    dial->setTitle("Confirm Action");
    dial->setBody("Please confirm deletion request");
    dial->setDismissAutomatically(true);
    bool success = connect(dial, SIGNAL(finished(bb::system::SystemUiResult::Type)), this, SLOT(onDeleteTaskDialogFinished(bb::system::SystemUiResult::Type)));

    if (success) {
        dial->show();
    } else {
        dial->deleteLater();
        Utilities::alert("Error relating to system dialog");
        return;
    }
}

void ApplicationUI::updateBookingRequest(int local_request_id, int current_leave_year, QDate current_from_date, QDateTime updating_from_date, QDate current_to_date, QDateTime updating_to_date,
        int current_first_day_half, bool updating_first_day_half, int current_last_day_half, bool updating_last_day_half, int current_day_half, QVariant updating_day_half, QString note)
{

    qDebug() << "GGGG ApplicationUI::updateBookingRequest: local_request_id=" << local_request_id << " current_leave_year=" << current_leave_year << " current_from_date=" << current_from_date
            << " updating_from_date=" << updating_from_date << " current_to_date=" << current_to_date << " updating_to_date=" << updating_to_date;
    qDebug() << "GGGG ApplicationUI::updateBookingRequest:  " << " current_first_day_half=" << current_first_day_half << "updating_first_day_half=" << updating_first_day_half;
    qDebug() << "GGGG ApplicationUI::updateBookingRequest:  " << " current_last_day_half=" << current_last_day_half << " updating_last_day_half=" << updating_last_day_half;
    qDebug() << "GGGG ApplicationUI::updateBookingRequest:  " << " current_day_half=" << current_day_half << " updating_day_half=" << updating_day_half << " note=" << note;

    // validation

    if (updating_to_date < updating_from_date) {
        Utilities::alert("Error: 'To Date' must not be before 'From Date'");
        return;
    }

    if (updating_to_date.date().year() != updating_from_date.date().year()) {
        Utilities::alert("Error: 'From Date' and 'To Date' must be in the same year");
        return;
    }

    _modelImpl->updateBookingRequest(local_request_id, current_leave_year, current_from_date, updating_from_date, current_to_date, updating_to_date,
            current_first_day_half, updating_first_day_half, current_last_day_half, updating_last_day_half, current_day_half,  updating_day_half, note);

    qDebug() << "GGGG done submitting request";

    showTab(TAB_REQUESTS);
}

void ApplicationUI::onCancelDialogFinished(bb::system::SystemUiResult::Type type)
{
    qDebug() << QString("GGGG onCancelDialogFinished");
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        _modelImpl->cancelBookingRequest(_current_status, _local_request_id);
    }
    showTab(TAB_REQUESTS);

}

void ApplicationUI::onDeleteTaskDialogFinished(bb::system::SystemUiResult::Type type)
{
    qDebug() << QString("GGGG onDeleteTaskDialogFinished");
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        _iModel->deleteApprovalTask(_task_id);
        showTab(TAB_INBOX);
        if (isInboxEmpty()) {
            emit signalInboxEmptyChanged();
        }
    }
}

void ApplicationUI::processInboundQueue()
{
    _modelImpl->processInboundQueue();
}

void ApplicationUI::refreshEntitlement(int leave_year)
{
    _iModel->refreshEntitlement(leave_year);
}

void ApplicationUI::requestAdapterStatus() {
    _iModel->requestAdapterStatus();
}

void ApplicationUI::showTab(int tab_index)
{
    switch (tab_index) {
        case TAB_REQUESTS:
            QMetaObject::invokeMethod(_root, "refreshRequests");
            break;
        case TAB_INBOX:
            QMetaObject::invokeMethod(_root, "refreshTasks");
            break;
    }
    QMetaObject::invokeMethod(_root, "onShowTab", Q_ARG(QVariant, tab_index));
}

void ApplicationUI::showRequests(int year)
{
    QMetaObject::invokeMethod(_root, "refreshRequestsForYear", Q_ARG(QVariant, year));
    QMetaObject::invokeMethod(_root, "onShowTab", Q_ARG(QVariant, TAB_REQUESTS));
}

void ApplicationUI::addCalendarEntry(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, int day_half, QString note)
{
    _iModel->addCalendarEntry(from_date, to_date, first_day_half, last_day_half, day_half, note);
}

int ApplicationUI::totalEntitlement()
{
    return _iModel->totalEntitlement();
}

int ApplicationUI::currentEntitlement()
{
    return _iModel->currentEntitlement();
}

void ApplicationUI::clearProcessingStatus(int local_request_id, int leave_year)
{
    _iModel->clearProcessingStatus(local_request_id, leave_year);
}

QString ApplicationUI::toLocalDateFormat(QString sqlite_format_date_string)
{
    return Utilities::toLocalDateFormat(sqlite_format_date_string);
}

QString ApplicationUI::statusText(int booking_status, int last_op_status)
{
    return Utilities::statusText(booking_status, last_op_status);
}

QString ApplicationUI::lastOpDesc(int last_op_status)
{
    return Utilities::statusName(last_op_status);
}

QString ApplicationUI::statusIcon(int booking_status, int last_op_status)
{
    return Utilities::statusIcon(booking_status, last_op_status);
}

QString ApplicationUI::taskText(int task_status, int decision, QString user_id, int half_days, QString from_date)
{
    return Utilities::taskText(task_status, decision, user_id, half_days, toLocalDateFormat(from_date));
}

QString ApplicationUI::taskIcon(int task_type, int task_status, int decision)
{
    return Utilities::taskIcon(task_type, task_status, decision);
}

QString ApplicationUI::taskStatus(int task_status, int decision)
{
    return Utilities::taskStatus(task_status, decision);
}

QString ApplicationUI::taskDecision(int decision)
{
    return Utilities::taskDecision(decision);
}

bool ApplicationUI::sameDates(QDateTime datetime1, QDateTime datetime2)
{
    return Utilities::sameDates(datetime1, datetime2);

}

QString ApplicationUI::requestParamsInterpretation(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, QVariant day_half)
{
    return _iModel->requestParamsInterpretation(from_date, to_date, first_day_half, last_day_half, day_half);
}

QString ApplicationUI::approvalParamsInterpretation(QString from_date, QString to_date, int first_day_half, int last_day_half, int day_half)
{
    return _iModel->approvalParamsInterpretation(from_date, to_date, first_day_half, last_day_half, day_half);
}

QString ApplicationUI::taskType(int task_status)
{
    switch (task_status) {
        case TASK_TYPE_BOOKING_APPROVAL:
            return "Booking";
            break;
        case TASK_TYPE_CANCEL_APPROVAL:
            return "Cancellation";
            break;
        case TASK_TYPE_UPDATE_APPROVAL:
            return "Update";
            break;
        default:
            return "UNKNOWN TASK TYPE";
    }
}

QString ApplicationUI::datesText(QString from_date, QString to_date, int first_day_half, int last_day_half, int day_half)
{

    qDebug() << "GGGG datesText: from_date=" << from_date << " to_date=" << to_date;

    QString text("");

    if (from_date.compare(to_date) != 0) {
        return QString("%1 to %2").arg(toLocalDateFormat(from_date)).arg(toLocalDateFormat(to_date));
    } else {
        return toLocalDateFormat(from_date);
    }

}

QString ApplicationUI::updatingText(int last_op_status, QString updating_from_date, QString updating_to_date, int updating_first_day_half, int updating_last_day_half, int updating_day_half)
{
    return _iModel->updatingText(last_op_status, updating_from_date, updating_to_date, updating_first_day_half, updating_last_day_half, updating_day_half);
}

void ApplicationUI::stopHeadlessService()
{
    _iModel->stopHeadlessService();
}

void ApplicationUI::startHeadlessService()
{
    _iModel->startHeadlessService();
}

void ApplicationUI::resetAdapterSettings()
{
    _iModel->resetAdapterSettings();
}

void ApplicationUI::submitApprovalDecision(int task_id, int task_type, int local_request_id, int decision, int originating_op_id)
{
    _iModel->submitApprovalDecision(task_id, task_type, local_request_id, decision, originating_op_id);
}

QString ApplicationUI::adapterName()
{
    return _iModel->adapterName();
}

QString ApplicationUI::adapterVersion()
{
    return _iModel->adapterVersion();
}

QString ApplicationUI::adapterDescription()
{
    qDebug() << "GGGG adapterDescription()";
    return _iModel->adapterDescription();
}

bool ApplicationUI::adapterConfigured()
{
    return _iModel->adapterConfigured();
}

QString ApplicationUI::setting1Name()
{
    Settings* settings = Settings::getInstance(this);
    _setting1_name = settings->getValueFor(QString(GUI_SETTING_1_NAME), "");
    return _setting1_name;
}

/*
 * For the email p2p adapter this setting is the email account name
 */
QString ApplicationUI::setting1Value()
{
    Settings* settings = Settings::getInstance(this);
    _setting1_value = settings->getValueFor(QString(GUI_SETTING_1_VALUE), "");
    return _setting1_value;
}

void ApplicationUI::setSetting1Value(QString setting_value)
{
    qDebug() << "GGGG setting value 1:" << setting_value;
    Settings* settings = Settings::getInstance(this);
    _setting1_value = setting_value;
    settings->saveValueFor(GUI_SETTING_1_VALUE, setting_value);
    emit signalSetting1ValueChanged();

    QString provider = _account_provider_map->value(setting_value);
    setSetting2Value(provider);
}

/*
 * For the email p2p adapter this setting is the email provider name
 */

QString ApplicationUI::setting2Name()
{
    Settings* settings = Settings::getInstance(this);
    _setting2_name = settings->getValueFor(QString(GUI_SETTING_2_NAME), "");
    return _setting2_name;
}

QString ApplicationUI::setting2Value()
{
    Settings* settings = Settings::getInstance(this);
    _setting2_value = settings->getValueFor(QString(GUI_SETTING_2_VALUE), "");
    return _setting2_value;
}

void ApplicationUI::setSetting2Value(QString setting_value)
{
    qDebug() << "GGGG setting value 2:" << setting_value;
    Settings* settings = Settings::getInstance(this);
    _setting2_value = setting_value;
    settings->saveValueFor(GUI_SETTING_2_VALUE, setting_value);
    emit signalSetting2ValueChanged();
}

/*
 * For the email p2p adapter this setting is the approver name
 */
QString ApplicationUI::setting3Name()
{
    Settings* settings = Settings::getInstance(this);
    _setting3_name = settings->getValueFor(QString(GUI_SETTING_3_NAME), "");
    return _setting3_name;
}

QString ApplicationUI::setting3Value()
{
    Settings* settings = Settings::getInstance(this);
    _setting3_value = settings->getValueFor(QString(GUI_SETTING_3_VALUE), "");
    return _setting3_value;
}

void ApplicationUI::setSetting3Value(QString setting_value)
{
    qDebug() << "GGGG setting value 3:" << setting_value;
    Settings* settings = Settings::getInstance(this);
    _setting3_value = setting_value;
    settings->saveValueFor(GUI_SETTING_3_VALUE, setting_value);
    emit signalSetting3ValueChanged();
}

/*
 * For the email p2p adapter this setting is the approver email address
 */
QString ApplicationUI::setting4Name()
{
    Settings* settings = Settings::getInstance(this);
    _setting4_name = settings->getValueFor(QString(GUI_SETTING_4_NAME), "");
    return _setting4_name;
}

QString ApplicationUI::setting4Value()
{
    Settings* settings = Settings::getInstance(this);
    _setting4_value = settings->getValueFor(QString(GUI_SETTING_4_VALUE), "");
    return _setting4_value;
}

void ApplicationUI::setSetting4Value(QString setting_value)
{
    qDebug() << "GGGG setting value 4:" << setting_value;
    Settings* settings = Settings::getInstance(this);
    _setting4_value = setting_value;
    settings->saveValueFor(GUI_SETTING_4_VALUE, setting_value);
    emit signalSetting4ValueChanged();
}

/*
 * For the email p2p adapter this setting is the user ID
 */
QString ApplicationUI::setting5Name()
{
    Settings* settings = Settings::getInstance(this);
    _setting5_name = settings->getValueFor(QString(GUI_SETTING_5_NAME), "");
    return _setting5_name;
}

QString ApplicationUI::setting5Value()
{
    Settings* settings = Settings::getInstance(this);
    _setting5_value = settings->getValueFor(QString(GUI_SETTING_5_VALUE), "");
    return _setting5_value.toUpper();
}

void ApplicationUI::setSetting5Value(QString setting_value)
{
    qDebug() << "GGGG setting value 5:" << setting_value;
    Settings* settings = Settings::getInstance(this);
    _setting5_value = setting_value.toUpper();
    settings->saveValueFor(GUI_SETTING_5_VALUE, setting_value);
    emit signalSetting5ValueChanged();
}

/*
 * Setting name/value pairs may be used by an adapter to indicate properties that it needs for configuration purposes.
 * The Settings.qml page by default, will offer these properties as editable text fields so that the user can enter values.
 * The documentation for the adapter should indicate what exactly these values are.
 *
 * When customising a UI however, the developer is free to use the suggested property names from the adapter or ignore them. In general, a custom Settings.qml
 * page should provide the user with the best experience.
 *
 */
void ApplicationUI::onSettingsChanged()
{
    qDebug() << "GGGG onSettingsChanged";
    emit signalSetting1NameChanged();
    emit signalSetting1ValueChanged();
    emit signalSetting2NameChanged();
    emit signalSetting2ValueChanged();
    emit signalSetting3NameChanged();
    emit signalSetting3ValueChanged();
    emit signalSetting4NameChanged();
    emit signalSetting4ValueChanged();
    emit signalSetting5NameChanged();
    emit signalSetting5ValueChanged();
}

bool ApplicationUI::isInboxEmpty()
{
    return _iModel->isInboxEmpty();
}

void ApplicationUI::checkAdapterSettings()
{
    _iModel->checkAdapterSettings();
}

void ApplicationUI::onZeroDaysWarning() {
    Utilities::toast("WARNING: your request was for zero days in real terms");
}

void ApplicationUI::onUpdateReceived() {
    Utilities::toast("An update was received");
}

void ApplicationUI::onSuggestShowRequestsTab() {
    qDebug() << "GGGG onSuggestShowRequestsTab";
    showTab(TAB_REQUESTS);
}

void ApplicationUI::onSuggestShowInboxTab() {
    showTab(TAB_INBOX);
}

void ApplicationUI::onSyncComplete() {
    Utilities::toast("Synchronisation Completed");
}

void ApplicationUI::onOperationNotSupported() {
    Utilities::toast("Operation not supported by adapter");
}

void ApplicationUI::onAdapterNotConfigured() {
    Utilities::toast("Please check application settings - Swipe down to access");
}

void ApplicationUI::loadTaskData() {
    _iModel->loadTaskData();
}

void ApplicationUI::loadRequestData(QString leave_year) {
    _iModel->loadRequestData(leave_year);
}

void ApplicationUI::populateEmailSettingControls() {

    bb::cascades::Sheet *settings_sheet = _root->findChild<bb::cascades::Sheet*>("settings");
    qDebug() << "GGGG found Settings sheet? " << settings_sheet;
    bb::cascades::DropDown *email_accounts_dropdown = settings_sheet->findChild<bb::cascades::DropDown*>("dd_email_accounts");
    qDebug() << "GGGG dropDown:" << email_accounts_dropdown->selectedOption();

    _account_provider_map = new QMap < QString, QString >();

    int account_counter=0;
    foreach (const bb::pim::account::Account &account, _accountList) {
        qDebug() << "GGGG account: displayName=" << account.displayName() << " providerName=" << account.provider().name() << endl;
        if ((account.provider().name().compare("Pin to Pin") != 0) && (account.provider().name().compare("sms-mms") != 0)) {
            Option *option = Option::create().text(account.displayName()).value(account.displayName()).selected(account_counter == 0);
            email_accounts_dropdown->add(option);
            _account_provider_map->insert(account.displayName(),account.provider().name());
        }
    }

}

bool ApplicationUI::isValidEmailAddress(QString email_address) {
    return Utilities::isValidEmailAddress(email_address);
}
