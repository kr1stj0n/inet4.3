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

#include "inet/transportlayer/tcp/flavours/Lgc.h"

#include <algorithm> // min,max

#include "inet/transportlayer/tcp/Tcp.h"

namespace inet {
namespace tcp {

Register_Class(Lgc);

// will record load
simsignal_t Lgc::loadSignal = cComponent::registerSignal("load");
// will record total number of RTOs
simsignal_t Lgc::calcLoadSignal = cComponent::registerSignal("calcLoad");
// will record marking probability
simsignal_t Lgc::markingProbSignal = cComponent::registerSignal("markingProb");

Lgc::Lgc() : TcpReno(),
    state((LgcStateVariables *&)TcpAlgorithm::state)
{
}

void Lgc::initialize()
{
    TcpReno::initialize();
    state->lgc_alpha = conn->getTcpMain()->par("lgcAlpha");
    state->lgc_phi = conn->getTcpMain()->par("lgcPhi");
    state->lgc_logP = conn->getTcpMain()->par("lgcLogP");
    state->lgc_coef = conn->getTcpMain()->par("lgcCoef");
    state->lgc_datarate = conn->getTcpMain()->par("lgcDataRate");

    state->lgc_cntr = 0;
}

void Lgc::LgcReset()
{
    state->lgc_nextSeq = state->snd_nxt;
    state->lgc_bytesMarked = 0;
    state->lgc_bytesAcked = 0;
}

// Same as TcpReno
void Lgc::recalculateSlowStartThreshold()
{
    // RFC 2581, page 4:
    // "When a TCP sender detects segment loss using the retransmission
    // timer, the value of ssthresh MUST be set to no more than the value
    // given in equation 3:
    //
    //   ssthresh = max (FlightSize / 2, 2*SMSS)            (3)
    //
    // As discussed above, FlightSize is the amount of outstanding data in
    // the network."

    // set ssthresh to flight size/2, but at least 2 SMSS
    // (the formula below practically amounts to ssthresh=cwnd/2 most of the time)
    uint32_t flight_size = std::min(state->snd_cwnd, state->snd_wnd); // FIXME - Does this formula computes the amount of outstanding data?
//    uint32_t flight_size = state->snd_max - state->snd_una;
    state->ssthresh = std::max(flight_size / 2, 2 * state->snd_mss);
    conn->emit(ssthreshSignal, state->ssthresh);
}

void Lgc::LgcProcessRateUpdate()
{
    if (state->lgc_cntr == 0) {
        state->lgc_rate = (state->snd_cwnd * 8 / (double)state->srtt.dbl());
        if (rateVector && simTime() >= conn->tcpMain->par("param3"))
            rateVector->record(state->ecnnum_Rate);

        state->ecnnum_cntr++;
    }

    if (state->lgc_bytesMarked / state->lgc_bytesAcked >= 0.9)
        state->lgc_fraction = (1 - state->lgc_alpha) * state->lgc_fraction +
            state->lgc_alpha * (state->lgc_bytesMarked / state->lgc_bytesAcked);
    else
        state->lgc_fraction = (1 - state->lgc_alpha) * state->lgc_fraction +
            state->lgc_alpha * 0;

    if(state->lgc_fraction == 1)
        state->lgc_fraction = 0.999;

    /* if (loadVector && simTime() >= conn->tcpMain->par("param3")) */
    /*     loadVector->record(state->dctcp_marked / state->dctcp_total); */
    /* if (calcLoadVector && simTime() >= conn->tcpMain->par("param3")) */
    /*     calcLoadVector->record(state->ecnnum_fraction); */

    {
        double q = ( -log(1-state->lgc_fraction)  / log(state->lgc_phi));

        double gradient = (1 - state->lgc_rate / state->lgc_datarate - q );
        double gr = pow(coef, -(state->lgc_bytesMarked / state->lgc_bytesAcked)
                        * log(logP));

        double newRate = state->lgc_rate + gr * state->lgc_rate * gradient;
        if(newRate > 2 * state->lgc_rate)
            state->lgc_rate *= 2;
        else
            state->lgc_rate = newRate;
    }

    if (state->lgc_rate <= 0)
        state->lgc_rate = 0.01;

    if (state->lgc_rate > state->lgc_datarate)
        state->lgc_rate = state->lgc_datarate;

    uint32_t newCwnd = state->snd_cwnd;
    newCwnd = state->lgc_rate * (double)state->srtt.dbl() / 8;
    newCwnd = round(newCwnd / state->snd_mss) * state->snd_mss;
    if (newCwnd < state->snd_mss)
        newCwnd = state->snd_mss;


    state->snd_cwnd = newCwnd;
    conn->emit(cwndSignal, state->snd_cwnd);


    /* if (cwndVector) */
    /*     cwndVector->record(state->snd_cwnd); */
    /* if (rateVector && simTime() >= conn->tcpMain->par("param3")) */
    /*     rateVector->record(state->ecnnum_Rate); */
    /* if (brVector) */
    /*         brVector->record(state->lgcc_r); */

    LgcReset();
}

void Lgc::receivedDataAck(uint32_t firstSeqAcked)
{
    TcpTahoeRenoFamily::receivedDataAck(firstSeqAcked);

    if (state->dupacks >= state->dupthresh) {
        //
        // Perform Fast Recovery: set cwnd to ssthresh (deflating the window).
        //
        EV_INFO << "Fast Recovery: setting cwnd to ssthresh=" << state->ssthresh << "\n";
        state->snd_cwnd = state->ssthresh;

        conn->emit(cwndSignal, state->snd_cwnd);
    }
    else {
        bool performSsCa = true; // Stands for: "perform slow start and congestion avoidance"
        if (state && state->ect) {
            // RFC 8257 3.3.1
            uint32_t bytes_acked = state->snd_una - firstSeqAcked;

            // bool cut = false; TODO unused?

            // RFC 8257 3.3.2
            state->lgc_bytesAcked += bytes_acked;

            // RFC 8257 3.3.3
            if (state->gotEce) {
                state->lgc_bytesMarked += bytes_acked;
                conn->emit(markingProbSignal, 1);
            }
            else {
                conn->emit(markingProbSignal, 0);
            }

            // RFC 8257 3.3.4
            if (state->snd_una > state->lgc_windEnd) {
                LgcProcessRateUpdate();
                LgcReset();
            }
        }

        if (performSsCa) {
            // If ECN is not enabled or if ECN is enabled and received multiple ECE-Acks in
            // less than RTT, then perform slow start and congestion avoidance.

            if (state->snd_cwnd < state->ssthresh) {
                EV_INFO << "cwnd <= ssthresh: Slow Start: increasing cwnd by one SMSS bytes to ";

                // perform Slow Start. RFC 2581: "During slow start, a TCP increments cwnd
                // by at most SMSS bytes for each ACK received that acknowledges new data."
                state->snd_cwnd += state->snd_mss;

                conn->emit(cwndSignal, state->snd_cwnd);
                conn->emit(ssthreshSignal, state->ssthresh);

                EV_INFO << "cwnd=" << state->snd_cwnd << "\n";
            }
            else {
                // perform Congestion Avoidance (RFC 2581)
                uint32_t incr = state->snd_mss * state->snd_mss / state->snd_cwnd;

                if (incr == 0)
                    incr = 1;

                state->snd_cwnd += incr;

                conn->emit(cwndSignal, state->snd_cwnd);
                conn->emit(ssthreshSignal, state->ssthresh);

                //
                // Note: some implementations use extra additive constant mss / 8 here
                // which is known to be incorrect (RFC 2581 p5)
                //
                // Note 2: RFC 3465 (experimental) "Appropriate Byte Counting" (ABC)
                // would require maintaining a bytes_acked variable here which we don't do
                //

                EV_INFO << "cwnd > ssthresh: Congestion Avoidance: increasing cwnd linearly, to " << state->snd_cwnd << "\n";
            }
        }
    }

    if (state->sack_enabled && state->lossRecovery) {
        // RFC 3517, page 7: "Once a TCP is in the loss recovery phase the following procedure MUST
        // be used for each arriving ACK:
        //
        // (A) An incoming cumulative ACK for a sequence number greater than
        // RecoveryPoint signals the end of loss recovery and the loss
        // recovery phase MUST be terminated.  Any information contained in
        // the scoreboard for sequence numbers greater than the new value of
        // HighACK SHOULD NOT be cleared when leaving the loss recovery
        // phase."
        if (seqGE(state->snd_una, state->recoveryPoint)) {
            EV_INFO << "Loss Recovery terminated.\n";
            state->lossRecovery = false;
        }
        // RFC 3517, page 7: "(B) Upon receipt of an ACK that does not cover RecoveryPoint the
        // following actions MUST be taken:
        //
        // (B.1) Use Update () to record the new SACK information conveyed
        // by the incoming ACK.
        //
        // (B.2) Use SetPipe () to re-calculate the number of octets still
        // in the network."
        else {
            // update of scoreboard (B.1) has already be done in readHeaderOptions()
            conn->setPipe();

            // RFC 3517, page 7: "(C) If cwnd - pipe >= 1 SMSS the sender SHOULD transmit one or more
            // segments as follows:"
            if (((int)state->snd_cwnd - (int)state->pipe) >= (int)state->snd_mss) // Note: Typecast needed to avoid prohibited transmissions
                conn->sendDataDuringLossRecoveryPhase(state->snd_cwnd);
        }
    }

    // RFC 3517, pages 7 and 8: "5.1 Retransmission Timeouts
    // (...)
    // If there are segments missing from the receiver's buffer following
    // processing of the retransmitted segment, the corresponding ACK will
    // contain SACK information.  In this case, a TCP sender SHOULD use this
    // SACK information when determining what data should be sent in each
    // segment of the slow start.  The exact algorithm for this selection is
    // not specified in this document (specifically NextSeg () is
    // inappropriate during slow start after an RTO).  A relatively
    // straightforward approach to "filling in" the sequence space reported
    // as missing should be a reasonable approach."
    sendData(false);
}

} // namespace tcp
} // namespace inet

