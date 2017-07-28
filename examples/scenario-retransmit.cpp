// scenario.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#include "src/ndnSIM/utils/ndnsim-globe.hpp"

#define configFilePath "src/ndnSIM/examples/etc/config.xml"
ConfigurationParameter g_CP;

namespace ns3 {	
	
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
  NodeContainer consumerNodes, producerNodes;
  
  consumerNodes.Add(Names::Find<Node>("Src1"));
  //consumerNodes.Add(Names::Find<Node>("Src2"));
  
  producerNodes.Add(Names::Find<Node>("Dst1"));
  //producerNodes.Add(Names::Find<Node>("Dst2"));
  
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
  
  // 6. Install NDN applications
  char normalServer[11]={"/root/Evil"};
  string strTemp;
  NodeContainer::Iterator it;
  
  // ========================normal consumer====f(k;N,q,s) = 1/(k+q)^s/(sigma(1/(i+q)^s))
  // Src1        Rtr1        10Mbps      1        10ms    20
  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  consumerHelper.SetAttribute("Frequency", DoubleValue (1.0));//
  consumerHelper.SetAttribute("Randomize", StringValue("none"));
  
  for(it = consumerNodes.Begin(); it!=consumerNodes.end(); ++it)
  {
	  consumerHelper.SetPrefix(normalServer);
	  ns3::ApplicationContainer app = consumerHelper.Install(*it);
	  app.Start(Seconds(g_CP.getStartTime()));
  }
  
  // ========================normal producer====  
  ndn::AppHelper producerHelper(g_CP.getProducerType());
  producerHelper.SetAttribute("PayloadSize", StringValue(g_CP.getDataSize()));
  for(it = producerNodes.Begin(); it!=producerNodes.end(); ++it)
  {
	  producerHelper.SetPrefix(normalServer);
	  producerHelper.Install(*it);
      ndnGlobalRoutingHelper.AddOrigins(normalServer, *it);
	  ndn::GlobalRoutingHelper::CalculateRoutes();
	  break;
  }
  
  ndn::GlobalRoutingHelper::CalculateRoutes();
  Simulator::Stop(Seconds(100));// 
  
  //tracers   /usr/local/src/trace/
  //ndn::L3RateTracer::InstallAll("/usr/local/src/trace/rate-trace-normal.txt", Seconds(0.1));
  //ndn::AppDelayTracer::InstallAll("/usr/local/src/trace/app-delays-trace.txt");
  //ndn::CsTracer::InstallAll("/usr/local/src/trace/cs-trace.txt", Seconds(50));
  //ndn::PitTracer::InstallAll("/usr/local/src/trace/pit-trace-normal.txt", Seconds(0.1));
  
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
