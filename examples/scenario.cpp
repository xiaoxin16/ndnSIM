// scenario.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#include "src/ndnSIM/utils/ndnsim-globe.hpp"

#define configFilePath "src/ndnSIM/examples/etc/config.xml"
ConfigurationParameter g_CP;

namespace ns3 {
	
void test()
{
    int test_i = 0;
	//random
	while((test_i++)<10)
	{
		double min = 0.0;
		double max = 1.0;
		Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
		x->SetAttribute ("Min", DoubleValue (min));
		x->SetAttribute ("Max", DoubleValue (max));
		double value = x->GetValue ();
		std::cout<< value <<std::endl;
	}
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
  AnnotatedTopologyReader topologyReader ("", 5);
  topologyReader.SetFileName(g_CP.getTopoFilePath()); 
  topologyReader.Read ();
  
  // 2. classify Nodes
  NodeContainer consumerNodes, producerNodes;
  
  for (NodeList::Iterator node = NodeList::Begin(); node != NodeList::End(); node++) {
	  std::string name = Names::FindName(*node);
	  if(name.find("Beijing")!=string::npos||name.find("Shanghai")!=string::npos||name.find("Guangzhou")!=string::npos||name.find("Xi'an")!=string::npos)
	  {
		  producerNodes.Add( *node );
	  }
	  consumerNodes.Add ( *node );
  }
  
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
  char normalServer[1][10]={"/HERNDON"};
  
  //int goodPrefixNum=38;//g_CP.getGoodPrefixNum();
  string strTemp;
  //int index = 0;
  NodeContainer::Iterator it;
  
  // ========================normal consumer====f(k;N,q,s) = 1/(k+q)^s/(sigma(1/(i+q)^s))
  ndn::AppHelper consumerHelper(g_CP.getConsumerType());
  consumerHelper.SetAttribute("Frequency", StringValue(g_CP.getFrequency()));//g_CP.getFrequency()
  consumerHelper.SetAttribute("NumberOfContents", StringValue(g_CP.getContents()));
  consumerHelper.SetAttribute("Randomize", StringValue("exponential"));
  consumerHelper.SetAttribute("q", StringValue("0.0"));
  consumerHelper.SetAttribute("s", StringValue("1.0"));
  
  for(it = consumerNodes.Begin(); it!=consumerNodes.end(); ++it)
  {
	  consumerHelper.SetPrefix(normalServer[0]);
	  ns3::ApplicationContainer app = consumerHelper.Install(*it);
	  app.Start(Seconds(g_CP.getStartTime()));
  }
  
  // ========================normal producer====  
  ndn::AppHelper producerHelper(g_CP.getProducerType());
  producerHelper.SetAttribute("PayloadSize", StringValue(g_CP.getDataSize()));
  for(it = producerNodes.Begin(); it!=producerNodes.end(); ++it)
  {
	  producerHelper.SetPrefix(normalServer[0]);
	  producerHelper.Install(*it);
      ndnGlobalRoutingHelper.AddOrigins(normalServer[0], *it);
	  ndn::GlobalRoutingHelper::CalculateRoutes();
	  break;
  }
  
  
  
  ndn::GlobalRoutingHelper::CalculateRoutes();
  Simulator::Stop(Seconds(g_CP.getStopTime()));// 
  
  //tracers   /usr/local/src/trace/
  ndn::L3RateTracer::InstallAll("/usr/local/src/trace/rate-trace.txt", Seconds(1));
  //ndn::AppDelayTracer::InstallAll("/usr/local/src/trace/app-delays-trace.txt");
  //ndn::CsTracer::InstallAll("/usr/local/src/trace/cs-trace.txt", Seconds(50));
  ndn::PitTracer::InstallAll("/usr/local/src/trace/pit-trace.txt", Seconds(1));
  
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
