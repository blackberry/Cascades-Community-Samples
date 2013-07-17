# AES Crypto Demo

The purpose of this demo is to see how to use AES from in Qt. This demo does not explain how or why you'd use AES.

## Approach

If you're a proficient C coder - you might find this demo convoluted. Please, for your own sanity, avert your eyes.

The author finds [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) to be one of the strongest 
assets of the c++ language. To this end, the contexts required by the Certicom API are allocated and deallocated 
in constructors and destructors - instead of monolithic if/goto blocks.

### Storing Keys, IVs and Cipher Text

Keys, IVs and cipher text are pseudo random binary data. Think carefully about encodings if you're trying to store 
them in strings. It is *very* probable that any of them will contain invalid characters or encodings. 

It is very convenient (especially when using a GUI) to represent data like this as hex bytes. That's what this demo
does.

## Limitations

### Hard Wiring

Several constants are used where a full functional library would allow parameterisation. For example;

* The key size is 128 bits. 
* The mode of AES is set to CBC.

### Padding

Block ciphers like AES and DES require consistent padding to complete partial blocks. The current Certicom API does not
expose any (suitable) padding algorithms. PKCS#5 padding is implemented in this demo.

#### A warning about Padding...

Do *not* rely on padding for message integrity. Use a SHA, or a HMAC, or an authenticated cipher suite.

Here's an exercise to demonstrate why relying on padding is bad:

1. Enter a message that is 3 or 4 blocks long in the plain text field. 
2. Generate a key and IV.
3. Encrypt it
4. Tamper with a single byte near the start or middle of the cipher text.
5. Decrypt it


## Info

**License**

The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://developer.blackberry.com/cascades/)
* [BlackBerry Native SDK](http://developer.blackberry.com/native/)

**Author(s)** 

* [Rob Williams](https://github.com/robbieDubya) - Twitter: @robbieDubya

**Release History**

* **V1** - Initial release

**Known Issues**

None

**Dependencies**

1. BlackBerry 10 Device Software **10.2**
1. BlackBerry 10 Native SDK **10.2** 

With minimal massaging it will compile on 10.0.

**How to Build AES Crypto Demo**

Import the project into a workspace inside Momentics.
 
**To contribute code to this repository you must be [signed up as an 
official contributor](http://blackberry.github.com/howToContribute.html).**

## Contributing Changes

Please see the [README](https://github.com/blackberry/Cascades-Community-Samples/blob/master/README.md) 
of the Cascades-Community-Samples repository for instructions on how to add new Samples or 
make modifications to existing Samples.


## Bug Reporting and Feature Requests

If you find a bug in a Sample, or have an enhancement request, simply file 
an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues) for 
the Sample and send a message (via github messages) to the Sample Author(s) to let 
them know that you have filed an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues).


## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
OTHER DEALINGS IN THE SOFTWARE.
