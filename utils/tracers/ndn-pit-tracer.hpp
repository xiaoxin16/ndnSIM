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

#ifndef NDN_PIT_TRACER_H
#define NDN_PIT_TRACER_H

#include "ns3/ndnSIM/model/ndn-common.hpp"
#include "ns3/ndnSIM/NFD/daemon/table/pit.hpp"
#include "ns3/ptr.h"
#include "ns3/simple-ref-count.h"
#include "ns3/nstime.h"
#include "ns3/event-id.h"
#include "ns3/node-container.h"

#include "src/ndnSIM/utils/ndnsim-globe.hpp"
#include "src/ndnSIM/utils/entropy/ndn-entropy.hpp"
//#include "ns3/ndnSIM/NFD/daemon/fw/ndn-entropy.hpp"

#include <tuple>
#include <map>
#include <list>

namespace nfd {
class Pit;
namespace pit {
class Entry;
} // namespace pit
} // namespace nfd

namespace ns3 {

class Node;
class Packet;

namespace ndn {
	
namespace pit{
	struct Stats {
    inline void
    Reset()
    {
		m_pitSize = 0;
		m_pitDrop = 0;
		m_pitTimeOut = 0;
		m_entropy = 0;
		m_cusumEnp = 0;
    }
	uint32_t m_pitSize = 0;
	uint32_t m_pitDrop = 0;
	uint32_t m_pitTimeOut = 0;
	double m_entropy;
	double m_cusumEnp;
  };
}

/**
 * @ingroup ndn-tracers
 * @brief Base class for network-layer (incoming/outgoing Interests and Data) tracing of NDN stack
 */
class PitTracer : public SimpleRefCount<PitTracer> {
public:
  static void
  InstallAll(const std::string& file, Time averagingPeriod = Seconds(0.5));
  static void
  Install(const NodeContainer& nodes, const std::string& file, Time averagingPeriod = Seconds(0.5));
  static void
  Install(Ptr<Node> node, const std::string& file, Time averagingPeriod = Seconds(0.5));
  static Ptr<PitTracer>
  Install(Ptr<Node> node, shared_ptr<std::ostream> outputStream,
          Time averagingPeriod = Seconds(0.5));
  static void
  Destroy();
  PitTracer(shared_ptr<std::ostream> os, Ptr<Node> node);
  PitTracer(shared_ptr<std::ostream> os, const std::string& node);
  ~PitTracer();
  void
  PrintHeader(std::ostream& os) const;
  void
  Print(std::ostream& os) const;

protected:
  void
  Connect();

  void
  pitSize(uint32_t s);   // size
  void
  pitDrop(uint32_t s);           // Drop
  void
  pitTimeOut(uint32_t s);        // TimeOut
  void
  enpInfo(const nfd::Entropy&);
  
private:
  void
  SetAveragingPeriod(const Time& period);

  void
  PeriodicPrinter();

  void
  Reset();

private:
  std::string m_node;
  Ptr<Node> m_nodePtr;
  
  shared_ptr<std::ostream> m_os;
  
  Time m_period;
  EventId m_printEvent;
  pit::Stats m_stats;
};

inline std::ostream&
operator<<(std::ostream& os, const PitTracer& tracer)
{
  os << "# ";
  tracer.PrintHeader(os);
  os << "\n";
  tracer.Print(os);
  return os;
}

} // namespace ndn
} // namespace ns3

#endif // NDN_PIT_TRACER_H
