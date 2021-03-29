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

#ifndef __INET_LGC_H
#define __INET_LGC_H

#include "inet/transportlayer/tcp/flavours/LgcFamily.h"
#include "inet/transportlayer/tcp/flavours/TcpReno.h"

namespace inet {
namespace tcp {

/**
 * State variables for Lgc.
 */
typedef LgcFamilyStateVariables LgcStateVariables;

/**
 * Implements LGC.
 */
class INET_API Lgc : public TcpReno
{
  protected:
    LgcStateVariables *& state;

    static simsignal_t loadSignal; // will record load
    static simsignal_t calcLoadSignal; // will record total number of RTOs
    static simsignal_t markingProbSignal; // will record marking probability

    /** Create and return a DcTcpStateVariables object. */
    virtual TcpStateVariables *createStateVariables() override
    {
        return new LgcStateVariables();
    }

    virtual void initialize() override;

  public:
    /** Constructor */
    Lgc();

    virtual void recalculateSlowStartThreshold() override;

    /** Redefine what should happen when data got acked,
     * to add congestion window management */
    virtual void receivedDataAck(uint32_t firstSeqAcked) override;
    void LgcProcessRateUpdate();
    void LgcReset();
};

} // namespace tcp
} // namespace inet

#endif

