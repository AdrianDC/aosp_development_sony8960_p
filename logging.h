/*
 * Copyright (C) 2015, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AIDL_LOGGING_H_
#define AIDL_LOGGING_H_

// We must include windows.h before android-base/logging.h on Windows.
#ifdef _WIN32
//#include <windows.h>// TODO This suddenly caused a weird Error (enum started failing in auto-gen lex file) and I commented it out. Why? (It may be related that Va_start() suddenly became undeclared though I hadn't touched that code in days.) -- CJP
#endif

#include <android-base/logging.h>

#endif // AIDL_LOGGING_H_
