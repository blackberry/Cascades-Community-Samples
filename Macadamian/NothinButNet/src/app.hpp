/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
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

#ifndef APP_H
#define APP_H

#include <QtCore/QObject>
#include <QtCore/QMetaType>

#include <bb/cascades/Event>
#include <bb/cascades/UiObject>
#include <bb/cascades/Control>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/NavigationPane>

#include "Matchups.h"
#include <GLES/gl.h>

class App : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

    enum TeamType{TeamA,TeamB};
public:
    App();

    Q_INVOKABLE void reset();

    Q_INVOKABLE int getWidth(QString team);
    Q_INVOKABLE int getHeight(QString team);
    Q_INVOKABLE int getTweetTotal();
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void goBack();
    Q_INVOKABLE void goForward(QString screenname);
    Q_INVOKABLE void selectTeamsFromRotation(float rotation);
    int value();
    void setValue(int i);
    void loadPage(QString pagename);

signals:
    void valueChanged(int);

private:
    /*!
     * returns a point in a bezier curve
     *
     * @param t - the segment
     * @param p0 - point 0
     * @param p1 - point 1
     * @param p2 - point 2
     * @param p3 - point 3
     * @return the point
     */
    QPoint CalculateBezierPoint(GLfloat t,QPoint p0, QPoint p1, QPoint p2, QPoint p3);

    /*!
     * Calculates the vertex array of points where the team names are draw around the circle
     *
     * @param segment  - the number of segments
     * @param p0 - point 0
     * @param p1 - point 1
     * @param p2 - point 2
     * @param p3 - point 3
     * @return the vertex
     */
    GLfloat* TeamCurve(int segments, QPoint p0, QPoint p1, QPoint p2, QPoint p3);

    /*!
     *
     */
    void PrepMinutesChart(bb::cascades::AbstractPane *root, data::Team *teamA, data::Team *teamB);

    /*!
     *
     */
    void PrepReboundsChart(bb::cascades::AbstractPane *root, data::Team *teamA, data::Team *teamB);

    /*!
     * Updates the QML page with the player's minutes per game and name
     *
     * @param TeamType - the team type
     * @param root - the root of the qml document
     * @param qmlId - objectName of the player control
     * @param playername - the players name
     * @param minutes - player minutes text
     * @param pct - the percentage position on the gradient.
     */
    void UpdateMinutesPlayer(TeamType type, bb::cascades::AbstractPane *root, QString qmlId, QString playername, float minutes, int pct);

private:
    int m_teamSetIndex;
    GLfloat* teamABezier;   //vertices of team A positions
    GLfloat* teamBBezier;   //vertices of team B positions
    int m_iValue;
    QMap<QUuid,QUuid> m_east, m_west;
    bb::cascades::NavigationPane* navigationPane;
};

#endif // ifndef APP_H
