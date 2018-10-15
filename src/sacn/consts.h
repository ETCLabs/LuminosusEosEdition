// Copyright 2016 Tom Barthel-Steer
// http://www.tomsteer.net
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SACN_CONSTS_H
#define SACN_CONSTS_H

#include <QStringList>

#define APP_NAME        "sACNView"
#define AUTHOR          "Tom Barthel-Steer\r\nMarcus Birkin\r\nHans Hinrichsen\r\nMatt Kerr"

// If this is a full release, only show the newer version message for other full releases
// If this is prerelease, show all newer versions
#define PRERELEASE      true

#define nNumOfSecPerHour 3600
#define nNumberOfSecPerMin 60
#define nNumOfMinPerHour 60

#define MIN_SACN_UNIVERSE 1
#define MAX_SACN_UNIVERSE 63999
#define MIN_DMX_ADDRESS 1
#define MAX_DMX_ADDRESS 512
#define PRESET_COUNT 8

#define MIN_SACN_PRIORITY 0
#define MAX_SACN_PRIORITY 200
#define DEFAULT_SACN_PRIORITY 100

#define MIN_SACN_LEVEL 0
#define MAX_SACN_LEVEL 255

#define MAX_SACN_TRANSMIT_TIME_SEC  1000000

#define DEFAULT_SOURCE_NAME "sACNView"
#define MAX_SOURCE_NAME_LEN 63


enum PriorityMode
{
    pmPER_SOURCE_PRIORITY,
    pmPER_ADDRESS_PRIORITY
};

static const QStringList PriorityModeStrings = { "Per-Source", "Per-Address" };


#endif // SACN_CONSTS_H
