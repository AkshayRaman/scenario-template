/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Akshay Raman <akshay.raman@cs.ucla.edu>
 */

#ifndef INCLUDED_SIMULATION_COMMON_HPP
#define INCLUDED_SIMULATION_COMMON_HPP

#include <cmath>

#include "ns3/core-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include "../extensions/ntorrent-consumer-app.hpp"
#include "../extensions/ntorrent-producer-app.hpp"
#include "../extensions/ntorrent-fwd-strategy.hpp"
#include "src/util/shared-constants.hpp"

#define PI 3.14159

namespace ndn_ntorrent = ndn::ntorrent;
namespace ndn{
namespace ntorrent{
const char * ndn_ntorrent::SharedConstants::commonPrefix = "";
}
}

namespace ns3 {
namespace ndn {


/*
 * @brief replace a lot of code in the scenarios with a single line.
 * 
 * @param namesPerSegment Number of names per segment (received as command line argument)
 * @param namesPerManifest Number of names per manifest (received as command line argument)
 * @param dataPacketSize Size of each data packet (received as command line argument)
 * @param type Can be "producer" or "consumer"
 * @param n Node pointer
 * @param startTime Time after which this node begins simulation
 *
 * ndn::AppHelper producerHelper("NTorrentProducerApp");
 * producerHelper.SetAttribute("Prefix", StringValue("/"));
 * producerHelper.SetAttribute("namesPerSegment", IntegerValue(namesPerSegment));
 * producerHelper.SetAttribute("namesPerManifest", IntegerValue(namesPerManifest));
 * producerHelper.SetAttribute("dataPacketSize", IntegerValue(dataPacketSize));
 * producerHelper.Install(nodes.Get(0)).Start(Seconds(1.0));
 *
 * will be replaced by
 * 
 * ndn::AppHelper p1("NTorrentProducerApp");
 * createAndInstall(p1, namesPerSegment, namesPerManifest, dataPacketSize, "producer", nodes.Get(0), 1.0f);
 *
 */

void createAndInstall(ndn::AppHelper x, uint32_t namesPerSegment, 
        uint32_t namesPerManifest, uint32_t dataPacketSize, std::string type, 
        Ptr<Node> n, float startTime)
{
  x.SetAttribute("Prefix", StringValue("/"));
  x.SetAttribute("namesPerSegment", IntegerValue(namesPerSegment));
  x.SetAttribute("namesPerManifest", IntegerValue(namesPerManifest));
  x.SetAttribute("dataPacketSize", IntegerValue(dataPacketSize));
  x.Install(n).Start(Seconds(startTime));
}

//Helper functions to create links...
void createLink(PointToPointHelper p2p, Ptr<Node> n1, Ptr<Node> n2)
{
    p2p.Install(n1, n2);
}

//Helper functions to create links with data rate and delay
void createLink(PointToPointHelper p2p, Ptr<Node> n1, Ptr<Node> n2, std::string dataRate, std::string delay)
{
    p2p.SetDeviceAttribute ("DataRate", StringValue(dataRate));
    p2p.SetChannelAttribute ("Delay", StringValue(delay));
    p2p.Install(n1, n2);
}

} //namespace ndn
} //namespace ns3

#endif // INCLUDED_SIMULATION_COMMON_HPP
