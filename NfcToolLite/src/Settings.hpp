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
#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

class Settings {

private:
	Settings();
	virtual ~Settings();

public:
	// General constants

	static const char* AppVersion;

	// NFC RTD useful constants

	static const char* NfcRtdSmartPoster;
	static const char* NfcRtdUri;
	static const char* NfcRtdText;

	static const char* LANG_EN;

	static const unsigned int NfcRtdUriPrefixNone;
	static const unsigned int NfcRtdUriPrefixHttpWww;
	static const unsigned int NfcRtdUriPrefixHttpsWww;
	static const unsigned int NfcRtdUriPrefixHttp;
	static const unsigned int NfcRtdUriPrefixHttps;

};

#endif /* SETTINGS_HPP_ */
