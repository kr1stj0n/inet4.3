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

package inet.queueing.filter;

import inet.queueing.base.PacketFilterBase;

//
// This module implements Shadow Queue (ShQ).
//
// It has n input and n output gates (specified by the 'numGates'
// parameter). Packets arrived at the ith input gate are
// forwarded to the ith output gate, or dropped.
//
// The module sums the used buffer space of the queues attached
// to the output gates. If it is below a minimum threshold,
// the packet won't be dropped, if above a maximum threshold,
// it will be dropped, if it is between the minimum and
// maximum threshold, it will be dropped by a given probability.
// This probability determined by a linear function which is
// 0 at the minth and maxp at maxth.
//
// The queue length can be smoothed by specifying the 'wq'
// parameter. The average queue length used in the tests
// are computed by the formula:
//
//  avg = (1-wq)*avg + wq*qlen
//
// The minth, maxth, and maxp parameters can be specified
// separately for each input gate, so this module can be
// used to implement different packet drop priorities.
//
simple ShQDropper extends PacketFilterBase
{
    parameters:
        string collectionModule = default("");
        double limit = default(1000);                  // max queue capacity
        double interval @unit(s) = default(0.01s);     // interval in sec.
        double maxp = default(0.1);                    // maximum value for pbs
        double alpha = default(0.2);                   // alpha value between [0, 1]
        double datarate @unit(bps) = default(100Mbps); // default bandwidth 100Mbps
        bool useEcn = default(true);                   // enable ECN
        int packetCapacity = default(int(limit));      // (int) max queue capacity
        @class(ShQDropper);
        @display("i=block/downarrow");

        @signal[avgMarkingProb](type=double);
        @statistic[avgMarked](title="avg. marking probability"; source="avgMarkingProb"; record=vector,mean; interpolationmode=none);

        @signal[avgOutputRate](type=double);
        @statistic[avgRate](title="avg. output rate"; source="avgOutputRate"; record=vector,mean; interpolationmode=none);

        @signal[packetMarked](type=inet::Packet);
        @statistic[packetMarked](title="packets marked"; source=packetMarked; record=count,sum(packetBytes),vector(packetBytes); interpolationmode=none);

        @signal[packetDropped](type=inet::Packet);
        @statistic[packetDropCongestion](title="packet drop: congestion"; source=packetDropReasonIsCongestion(packetDropped); record=count,sum(packetBytes),vector(packetBytes); interpolationmode=none);
}
