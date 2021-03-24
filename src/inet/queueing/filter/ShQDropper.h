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

#ifndef __INET_SHQDROPPER_H
#define __INET_SHQDROPPER_H

#include "inet/common/packet/Packet.h"
#include "inet/queueing/base/PacketFilterBase.h"
#include "inet/queueing/contract/IPacketCollection.h"

namespace inet {
namespace queueing {

/**
 * Implementation of Shadow Queue (ShQ).
 */
class INET_API ShQDropper : public PacketFilterBase
{
  protected:
    enum ShQResult { QUEUE_FULL, RANDOMLY_MARK, RANDOMLY_NOT_MARK };

  protected:
    double limit    = 0.0;
    simtime_t interval;
    double maxp     = NaN;
    double alpha    = NaN;
    double pkrate   = NaN;
    bool useEcn     = true;

    double avgRate  = 0.0;
    double curRate  = 0.0;
    simtime_t r_time;

    int packetCapacity = -1;
    bool markNext      = false;
    mutable ShQResult lastResult;

    simsignal_t markingProbSignal;
    simsignal_t avgMarkingProbSignal;
    simsignal_t avgOutputRateSignal;

    IPacketCollection *collection = nullptr;

  protected:
    virtual void initialize(int stage) override;
    virtual ShQResult doRandomEarlyDetection(const Packet *packet);
    virtual void processPacket(Packet *packet) override;
    virtual void pushOrSendPacket(Packet *packet, cGate *gate, IPassivePacketSink *consumer) override;

  public:
    virtual bool matchesPacket(const Packet *packet) const override;
};

} // namespace queueing
} // namespace inet

#endif
