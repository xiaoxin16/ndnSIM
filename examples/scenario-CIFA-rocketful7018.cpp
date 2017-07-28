// scenario.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#include "src/ndnSIM/utils/ndnsim-globe.hpp"

#define configFilePath "src/ndnSIM/examples/etc/config.xml"
ConfigurationParameter g_CP;

using namespace ns3;
using namespace ns3::ndn;
using namespace std;

namespace ns3 {	

double randomTime()
{
	double min = 0.0, max = 1.0;
	Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
	x->SetAttribute ("Min", DoubleValue (min));
	x->SetAttribute ("Max", DoubleValue (max));
	return x->GetValue ();
}

uint32_t randomUInt(uint32_t a, uint32_t b)
{
	double min = a*1.0, max = b*1.0;
	Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
	x->SetAttribute ("Min", DoubleValue (min));
	x->SetAttribute ("Max", DoubleValue (max));
	return (uint32_t)(x->GetValue ());
}

int
main(int argc, char* argv[])
{
	if(!g_CP.readConf())
	{
		printf("In %s:%d, Init configuration parameter error!", __FILE__, __LINE__);
		return -1;
	}

  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  // 1. read topology
  AnnotatedTopologyReader topologyReader ("", 10);
  topologyReader.SetFileName(g_CP.getTopoFilePath()); 
  topologyReader.Read ();
  
  // 2. classify Nodes 
  NodeContainer leaves, gw, bb;
  for_each (NodeList::Begin (), NodeList::End (), [&] (Ptr<Node> node) {
  if (Names::FindName (node).compare (0, 5, "leaf-")==0)
  {
	  leaves.Add (node);
  }
      else if (Names::FindName (node).compare (0, 3, "gw-")==0)
        {
          gw.Add (node);
        }
      else if (Names::FindName (node).compare (0, 3, "bb-")==0)
        {
          bb.Add (node);
        }
    });
  std::cout << "Total_numbef_of_nodes      " << NodeList::GetNNodes () << std::endl;
  std::cout << "Total_number_of_leaf_nodes " << leaves.GetN () << std::endl;
  std::cout << "Total_number_of_gw_nodes   " << gw.GetN () << std::endl;
  std::cout << "Total_number_of_bb_nodes   " << bb.GetN () << std::endl;
  
  // 3. Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  
  if(g_CP.getCsSize()<1)
  {
	  ndnHelper.SetOldContentStore("ns3::ndn::cs::Nocache");
	  ndnHelper.InstallAll();
  }
  else
  {
	  ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize",std::to_string(g_CP.getCsSize())); 
	  ndnHelper.InstallAll();
  }
  
  // 4. Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll("/", g_CP.getFwStrategy());
  // 5. Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();  
  
  
  // 6. Consumer, Producer, Attacker
  uint32_t badCount = 0;
  uint32_t goodCount = leaves.GetN () - badCount;
  string producerLocation = "bb"; //"gw"
  string prefix = "/root";
  
  NodeContainer evilNodes;
  NodeContainer goodNodes;
  NodeContainer serverNodes;
  
  set< Ptr<Node> > producers;
  set< Ptr<Node> > evils;
  set< Ptr<Node> > angels;
  
  // std::cout << "Number of evil nodes: " << badCount << std::endl;	
  
  while (evils.size () < badCount)
    {
      uint32_t index = randomUInt (0, leaves.GetN ());
      Ptr<Node> node = leaves.Get (index);

      if (evils.find (node) != evils.end ())
        continue;
      evils.insert (node);
      
      string name = Names::FindName (node);
      Names::Rename (name, "evil-"+name);
    }

  while (angels.size () < goodCount)
    {
      uint32_t index = randomUInt (0, leaves.GetN ());
      Ptr<Node> node = leaves.Get (index);
      if (angels.find (node) != angels.end () ||
          evils.find (node) != evils.end ())
        continue;
      
      angels.insert (node);
      string name = Names::FindName (node);
      Names::Rename (name, "good-"+name);
    }

  while (producers.size () < 1)
    {
      Ptr<Node> node = 0;
      if (producerLocation == "gw")
        {
          uint32_t index = randomUInt (0, gw.GetN ());
          node = gw.Get (index);
        }
      else if (producerLocation == "bb")
        {
          uint32_t index = randomUInt (0, bb.GetN ());
          node = bb.Get (index);
        }
      
      producers.insert (node);
      string name = Names::FindName (node);
      Names::Rename (name, "producer-"+name);
    }
  // a little bit of C++11 flavor, compile with -std=c++11 flag
  std::cout << "Evil: " << evils.size() << "\n";
  for (set< Ptr<Node> >::iterator it = evils.begin (); it != evils.end (); it++)
  {
	  std::cout<< "\t" << (*it)->GetId() << "\t" << Names::FindName(*it) << "\n";
	  evilNodes.Add(*it);
  }
  std::cout << "Good: " << angels.size() << "\n";
  for (set< Ptr<Node> >::iterator it = angels.begin (); it != angels.end (); it++)
  {
	  std::cout<< "\t" << (*it)->GetId() << "\t" << Names::FindName(*it) << "\n";
	  goodNodes.Add(*it);
  }
  std::cout  << "Producers: " << producers.size() << "\n";
  for (set< Ptr<Node> >::iterator it = producers.begin (); it != producers.end (); it++)
  {
	  std::cout<< "\t" << (*it)->GetId() << "\t" << Names::FindName(*it) << "\n";
	  serverNodes.Add(*it);
  }
  //std::cout  << "\n";
  
  // good
  ndn::AppHelper consumerHelper(g_CP.getConsumerType());
  consumerHelper.SetAttribute("Frequency", StringValue(g_CP.getFrequency()));//g_CP.getFrequency()
  consumerHelper.SetAttribute("NumberOfContents", StringValue(g_CP.getContents()));
  consumerHelper.SetAttribute("Randomize", StringValue("exponential"));
  consumerHelper.SetAttribute("q", StringValue("0.0"));
  consumerHelper.SetAttribute("s", StringValue("0.0"));
  
  consumerHelper.SetPrefix(prefix + "/good");
  consumerHelper.Install(goodNodes);
  
  // evil
  ndn::AppHelper consumerHelperCol(g_CP.getConsumerType());
  consumerHelperCol.SetAttribute("Frequency", StringValue(g_CP.getFrequency()));//g_CP.getFrequency()
  consumerHelperCol.SetAttribute("NumberOfContents", StringValue(g_CP.getContents()));
  consumerHelperCol.SetAttribute("Randomize", StringValue("exponential"));
  consumerHelperCol.SetAttribute("q", StringValue("0.0"));
  consumerHelperCol.SetAttribute("s", StringValue("0.0"));
  
  consumerHelperCol.SetPrefix(prefix + "/evil");
  consumerHelperCol.Install(evilNodes);
  
  //for (NodeContainer::Iterator node = goodNodes.Begin (); node != goodNodes.End (); node++)
  //{}
  
  //for (NodeContainer::Iterator node = evilNodes.Begin (); node != evilNodes.End (); node++)
  //{}
  
  ndn::AppHelper ph ("ns3::ndn::Producer");
  ph.SetAttribute ("PayloadSize", StringValue("1100"));
  ph.SetPrefix (prefix + "/good");
  ph.Install (serverNodes);
  
  //
  ndnGlobalRoutingHelper.AddOrigins (prefix, serverNodes);
  ndnGlobalRoutingHelper.AddOrigins (prefix + "/good", serverNodes);
  ndnGlobalRoutingHelper.AddOrigins (prefix + "/evil", serverNodes);
  ndnGlobalRoutingHelper.CalculateRoutes ();
  // 6. Install NDN applications

  Simulator::Stop(Seconds(g_CP.getStopTime()));
  
  ostringstream os;
  os << g_CP.getFrequency();

  string rateTracerDir = "/usr/local/src/trace/rate-trace-CIFA-complex-N-" + os.str() + ".txt";
  string pitTracerDir = "/usr/local/src/trace/pit-trace-CIFA-complex-N-" + os.str() + ".txt";
  
  string pitTracerDirbb = "/usr/local/src/trace/pit-trace-CIFA-complex-N-" + os.str() + "-bb.txt";
  string pitTracerDirgw = "/usr/local/src/trace/pit-trace-CIFA-complex-N-" + os.str() + "-gw.txt";
  string pitTracerDirleaves = "/usr/local/src/trace/pit-trace-CIFA-complex-N-" + os.str() + "-leaves.txt";
	
  ndn::L3RateTracer::InstallAll(rateTracerDir.c_str(), Seconds(1));
  //ndn::PitTracer::InstallAll(pitTracerDir.c_str(), Seconds(0.1));
  
  ndn::PitTracer::Install(bb, pitTracerDirbb.c_str(), Seconds(1));
  ndn::PitTracer::Install(gw, pitTracerDirgw.c_str(), Seconds(1));
  ndn::PitTracer::Install(leaves, pitTracerDirleaves.c_str(), Seconds(1));
  
  Simulator::Run();
  Simulator::Destroy();
  
  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
