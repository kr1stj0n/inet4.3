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

#include "inet/queueing/filter/ShQDropper.h"

#include "inet/common/ModuleAccess.h"
#include "inet/queueing/marker/EcnMarker.h"

namespace inet {
namespace queueing {

Define_Module(ShQDropper);

void ShQDropper::initialize(int stage)
{
    PacketFilterBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        limit = par("limit");
        if (limit < 0.0)
            throw cRuntimeError("limit parameter must not be negative");
        interval = par("interval");
        if (interval < 0.0)
            throw cRuntimeError("interval parameter must not be negative");
        maxp = par("maxp");
        if (maxp < 0.0 || maxp > 1.0)
            throw cRuntimeError("Invalid value for maxp parameter: %g", maxp);
        alpha = par("alpha");
        if (alpha < 0.0 || alpha > 1.0)
            throw cRuntimeError("Invalid value for alpha parameter: %g", alpha);
        pkrate = par("pkrate");
        if (pkrate < 0.0)
            throw cRuntimeError("Invalid value for pkrate parameter: %g", pkrate);
        useEcn = par("useEcn");
        packetCapacity = par("packetCapacity");
        auto outputGate = gate("out");
        collection = findConnectedModule<IPacketCollection>(outputGate);
        if (collection == nullptr)
            collection = getModuleFromPar<IPacketCollection>(par("collectionModule"), this);

        packetMarkedSignal   = registerSignal("packetMarked");
        markingProbSignal    = registerSignal("markingProb");
        avgMarkingProbSignal = registerSignal("avgMarkingProb");
        avgOutputRateSignal  = registerSignal("avgOutputRate");
    }
}

ShQDropper::ShQResult ShQDropper::doRandomEarlyDetection(const Packet *packet)
{
    double pb = 0.0;
    int queueLength = collection->getNumPackets();

    if (queueLength >= packetCapacity) { // maxth is also the "hard" limit
        EV_INFO << "Queue length >= capacity" << EV_FIELD(queueLength) << EV_FIELD(packetCapacity) << EV_ENDL;
        return QUEUE_FULL;
    }

    curRate += (double) packet->getByteLength();

    const simtime_t now = simTime();
    if (now >= r_time + interval) {
        double duration = SIMTIME_DBL(now - r_time);
        curRate += queueLength * 1500;
        avgRate = (1.0 - alpha) * avgRate + alpha * curRate;

        pb = maxp * (avgRate / (pkrate * duration));
        if (pb < 0.0)
            pb = 0;
        else if (pb >= maxp)
            pb = 1.0;

        emit(avgMarkingProbSignal, pb);
        emit(avgOutputRateSignal, avgRate * (SIMTIME_DBL(interval) / duration));

        r_time = simTime();
        curRate = 0;
    }

    if (dblrand() < pb) {
        emit(packetMarkedSignal, packet);
        return RANDOMLY_MARK;
    } else {
        return RANDOMLY_NOT_MARK;
    }
}

bool ShQDropper::matchesPacket(const Packet *packet) const
{
    lastResult = const_cast<ShQDropper *>(this)->doRandomEarlyDetection(packet);
    switch (lastResult) {
        case RANDOMLY_MARK:
            return useEcn && EcnMarker::getEcn(packet) != IP_ECN_NOT_ECT;
        case RANDOMLY_NOT_MARK:
            return true;
        case QUEUE_FULL:
            return false;
        default:
            throw cRuntimeError("Unknown ShQ result");
    }
}

void ShQDropper::processPacket(Packet *packet)
{
    switch (lastResult) {
        case RANDOMLY_MARK: {
            if (useEcn) {
                IpEcnCode ecn = EcnMarker::getEcn(packet);
                if (ecn != IP_ECN_NOT_ECT) {
                    // if next packet should be marked and it is not
                    if (markNext && ecn != IP_ECN_CE) {
                        EcnMarker::setEcn(packet, IP_ECN_CE);
                        markNext = false;
                    }
                    else {
                        if (ecn == IP_ECN_CE)
                            markNext = true;
                        else
                            EcnMarker::setEcn(packet, IP_ECN_CE);
                    }
                }
            }
        }
        case RANDOMLY_NOT_MARK:
        case QUEUE_FULL:
            return;
        default:
            throw cRuntimeError("Unknown ShQ result");
    }
}

void ShQDropper::pushOrSendPacket(Packet *packet, cGate *gate,
                                  IPassivePacketSink *consumer)
{
    PacketFilterBase::pushOrSendPacket(packet, gate, consumer);
}

} // namespace queueing
} // namespace inet

