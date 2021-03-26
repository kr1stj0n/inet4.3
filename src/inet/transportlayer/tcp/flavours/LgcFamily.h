//
// Copyright (C) 2020 Marcel Marek
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#ifndef __INET_LGCFAMILY_H
#define __INET_LGCFAMILY_H

#include "inet/transportlayer/tcp/flavours/TcpTahoeRenoFamily.h"

namespace inet {
namespace tcp {

/**
 * State variables for Lgc.
 */
class INET_API LgcFamilyStateVariables : public TcpTahoeRenoFamilyStateVariables
{
  public:
    LgcFamilyStateVariables();
    virtual std::string str() const override;
    virtual std::string detailedInfo() const override;

    // LGC
    // TODO
    bool dctcp_ce;
    uint32_t dctcp_windEnd;
    uint32_t dctcp_bytesAcked;
    uint32_t dctcp_bytesMarked; // amount of bytes marked
    double dctcp_alpha;
    double dctcp_gamma;
};

/**
 * Provides utility functions to implement Lgc.
 */
class INET_API LgcFamily : public TcpTahoeRenoFamily
{
  protected:
    LgcFamilyStateVariables *& state; // alias to TcpAlgorithm's 'state'

  public:
    /** Ctor */
    LgcFamily();
};

} // namespace tcp
} // namespace inet

#endif

