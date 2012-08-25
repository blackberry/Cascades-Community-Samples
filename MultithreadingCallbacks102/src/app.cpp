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

/*
 * Legal Information for images used.
 *
 * Image: http://upload.wikimedia.org/wikipedia/commons/6/62/Peace_riding_in_a_triumphal_chariot_Bosio_Carrousel_-_2012-05-28.jpg
 * Attribution: By PierreSelim (Own work) [CC-BY-3.0 (http://creativecommons.org/licenses/by/3.0)], via Wikimedia Commons
 *
 * Image: http://upload.wikimedia.org/wikipedia/commons/a/af/Crepuscular_rays_with_reflection_in_GGP.jpg
 * Attribution: By Brocken Inaglory (Own work) [CC-BY-SA-3.0 (http://creativecommons.org/licenses/by-sa/3.0) or GFDL (http://www.gnu.org/copyleft/fdl.html)], via Wikimedia Commons
 *
 * Image: http://upload.wikimedia.org/wikipedia/commons/2/2a/Anodorhynchus_hyacinthinus_-Hyacinth_Macaw_-side_of_head.jpg
 * Attribution: By Randy (originally posted to Flickr as Hyacinth) [CC-BY-2.0 (http://creativecommons.org/licenses/by/2.0)], via Wikimedia Commons
 *
 * Image: http://upload.wikimedia.org/wikipedia/commons/2/29/Bataille_Waterloo_1815_reconstitution_2011_cuirassier.jpg
 * Attribution: By Myrabella (Own work) [CC-BY-SA-3.0 (http://creativecommons.org/licenses/by-sa/3.0)], via Wikimedia Commons
 *
 * Image: http://upload.wikimedia.org/wikipedia/commons/e/ec/Armadillo_Aerospace_Pixel_Hover.jpg
 * Attribution: By Armadillo Aerospace/Matthew C. Ross [CC-BY-3.0-us (http://creativecommons.org/licenses/by/3.0/us/deed.en)], via Wikimedia Commons
 *
 * Image: http://upload.wikimedia.org/wikipedia/commons/f/f5/A_sculpture_at_the_entrance_to_the_palace_of_Versailles.jpg
 * Attribution: By Chris Robinson [CC-BY-2.0 (http://creativecommons.org/licenses/by/2.0)], via Wikimedia Commons
 *
 * Image: http://upload.wikimedia.org/wikipedia/commons/6/6d/Firehole_river_at_Upper_Geyser_Basin-2008-june.jpg
 * Attribution: By Brocken Inaglory (Own work) [CC-BY-SA-3.0 (http://creativecommons.org/licenses/by-sa/3.0) or GFDL (http://www.gnu.org/copyleft/fdl.html)], via Wikimedia Commons
 *
 * Image: http://upload.wikimedia.org/wikipedia/commons/7/7c/Peugeot_206_WRC.jpg
 * Attribution: By Christopher Batt ( http://www.flickr.com/people/christopherbatt/ ) [CC-BY-SA-2.0 (http://creativecommons.org/licenses/by-sa/2.0)], via Wikimedia Commons
 *
 * Image: http://upload.wikimedia.org/wikipedia/commons/9/97/Toda_Hut.JPG
 * Attribution: Pratheep P S [CC-BY-SA-2.5 (http://creativecommons.org/licenses/by-sa/2.5)], via Wikimedia Commons
 *
 * Image: http://upload.wikimedia.org/wikipedia/commons/d/dc/Marriott_Center_1.JPG
 * Attribution: By Mark A. Philbrick (Brigham Young University) [CC-BY-SA-3.0 (http://creativecommons.org/licenses/by-sa/3.0)], via Wikimedia Commons
 */
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Image>
#include <bb/cascades/ImageView>
#include <bb/cascades/PixelBufferData>
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/data/JsonDataAccess>
#include <QVariant>
#include "app.hpp"
#include "LoadImage.hpp"

using namespace bb::cascades;

/**
 * This app demonstrates the techniques how to use multi-threads to handle network activities
 * and sync the results with the main thread.
 *
 * In this app, you will learn:
 * 	-how to use QNetworkAccessManager to perform asynchronous network requests.
 * 	-how to subclass QThread to perform time-consuming operations in their own threads.
 *
 */


/**
 * App::App()
 *
 * Initial setup to expose c++ objects/methods to be used by QML view
 *
 * so that the UI thread will be able to invoke  asynchronous image downloading.
 *
 */
App::App()
{
    QmlDocument* qml = QmlDocument::create("main.qml");
    qml->setContextProperty("cs", this);

    m_root = qml->createRootNode<AbstractPane>();
    Application::setScene(m_root);
}

/**
 * App::onImageLoaded(QString url, bool success, QString response)
 *
 * Handle the result of a loading image request.
 *
 * Basing on the result, we will update UI accordingly.
 *
 * --url, the url of the previous url
 * --success, the flag to indicate if the request was successful.
 * --response, this will carry some error information if the operation was not successful.
 *
 */
void App::onImageLoaded(QString url, bool success, QString response)
{
    //get the LoadImage object
    LoadImage* img = qobject_cast<LoadImage*>(sender());

    if (!img)
    {
        return;
    }

    connect(this, SIGNAL(finishUpdateImage()), img, SIGNAL(finishThread()));

    qDebug() << "Download complete: " << url;

    //find the url to photo index mapping
    QMap<QString, int>::iterator it = m_imageMap.find(url);
    if (it != m_imageMap.end())
    {
        //stop indicator
        ActivityIndicator* activityId = m_root->findChild<ActivityIndicator*>(QString("image%1Indicator").arg(it.value()));
        if (activityId)
        {
            activityId->stop();
            activityId->setVisible(false);
        }

        if (success)
        {
            //show the image
            ImageView* imageView = m_root->findChild<ImageView*>(QString("image%1").arg(it.value()));
            if (imageView)
            {
                QImage qm = img->getImage().rgbSwapped().scaled(768, 500, Qt::KeepAspectRatioByExpanding); //resize image to fit container
                bb::cascades::PixelBufferData pb = bb::cascades::PixelBufferData(PixelBufferData::RGBX, qm.width(), qm.height(), qm.width(), qm.bits());
                bb::cascades::Image image(pb);
                imageView->setImage(image);
                imageView->setVisible(true);
            }
        }
        else
        {
            //show the label with the error from the retrieval
            Label* imageLabel = m_root->findChild<Label*>(QString("image%1Label").arg(it.value()));
            if (imageLabel)
            {
                imageLabel->setText(response);
                imageLabel->setVisible(true);
            }
        }
        m_imageMap.erase(it);
    }
    emit finishUpdateImage();
   // no need to explicitly delete img, it has been destroyed when receiving the "finishThread" signal
}

/**
 * App::loadImage(QString const& url, int id)
 *
 * This method is to be invoked from main thread to initialize an asynchronous
 * request of down loading an image from the network.
 * --url, the url to down load the image.
 * --id, the id used to update UI components later on.
 */
void App::loadImage(QString const& url, int id)
{
    QMap<QString, int>::iterator it = m_imageMap.find(url);
    if (it != m_imageMap.end())
    {
        qDebug() << "url: " << url << " download is still in progress..";
        return;
    }

    m_imageMap[url] = id;

    //use LoadImage to download and convert the photo into a QImage in a background thread
    LoadImage* lm = new LoadImage;
    lm->load(url);
    connect(lm, SIGNAL(complete(QString, bool, QString)), this, SLOT(onImageLoaded(QString, bool, QString)));

    qDebug() << "Downloading image: " << url;
}
