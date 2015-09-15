# Some Simple Cryptography API Examples

I had personally found the description of the various APIs in the incredibly powerful **BlackBerry 10 Security Builder Cryptography Library** quite intimidating and getting started with them felt like an uphill struggle. However, over a period of time I had accumulated a number of fairly standard code snippets that used various common capabilities of this library which I thought I ought to share with the community

This is the purpose of this application which will specifically take a look at:

1. Generating a simple random number
2. Calculating Hashes (**SHA1**, **SHA256**) and **HMAC** using a predefined key.
3. Using RSA
	1. Creating an **RSA** public/private key pair
	2. Creating a self-signed certficate using this pair of keys showing how to select **SHA256** as the hash for the **RSA** certificate signing rather than the default **MD5**.
	3. Encrypting plaintext and decrypting the resulting ciphertext using this pair of keys.
4. Demonstrate Key Derivation from a shared secret using SHA1-KDF2

It doesn't discuss **AES** or other general considerations about the Cryptography Libraries since these are discussed in an [earlier GitHub sample application](https://github.com/blackberry/Cascades-Community-Samples/tree/master/AESCryptoDemo).

The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

## Limitations

### Hard Wiring

In the interest of simpicity and to aleviate clutter on the UI many of the parameters: such as samples of plaintext, HMAC key, data to be hashed, etc., are hardwired into constants in the code. If you want to experiment with these you should examine the code and change the values there.

## Other Information

### Applies To

* [Cascades for BlackBerry 10](https://developer.blackberry.com/cascades/)
* [BlackBerry Native SDK](http://developer.blackberry.com/native/)

### Author(s)

* [John Murray](https://github.com/jcmurray) - Twitter: @jcmrim

### Release History

* **Version 1.0.0** - Initial version
* **Version 1.1.0** - Added functionality to demonstrate Key Derivation from a shared secret using SHA1-KDF2

### Known Issues

None.

### Dependencies

This sample was developed using the **10.3.1** SDK although it will build correctly on the following:

1. BlackBerry 10 Device Software 10.2.1
1. BlackBerry 10 Native SDK 10.2.1

### How to Build CryptoSampleTool

Simply Import the project into a workspace in your NDK. Of course you should review the project after it's been imported paying particular attention to the settings in the 
**bar-descriptor.xml** file.
 
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