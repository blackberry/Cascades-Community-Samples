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
#include "GraphControl.h"

#include <bb/cascades/Container>
#include <bb/cascades/Color>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Image>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>

#include <bb/ImageData>
#include <bb/PixelFormat>

using namespace bb;
using namespace bb::cascades;

/*
 * Colors:
 * light column    0F233C
 * dark column     0D1F35
 * divider lines   35475B
 * label bkg       050C14
 * graph lines     63E2D9 (thickness 3-4 px)
 *
 */

GraphControl::GraphControl(Container *parent) :
        CustomControl(parent)
{
    Container *content = new Container();
    //content->setBackground(Color(Color::Red));

    // The content Container will be set to lay out children using a dock layout (to center everything on screen).
    content->setLayout(new DockLayout());

    Container *xAxis = Container::create().vertical(VerticalAlignment::Bottom).horizontal(HorizontalAlignment::Fill).background(Color::fromARGB(0xff2F4C6E)).preferredHeight(5);
    Container *yAxis = Container::create().vertical(VerticalAlignment::Fill).horizontal(HorizontalAlignment::Left).background(Color::fromARGB(0xff2F4C6E)).preferredWidth(5);

    content->add(xAxis);
    content->add(yAxis);

    // Decoration grid only, needs adjusting to data for a more realistic implementation
    for(int x = 200; x < 1280; x += 200) {
        content->add(Container::create().vertical(VerticalAlignment::Fill).background(Color::fromARGB(0xff2F4C6E)).preferredWidth(3).translate(x, 0));
    }
    for(int y = 200; y < 768; y += 200) {
        content->add(Container::create().horizontal(HorizontalAlignment::Fill).background(Color::fromARGB(0xff2F4C6E)).preferredHeight(3).translate(0, y));
    }

    /*
    mDebugLabel1 = Label::create().translate(30, 30).text("1");
    mDebugLabel2 = Label::create().translate(30, 130).text("2");
    content->add(mDebugLabel1);
    content->add(mDebugLabel2);
     */
    mIsCreated = false;
    connect(this, SIGNAL(creationCompleted ()), this, SLOT(onCreationCompleted ()));

    setRoot(content);
}

GraphControl::~GraphControl()
{
    free(mBuffer);
}

void GraphControl::drawLine(QPoint point)
{
    static const uint ALPHA = 0xFF;

    uint width = mDrawArea->preferredWidth();
    uint height = mDrawArea->preferredHeight();

    QPoint currentPoint(((point.x() - mMinPoint.x()) * mXNormalization),
            ((point.y() - mMinPoint.y()) * mYNormalization));

    if ((currentPoint.x() >= 0 && currentPoint.x() < (int) width)
            && (currentPoint.y() >= 0 && currentPoint.y() < (int) height)) {

        int brushSize = 4;
        int arraySize = width * height * 4;

        for (int i = 0; i < brushSize; i++) {

            if (mPreviousPoint.x() != -1 && mPreviousPoint.y() != -1
                    && mPreviousPoint != currentPoint) {
                // A straight line
                // y = k * x + m
                // k = (y1-y2) / (x1 - x2)
                // m = y1 - k * x1

                float k = (float) (mPreviousPoint.y() - currentPoint.y())
                        / (float) (mPreviousPoint.x() - currentPoint.x());
                float m = currentPoint.y() - k * currentPoint.x();

                for (int lineX = mPreviousPoint.x(); lineX < currentPoint.x(); lineX++) {

                    for (int i = 0; i < brushSize; i++) {
                        int brushX = lineX + i;
                        int xOffset = brushX * 4;
                        int lineY = k * lineX + m;

                        for (int j = 0; j < brushSize; j++) {
                            int brushY = height - (lineY + j); // height - y pos to get coordinate system turned correctly.

                            int yOffset = width * brushY * 4;
                            int offset = xOffset + yOffset;

                            if ((offset + 3) < arraySize) {

                                mBuffer[(offset)] = (0x63 * ALPHA) >> 8;
                                mBuffer[(offset + 1)] = (0xE2 * ALPHA) >> 8;
                                mBuffer[(offset + 2)] = (0xD9 * ALPHA) >> 8;
                                mBuffer[(offset + 3)] = ALPHA;
                            }
                        }
                    }
                }
            }

            mPreviousPoint = currentPoint;
        }
    }
}

void GraphControl::generatePixels(uint width, uint height, unsigned char* buf)
{
    for (uint y = 0; y < height; ++y) {
        for (uint x = 0; x < width; ++x) {
            buf[0] = (0xFF * 0x00) >> 8;
            buf[1] = (0xFF * 0x00) >> 8;
            buf[2] = (0xFF * 0x00) >> 8;
            buf[3] = 0x00;
            buf += 4;
        }
    }
}

void GraphControl::setUpDrawArea(uint width, uint height)
{
    Container *content = qobject_cast<Container*>(this->root());

    if (content) {
        // Remove the current recipe once we return to the ListView
        if (mDrawArea && content->remove(mDrawArea)) {
            delete mDrawArea;
        }
    }

    mDrawArea = new ImageView();
    mDrawArea->setPreferredSize(width, height);

    if (!mBuffer) {
        mBuffer = (unsigned char*) malloc((width + 1) * height * 4);
    }

    generatePixels(width, height, mBuffer);
    content->add(mDrawArea);

}

void GraphControl::drawGraph()
{
    uint width = this->preferredWidth();
    uint height = this->preferredHeight();

    mXNormalization = ((float) width) / (mMaxPoint.x() - mMinPoint.x());
    mYNormalization = ((float) height) / (mMaxPoint.y() - mMinPoint.y());

    //mDebugLabel1->setText(QString::number( mXNormalization));
    //mDebugLabel2->setText(QString::number( mYNormalization));

    setUpDrawArea(width, height);

    if (!mValues.isEmpty()) {
        for (int i = 0; i < mValues.size(); ++i) {
            drawLine(mValues.at(i));
        }

        if (mBuffer) {
            Image img(
                    bb::ImageData::fromPixels(mBuffer, bb::PixelFormat::RGBA_Premultiplied, width,
                            height, width * 4));
            mDrawArea->setImage(img);
        }
    }
}

bool GraphControl::readGraphDataSource()
{
    // Now read the data from the file and plot the lines.
    QString appFolder(QDir::homePath());
    appFolder.chop(4);
    QString dataFileName = appFolder + "app/native/assets/" + mGraphDataSource;
    QFile dataFile(dataFileName);

    if (dataFile.exists()) {
        if (dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            mValues.clear();

            int counter = 0;
            while (!dataFile.atEnd()) {
                float x = 0;
                float y = 0;
                QString dataLine = dataFile.readLine();
                QStringList dataValues = dataLine.split(",");

                x = dataValues.at(0).toFloat();
                y = dataValues.at(1).toFloat();

                mValues << QPoint(x, y);

                // Find max and min values for normalization of data.
                if (counter == 0) {
                    mMaxPoint.setX(x);
                    mMaxPoint.setY(y);

                    mMinPoint.setX(x);
                    mMinPoint.setY(y);
                } else {
                    if (x < mMinPoint.x()) {
                        mMinPoint.setX(x);
                    }
                    if (y < mMinPoint.y()) {
                        mMinPoint.setY(y);
                    }
                    if (x > mMaxPoint.x()) {
                        mMaxPoint.setX(x);
                    }
                    if (y > mMaxPoint.y()) {
                        mMaxPoint.setY(y);
                    }
                }

                counter++;
            }
            dataFile.close();



            return true;
        }
    } else {
        qDebug() << "Failed to open data source for graph: " + mGraphDataSource;
    }
    return false;
}

void GraphControl::onCreationCompleted()
{
    mIsCreated = true;

    if (!mGraphDataSource.isEmpty()) {
        if (readGraphDataSource()) {
            drawGraph();
        }
    }
}

void GraphControl::setGraphDataSource(QString graphDataSource)
{
    if (mGraphDataSource.compare(graphDataSource) != 0) {
        mGraphDataSource = graphDataSource;
        emit graphDataSourceChanged(mGraphDataSource);

        // The first time the control is created delay reading and drawing until
        // all initialization of properties is done (in order to get width and height
        // properties set in QML)
        if (mIsCreated) {
            if (readGraphDataSource()) {
                drawGraph();
            }
        }
    }
}

QString GraphControl::graphDataSource()
{
    return mGraphDataSource;
}
