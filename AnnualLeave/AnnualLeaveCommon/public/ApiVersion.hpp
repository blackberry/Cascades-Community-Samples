/*
 * Copyright (c) 2014 BlackBerry Limited.
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

#ifndef APIVERSION_HPP_
#define APIVERSION_HPP_

/*
 * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE *NOTE * NOTE
 *
 * This variable establishes the API version of the current build
 * It should be changed only when an interface change that would
 * impact a 3rd party developer of an adapter using the SDK would
 * be affected. It should be incremented when each release that alters the
 * API is made public.
 *
 * Adapter developers are able to able to check the API level
 * they're building against at compile time using constructs such as:
 *
 *		#if ANNUAL_LEAVE_API_LEVEL >12
 *			// do something API version dependent on API level 12
 *		#endif
 *
 * Level 1 => introduced in Beta 2
 * Level 2 => introduced in Beta 3
 *
 * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE *NOTE * NOTE
 */

#define ANNUAL_LEAVE_API_LEVEL_BETA_2 1
#define ANNUAL_LEAVE_API_LEVEL_BETA_3 2

#define ANNUAL_LEAVE_API_LEVEL ANNUAL_LEAVE_API_LEVEL_BETA_3

#endif /* APIVERSION_HPP_ */
