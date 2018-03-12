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
 * Authors: Spyridon (Spyros) Mastorakis <mastorakis@cs.ucla.edu>
 *          Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "ntorrent-consumer-app.hpp"

NS_LOG_COMPONENT_DEFINE("NTorrentConsumerApp");
namespace ndn_ntorrent = ndn::ntorrent;

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(NTorrentConsumerApp);

TypeId
NTorrentConsumerApp::GetTypeId(void)
{
    static TypeId tid = TypeId("NTorrentConsumerApp")
      .SetParent<Application>()
      .AddConstructor<NTorrentConsumerApp>()
      .AddAttribute("StartSeq", "Initial sequence number", IntegerValue(0),
                    MakeIntegerAccessor(&NTorrentConsumerApp::m_seq), MakeIntegerChecker<int32_t>())

      .AddAttribute("Prefix", "Name of the Interest", StringValue("/"),
                    MakeNameAccessor(&NTorrentConsumerApp::m_interestName), MakeNameChecker())
      .AddAttribute("LifeTime", "LifeTime for interest packet", StringValue("1s"),
                    MakeTimeAccessor(&NTorrentConsumerApp::m_interestLifeTime), MakeTimeChecker());
    return tid;
}

NTorrentConsumerApp::NTorrentConsumerApp()
{
}

NTorrentConsumerApp::~NTorrentConsumerApp()
{
}

void
NTorrentConsumerApp::StartApplication()
{
    ndn::App::StartApplication();
    ndn::FibHelper::AddRoute(GetNode(), "/", m_face, 0);
    
    /*for(int i=0;i<5;i++)
        Simulator::Schedule(Seconds(i+1.0), &NTorrentConsumerApp::SendInterest, this);*/

    //std::string torrentName = ndn_ntorrent::DUMMY_FILE_PATH+"torrent-file";
    std::string torrentName = "/ndn/nTorrent/NTORRENT/dummy/torrent-file/sha256digest=10add2be3d0e56a239a78b56e3f676bf2c733c22d4c2a64ded64d9a22f4bd5ab";
    SendInterest(torrentName);
}

void
NTorrentConsumerApp::StopApplication()
{
    ndn::App::StopApplication();
}

void
NTorrentConsumerApp::SendInterest()
{
  //Just for testing, not really going to be used.. 
  auto interest = std::make_shared<Interest>(std::string(ndn_ntorrent::SharedConstants::commonPrefix) + "/NTORRENT/" + to_string(m_seq++));
  Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();
  interest->setNonce(rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
  interest->setInterestLifetime(ndn::time::seconds(1));
  NS_LOG_DEBUG("Sending Interest packet for " << *interest);
  m_transmittedInterests(interest, this, m_face);
  m_appLink->onReceiveInterest(*interest);
}

void
NTorrentConsumerApp::SendInterest(const string& interestName)
{
  auto interest = std::make_shared<Interest>(interestName);
  Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();
  interest->setNonce(rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
  interest->setInterestLifetime(ndn::time::seconds(1));
  NS_LOG_DEBUG("Sending Interest packet for " << *interest);
  m_transmittedInterests(interest, this, m_face);
  m_appLink->onReceiveInterest(*interest);
}

void
NTorrentConsumerApp::OnInterest(std::shared_ptr<const Interest> interest)
{
  //We don't have to process interests on the consumer-end for now
}

void
NTorrentConsumerApp::OnData(std::shared_ptr<const Data> data)
{
    //TODO: Use switch statement similar to that in the producer
    NS_LOG_DEBUG("Received: " << data->getName());
    ndn_ntorrent::TorrentFile file(data->wireEncode());
    std::vector<Name> manifestCatalog = file.getCatalog();
    manifests.insert(manifests.end(), manifestCatalog.begin(), manifestCatalog.end());
    NS_LOG_DEBUG("Manifests: " << manifests.size());
    shared_ptr<Name> nextSegmentPtr = file.getTorrentFilePtr();
    if(nextSegmentPtr!=nullptr){
        NS_LOG_DEBUG("Wait.. There's more...");
        NS_LOG_DEBUG(*file.getTorrentFilePtr());
        SendInterest(nextSegmentPtr.get()->toUri());
    }
    else
    {
        //TODO: Start requesting the data...
        NS_LOG_DEBUG("W00t! Torrent file is done!");
    }
}

} // namespace ndn
} // namespace ns3
