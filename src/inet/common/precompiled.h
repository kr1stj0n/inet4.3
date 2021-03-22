//
// Copyright (C) 2020 OpenSim Ltd.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef __INET_PRECOMPILED_H
#define __INET_PRECOMPILED_H

// NOTE: All macros that modify the behavior of an omnet or system header file
// MUST be defined inside this header otherwise the precompiled header will be incorrectly built

// use GNU compatibility (see: https://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html)
// for asprintf() and other functions
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <omnetpp.h>

#endif

