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

#ifndef _GraphControl_H_
#define _GraphControl_H_

#include <bb/cascades/CustomControl>

using namespace bb::cascades;

namespace bb
{
    namespace cascades
    {
        class Container;
        class ImageView;
        class Label;
    }
}

/**
 * GraphControl Description:
 * 
 * This is a custom control that will present data contained in a file
 * as a graph.
 */
class GraphControl: public bb::cascades::CustomControl
{
Q_OBJECT

    /**
     * The path to the data source to be used, needs to be a two column comma separated text file.
     */
    Q_PROPERTY(QString graphDataSource READ graphDataSource WRITE setGraphDataSource NOTIFY graphDataSourceChanged)

public:
    GraphControl(Container *parent = 0);
    ~GraphControl();

    /**
     * Sets the data source path for the Graph.
     *
     * @param graphDataSource the path to the data source.
     */
    void setGraphDataSource(QString graphDataSource);

    /**
     * Returns the current data source path.
     *
     * @return A string containing the path to the data source.
     */
    QString graphDataSource();

signals:
    void graphDataSourceChanged(QString graphDataSource);

private slots:
    void onCreationCompleted();

private:
    // Functions for setting up and drawing to a pixel buffer that will
    // visualize the graph data in an ImageView.
    void setUpDrawArea(uint width, uint height);
    void generatePixels(uint width, uint height, unsigned char* buf);
    void drawLine(QPoint currentPoint);
    void drawGraph();
    bool readGraphDataSource();

    // Custom control components.
    ImageView *mDrawArea;
    unsigned char *mBuffer;
    QString mGraphDataSource;

    // State variable
    bool mIsCreated;

    // Variables used to draw lines in the pixelbuffer presented in mDrawArea
    QPoint mMaxPoint;
    QPoint mMinPoint;
    QList<QPoint> mValues;
    float mXNormalization;
    float mYNormalization;
    QPoint mPreviousPoint;

};

#endif // ifndef _GraphControl_H_
