TranslateHL
--------------
In this sample app, you'll learn how to translate the headless part of your app (service). It relies on QTranslator and works with the standard QObject::tr() function you've been used to work with in C++. As LocaleHandler class relies on Cascades library, headless needs to use something else to get the locale installed and be notified of locale changes, even if UI isn't running. This sample app uses a custom class with BPS locale.h to replace LocaleHandler class.

Make sure to add this to your bar-descriptor.xml file (the first part should already be there)
```sh
    <!-- Locale support -->
    <asset path="translations" dest="qm">
        <include name="*.qm" />
    </asset>
    <!-- Don't forget to add the path to your Headless translations folder -->
    <asset path="../TranslateHLService/translations" dest="qm">
        <include name="*.qm" />
    </asset>
```

And add the desired languages to your headless pro file (French, English, Spanish and Italian added in this project)
```sh
TRANSLATIONS = \
    $${TARGET}_fr.ts \
    $${TARGET}_en.ts \
    $${TARGET}_es.ts \
    $${TARGET}_it.ts \
    $${TARGET}.ts
```

Your headless should populates ts files in the translations folder of your headless app, you just have to translate those with QtLinguist or any other tool you'd like.

Be warn that changing the language many times in a row from the Settings app might slow down your device, that's why I've included a 'simulate' button to simulate a language change without changing the OS language, it just makes the app believes there was a language change.


Contributors: Roger Leblanc


##Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.