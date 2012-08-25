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

#ifndef CUSTOMEXCEPTION_HPP_
#define CUSTOMEXCEPTION_HPP_

#include <QtCore/QString>
#include <bb/Exception>

class CustomException : public bb::Exception
{
public:

    /*
     * Default constructor. Will initialize error message with "".
     */
    CustomException();

    /*
     * Copy Constructor.
     * @param customException The object from which to copy members
     */
    CustomException(const CustomException& customException);

    /*
     * Assignment Operator.
     * @param customException The object from which to assign members
     */
    CustomException& operator=(const CustomException& customException);

    /*
     * Parameterized construction
     * @param errorMessage The value used to set the error message.
     */
    CustomException(const QString& errorMessage);

    /*
     * virtual Destructor. May throw.
     */
    virtual ~CustomException() throw();

    /*
     * Accessor for m_errorMessage member.
     * @return a copy of the m_errorMessage member.
     */
    virtual QString getErrorMessage() const;

    /*
     * Setter for the m_errorMessage member.
     * @param errorMessage The new value of the error message.
     */
    virtual void setErrorMessage(const QString& errorMessage);

    /*
     * Method used to print the exception message to the console.
     */
    void logExceptionAsWarning() const;

private:

    /*
     * The member holding the error message value.
     */
    QString m_errorMessage;

};

#endif /* CUSTOMEXCEPTION_HPP_ */
