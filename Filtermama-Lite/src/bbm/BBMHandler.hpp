/* Copyright (c) 2012, 2013  BlackBerry Limited.
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


#ifndef BBMHANDLER_HPP
#define BBMHANDLER_HPP

#include <bb/platform/bbm/Context>
#include <bb/platform/bbm/MessageService>
#include <bb/platform/bbm/RegistrationState>
#include <bb/system/SystemUiResult>
#include <bb/platform/bbm/UserProfile>

#include <QtCore/QObject>
#include <QUuid>

/**
 * @brief A class that encapsulates the registration at the BBM Social Platform.
 */
class BBMHandler : public QObject
{
    Q_OBJECT

    // Flag indicating whether the application is successfully registered
    // with BBM.
    Q_PROPERTY(bool allowed READ isAllowed NOTIFY stateChanged)

    // The status message describing the registration process.
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY stateChanged)

    // Flag indicating whether registration failed due to a temporary error.
    // This allows the user to re-try registration.
    Q_PROPERTY(bool temporaryError READ isTemporaryError NOTIFY stateChanged)

    Q_PROPERTY(bb::platform::bbm::Context* context READ context NOTIFY stateChanged);

public:

	// This method is invoked to open the invitation dialog
	Q_INVOKABLE
	void sendInvite();

	// This method is invoked to change the user's personal message
	Q_INVOKABLE void setPersonalMessage(const QString& message);

    // Enumerates the possible registration progress states.
    struct BbmRegistrationProgress
    {
        enum Type {
            // Registration has not started and has never been attempted since
            // the application started.
            NotStarted = 0,
            // Registration has started.
            Started,
            // Registration is in progress.
            Pending,
            // Registration is done. Use isRegistered() or
            // Context::isAccessAllowed() to check if the application is
            // registered successfully.
            Finished
        };
    };

    /**
     * Creates a new registration handler.
     *
     * @param uuid The unique ID of the application
     * @param parent The parent object
     */
    BBMHandler(const QUuid &uuid, QObject *parent = 0);

    /**
     * Returns the BBM context that is associated with this application.
     */
    bb::platform::bbm::Context* context()
    { return m_context; }

    /**
     * Returns the registration progress.
     * @see BbmRegistrationProgress::Type
     */
    BbmRegistrationProgress::Type progress() const
    { return m_progress; }

public Q_SLOTS:
    /**
     * This method is called to trigger the registration with the BBM Social
     * Platform. Check the progress prior to calling this function to ensure
     * that another registration is not in progress.
     */
    void registerApplication();

    /**
     * This method is invoked in order to generate a registered signal
     * on demand based on user interaction.
     */
    void finishRegistration();

Q_SIGNALS:
    // The change notification signal of the properties.
    void stateChanged();

    // The registered notification signal.
    void registered();

private Q_SLOTS:
    // This slot is invoked whenever the registration status is changed.
    // This will initiate, continue, or finish registration based on the status.
    // @param state is the registration state
    void processRegistrationStatus(const bb::platform::bbm::RegistrationState::Type state);

    // This slot is invoked when the uuid is invalid or NULL.
    // This will cause the application to exit with error code -1
    // @param value is the system ui result indicating which button was pressed
    void dialogFinished(bb::system::SystemUiResult::Type value);

private:
    // Return true if registration has completed successfully.
    bool isAllowed() const
    { return m_isAllowed; }

    // Return true if registration failed due to a temporary error.
    bool isTemporaryError() const
    { return m_temporaryError; }

    // Return the message that describes the registration state.
    const QString& statusMessage() const
    { return m_statusMessage; }

    // Registration finished. This method updates m_registered, m_statusMessage,
    // and m_progress. It emits a stateChanged() signal.
    void registrationFinished();

    // BBM Social Platform Context used to gain access to BBM functionality.
    bb::platform::bbm::Context* m_context;

    // A flag that indicates whether registration completed successfully.
    bool m_isAllowed;

    // Registration progress. Use this to check if you have already attempted
    // registration, if registration has finished, or it's still in progress.
    BbmRegistrationProgress::Type m_progress;

    // A flag that indicates whether registration failed due to a temporary
    // error.
    bool m_temporaryError;

    // A status message that describes the current state of registration.
    QString m_statusMessage;

    // The service object to send BBM messages
    bb::platform::bbm::MessageService* m_messageService;

    // pointer to user profile object
    bb::platform::bbm::UserProfile* m_userProfile;
};

#endif
