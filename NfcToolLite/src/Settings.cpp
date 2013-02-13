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
#include "Settings.hpp"

// General constants

const char* Settings::AppVersion = "1.1.0";

// NFC RTD useful constants

const char* Settings::NfcRtdSmartPoster = "Sp";
const char* Settings::NfcRtdUri = "U";
const char* Settings::NfcRtdText = "T";

const char* Settings::LANG_EN = "en";

const unsigned int Settings::NfcRtdUriPrefixNone = 0x00;
const unsigned int Settings::NfcRtdUriPrefixHttpWww = 0x01;
const unsigned int Settings::NfcRtdUriPrefixHttpsWww = 0x02;
const unsigned int Settings::NfcRtdUriPrefixHttp = 0x03;
const unsigned int Settings::NfcRtdUriPrefixHttps = 0x04;
