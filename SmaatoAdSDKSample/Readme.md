# SmaatoSDKSample

The purpose of this application is to demonstrate how to use [Smaato's Mobile Advertising SDK  for BlackBerry 10](http://www.smaato.com/sdks).

The Smaato static library  included within the sample is from ***Smaato SDK for BlackBery 10 v1.1.2***. To ensure you are running the latest, please [download](http://www.smaato.com/sdks) the latest SDK for Smaato's website and replace the **SmaatoSDK** folder within the project.

The Smaato SDK is covered under these [terms](http://www.smaato.com/terms/)

The sample code for this application is Open Source under the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://bdsc.webapps.blackberry.com/cascades/)

**Author(s)** 

* [Sarim Aziz](https://github.com/saziz)
* [Simon Hsu](https://github.com/icetingyu)
* [Alan Wong](https://github.com/alawong)

**Release History**

* **v1.0.0** - Initial release

**Known Issues**

* None

**Dependencies**

1. BlackBerry Device Software **10.x ** and greater
2. Smaato SDK **v1.1.2**
2. Smaato **Publisher ID**. Sign up for free at [http://www.smaato.com][1] 
3. Smaato **Ad Space ID**. Create an Ad Space for your sample after registration.

**How to Build SmaatoSDKSample**

Simply Import the project into a workspace in your NDK. 

The **SmaatoSDKSample** application demonstrates how to integrate ads from Smaato using QML and C++.

**Using SSmaatoAdView in QML**

    SSmaatoAdView {
        id: adView
        adSpaceId: "0"  
        publisherId: "0" 
        preferredWidth: 768
        preferredHeight: 128
        viewSize: SSmaatoAdView.AdViewSizeNormal
        format: 1
        coppa: 0
        age: 30
        gender: SSmaatoAPI.Male
        tags: "sports,news"
        zip: "M17 1LB"
        autoRefreshPeriod: 30
        onAdUpdated: {
            if(success) {
            // code for success update
            } else {
            // code for failed update
            }
        }
    }

**Using SSmaatoAdView in C++**

    SSmaatoAdView* _adView = new SSmaatoAdView();
    	_adView->setAdSpaceId("0");
    	_adView->setPublisherId("0");
    	_adView->setAdViewSize(SSmaatoAdView::AdViewSizeNormal);
    	_adView->setFormat(1);
    	_adView->setCoppa(0);
    	_adView->setGender(SSmaatoAPI::Male);
    	_adView->setAge(35);
    	_adView->setZip("90210");
    	_adView->setTags("News,Sports,Travel");
    	_adView->setAutoRefreshPeriod(30);
        _adView->setScale(2.4,2.4);
        _adView->setPreferredWidth(720);
        _adView->setTranslationY(40);
    	_adView->updateAd();
    	mainContainter->add(_adView);


**Getting Real Ads**

You need to replace the **Publisher ID** and **AdSpace ID** with your own to get real ads. Leaving them at 0 will get you Smaato's test ad.

Refer to the Smaato documentation included within the Smaato SDK for further details on supported parameters.
 
## Contributing Changes

To contribute code to this repository you must be [signed up as an official contributor](http://blackberry.github.com/howToContribute.html).

Please see the [README](https://github.com/blackberry/Cascades-Community-Samples/blob/master/README.md) of the Cascades-Community-Samples repository for instructions on how to add new Samples or make modifications to existing Samples.

## Bug Reporting and Feature Requests

If you find a bug in a Sample, or have an enhancement request, simply file an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues) for the Sample and send a message (via github messages) to the Sample Author(s) to let them know that you have filed an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues).


## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


  [1]: http://www.smaato.com