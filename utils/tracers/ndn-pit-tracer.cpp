/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/


#include "ndn-pit-tracer.hpp"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/config.h"
#include "ns3/names.h"
#include "ns3/callback.h"

#include "apps/ndn-app.hpp"
#include "ns3/ndnSIM/model/ndn-l3-protocol.hpp"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/node-list.h"
#include "daemon/table/pit-entry.hpp"

#include <boost/lexical_cast.hpp>

#include <fstream>

NS_LOG_COMPONENT_DEFINE("ndn.PitTracer");

namespace ns3 {
namespace ndn {
static std::list<std::tuple<shared_ptr<std::ostream>, std::list<Ptr<PitTracer>>>> g_tracers;

void
PitTracer::Destroy()
{
  g_tracers.clear();
}

void
PitTracer::InstallAll(const std::string& file, Time averagingPeriod /* = Seconds (0.5)*/)
{
  using namespace boost;
  using namespace std;
  
  std::list<Ptr<PitTracer>> tracers;
  shared_ptr<std::ostream> outputStream;
  if (file != "-") {
    shared_ptr<std::ofstream> os(new std::ofstream());
    os->open(file.c_str(), std::ios_base::out | std::ios_base::trunc);

    if (!os->is_open()) {
      NS_LOG_ERROR("File " << file << " cannot be opened for writing. Tracing disabled");
      return;
    }

    outputStream = os;
  }
  else {
    outputStream = shared_ptr<std::ostream>(&std::cout, std::bind([]{}));
  }

  for (NodeList::Iterator node = NodeList::Begin(); node != NodeList::End(); node++) {
    Ptr<PitTracer> trace = Install(*node, outputStream, averagingPeriod);
    tracers.push_back(trace);
  }

  if (tracers.size() > 0) {
    // *m_l3RateTrace << "# "; // not necessary for R's read.table
    tracers.front()->PrintHeader(*outputStream);
    *outputStream << "\n";
  }

  g_tracers.push_back(std::make_tuple(outputStream, tracers));
}

void
PitTracer::Install(const NodeContainer& nodes, const std::string& file,
                      Time averagingPeriod /* = Seconds (0.5)*/)
{
  using namespace boost;
  using namespace std;

  std::list<Ptr<PitTracer>> tracers;
  shared_ptr<std::ostream> outputStream;
  if (file != "-") {
    shared_ptr<std::ofstream> os(new std::ofstream());
    os->open(file.c_str(), std::ios_base::out | std::ios_base::trunc);

    if (!os->is_open()) {
      NS_LOG_ERROR("File " << file << " cannot be opened for writing. Tracing disabled");
      return;
    }

    outputStream = os;
  }
  else {
    outputStream = shared_ptr<std::ostream>(&std::cout, std::bind([]{}));
  }

  for (NodeContainer::Iterator node = nodes.Begin(); node != nodes.End(); node++) {
    Ptr<PitTracer> trace = Install(*node, outputStream, averagingPeriod);
    tracers.push_back(trace);
  }

  if (tracers.size() > 0) {
    // *m_l3RateTrace << "# "; // not necessary for R's read.table
    tracers.front()->PrintHeader(*outputStream);
    *outputStream << "\n";
  }

  g_tracers.push_back(std::make_tuple(outputStream, tracers));
}

void
PitTracer::Install(Ptr<Node> node, const std::string& file,
                      Time averagingPeriod /* = Seconds (0.5)*/)
{
  using namespace boost;
  using namespace std;

  std::list<Ptr<PitTracer>> tracers;
  shared_ptr<std::ostream> outputStream;
  if (file != "-") {
    shared_ptr<std::ofstream> os(new std::ofstream());
    os->open(file.c_str(), std::ios_base::out | std::ios_base::trunc);

    if (!os->is_open()) {
      NS_LOG_ERROR("File " << file << " cannot be opened for writing. Tracing disabled");
      return;
    }

    outputStream = os;
  }
  else {
    outputStream = shared_ptr<std::ostream>(&std::cout, std::bind([]{}));
  }

  Ptr<PitTracer> trace = Install(node, outputStream, averagingPeriod);
  tracers.push_back(trace);

  if (tracers.size() > 0) {
    // *m_l3RateTrace << "# "; // not necessary for R's read.table
    tracers.front()->PrintHeader(*outputStream);
    *outputStream << "\n";
  }

  g_tracers.push_back(std::make_tuple(outputStream, tracers));
}

Ptr<PitTracer>
PitTracer::Install(Ptr<Node> node, shared_ptr<std::ostream> outputStream,
                      Time averagingPeriod /* = Seconds (0.5)*/)
{
  NS_LOG_DEBUG("Node: " << node->GetId());
  Ptr<PitTracer> trace = Create<PitTracer>(outputStream, node);
  trace->SetAveragingPeriod(averagingPeriod);
  return trace;
}

PitTracer::PitTracer(shared_ptr<std::ostream> os, Ptr<Node> node)
  : m_nodePtr(node)
  , m_os(os)
{
  m_node = boost::lexical_cast<std::string>(m_nodePtr->GetId());
  Connect();

  std::string name = Names::FindName(node);
  if (!name.empty()) {
    m_node = name;
  }
}

PitTracer::PitTracer(shared_ptr<std::ostream> os, const std::string& node)
  : m_node(node)
  , m_os(os)
{
  Connect();
}

PitTracer::~PitTracer()
{
  m_printEvent.Cancel();
}

void
PitTracer::SetAveragingPeriod(const Time& period)
{
  m_period = period;
  m_printEvent.Cancel();
  m_printEvent = Simulator::Schedule(m_period, &PitTracer::PeriodicPrinter, this);
}

void
PitTracer::PeriodicPrinter()
{
  Print(*m_os);
  Reset();

  m_printEvent = Simulator::Schedule(m_period, &PitTracer::PeriodicPrinter, this);
}

void
PitTracer::PrintHeader(std::ostream& os) const
{
  os << "Time"
     << "\t"

     << "Node"
     << "\t"

     << "PitSize"
     << "\t"
     << "PitTimeOut"
     << "\t"	 
     << "PitDrop"
	 << "\t"

     << "Entropy"
     << "\t"
     << "Cusum"
	 ;
}

void
PitTracer::Reset()
{
	m_stats.Reset();
}

#define PRINTER(fieldName1, fieldName2, fieldName3, fieldName4, fieldName5)                                                              \
  os << time.ToDouble(Time::S) << "\t" << m_node << "\t";                                          \
  os << m_stats.fieldName1<< "\t" << m_stats.fieldName2 << "\t" << m_stats.fieldName3<< "\t" << m_stats.fieldName4 << "\t" << m_stats.fieldName5 <<"\n";

void
PitTracer::Print(std::ostream& os) const
{
  Time time = Simulator::Now();
  
  PRINTER(m_pitSize, m_pitTimeOut, m_pitDrop, m_entropy, m_cusumEnp);
}


void
PitTracer::enpInfo(const nfd::Entropy& m_enp)
{
	m_stats.m_entropy = m_enp.newEnp;
	m_stats.m_cusumEnp = m_enp.cumulateEntropy;
}

void
PitTracer::pitSize(uint32_t s)
{
	m_stats.m_pitSize = s;
}

void
PitTracer::pitDrop(uint32_t s)
{
	m_stats.m_pitDrop++;
}

void
PitTracer::pitTimeOut(uint32_t s)
{
	m_stats.m_pitTimeOut++;
}
  
  
void
PitTracer::Connect()
{
  Ptr<L3Protocol> l3 = m_nodePtr->GetObject<L3Protocol>();
  // satisfied/timed out PIs
  l3->TraceConnectWithoutContext("enpInfo", MakeCallback(&PitTracer::enpInfo, this));
  l3->TraceConnectWithoutContext("pitSize", MakeCallback(&PitTracer::pitSize, this));
  l3->TraceConnectWithoutContext("pitDrop", MakeCallback(&PitTracer::pitDrop, this));
  l3->TraceConnectWithoutContext("pitTimeOut", MakeCallback(&PitTracer::pitTimeOut, this));
}

} // namespace ndn
} // namespace ns3
