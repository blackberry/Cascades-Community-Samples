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
	static const char* DOMAIN;
	static const char* TYPE;
	static const char* CONTENT;
	static const char* SP_URI;
	static const char* SP_TEXT;
	static const char* TEXT;
	static const char* URI;
	static const char* FIRST_NAME;
	static const char* LAST_NAME;
	static const char* ADDRESS;
	static const char* EMAIL;
	static const char* MOBILE;
	static const char* EMULATE_URI;
	static const char* EMULATE_TEXT;
	static const char* HEX_CLA;
	static const char* HEX_INS;
	static const char* HEX_P1P2;
	static const char* HEX_LC;
	static const char* HEX_COMMAND;
	static const char* HEX_LE;
	static const char* AID;

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
