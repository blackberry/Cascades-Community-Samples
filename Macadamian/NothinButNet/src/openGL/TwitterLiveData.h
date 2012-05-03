/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
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

#ifndef TWITTERLIVEDATA_H_
#define TWITTERLIVEDATA_H_

class TwitterLiveData {
public:
	static int teamSetIndex;
	TwitterLiveData();
	static void setTeamSetIndex(int setIndex);
	static const char** handles();
	static const float (*color())[3];
	static const float *data(int teamIndex);
	static float spline(int teamIndex,float x);
	virtual ~TwitterLiveData();
};

#endif /* TWITTERLIVEDATA_H_ */
