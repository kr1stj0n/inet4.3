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

#include "inet/transportlayer/tcp/flavours/LgcFamily.h"

#include "inet/transportlayer/tcp/Tcp.h"

namespace inet {
namespace tcp {

LgcFamilyStateVariables::LgcFamilyStateVariables()
{
    lgc_alpha       = 0.2;
    lgc_phi         = 2.78;
    lgc_logP        = 1.4;
    lgc_coef        = 20;
    lgc_datarate    = 100000000; // 100Mbps
    lgc_rate        = 0;
    lgc_cntr        = 0;
    lgc_windEnd     = snd_una;
    lgc_bytesMarked = 0;
    lgc_bytesAcked  = 0;
    lgc_fraction    = 0.0;
}

std::string LgcFamilyStateVariables::str() const
{
    std::stringstream out;
    out << TcpTahoeRenoFamilyStateVariables::str();
    out << " lgc_alpha=" << lgc_alpha;
    out << " lgc_phi=" << lgc_phi;
    out << " lgc_logP=" << lgc_logP;
    out << " lgc_coef=" << lgc_coef;
    out << " lgc_datarate=" << lgc_datarate;
    out << " lgc_rate=" << lgc_rate;
    out << " lgc_windEnd=" << lgc_windEnd;
    out << " lgc_bytesAcked=" << lgc_bytesAcked;
    out << " lgc_bytesMarked=" << lgc_bytesMarked;
    out << " lgc_fraction=" << lgc_fraction;

    return out.str();
}

std::string LgcFamilyStateVariables::detailedInfo() const
{
    std::stringstream out;
    out << TcpTahoeRenoFamilyStateVariables::str();
    out << " lgc_alpha=" << lgc_alpha;
    out << " lgc_phi=" << lgc_phi;
    out << " lgc_logP=" << lgc_logP;
    out << " lgc_coef=" << lgc_coef;
    out << " lgc_datarate=" << lgc_datarate;
    out << " lgc_rate=" << lgc_rate;
    out << " lgc_windEnd=" << lgc_windEnd;
    out << " lgc_bytesAcked=" << lgc_bytesAcked;
    out << " lgc_bytesMarked=" << lgc_bytesMarked;
    out << " lgc_fraction=" << lgc_fraction;
    return out.str();
}

// ---

LgcFamily::LgcFamily() : TcpTahoeRenoFamily(),
    state((LgcFamilyStateVariables *&)TcpTahoeRenoFamily::state)
{
}

} // namespace tcp
} // namespace inet

