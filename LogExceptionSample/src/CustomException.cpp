/* Copyright (c) 2012 Research In Motion Limited.
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

#include "CustomException.hpp"
#include <Qdebug>

/**
 * This class sub class bb::Exception class to create custom Exception
 */

/**
 * Default constructor
 */
CustomException::CustomException()
    : m_errorMessage("")
{
    // void
}

/**
 * Copy constructor
 */
CustomException::CustomException(const CustomException& customException)
    : m_errorMessage("")
{
    m_errorMessage = customException.getErrorMessage();
}


/**
 * Assignment operator
 */
CustomException& CustomException::operator=(const CustomException& customException)
{
    m_errorMessage = customException.getErrorMessage();
    return *this;
}

/**
 * Parameterized constructor
 *
 */
CustomException::CustomException(const QString& errorMessage)
    : m_errorMessage("")
{
    m_errorMessage = errorMessage;
}

/**
 * destructor
 */
CustomException::~CustomException() throw()
{
    // void
}

/**
 * Retrieve the custom message for this exception
 */
QString CustomException::getErrorMessage() const
{
    return m_errorMessage;
}

/**
 * Set the custom message for this exception
 */
void CustomException::setErrorMessage(const QString& errorMessage)
{
    m_errorMessage = errorMessage;
}

/**
 * CustomException::logExceptionAsWarning()
 *
 * logging the exception as warning to the log file
 */
void CustomException::logExceptionAsWarning() const
{
    qWarning() << m_errorMessage.toStdString().c_str() << "\n";
}

