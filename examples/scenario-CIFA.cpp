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
	  //if(name.find("Beijing")!=string::npos||name.find("Shanghai")!=string::npos||name.find("Guangzhou")!=string::npos||name.find("Xi'an")!=string::npos)
	  if(name.find("Server")!=string::npos)
	  {
		  producerNodes.Add( *node );
	  }
	  else if(name.find("Kashi")!=string::npos||name.find("Lhasa")!=string::npos||name.find("Wuhan")!=string::npos||name.find("Guangzhou")!=string::npos||name.find("Shanghai")!=string::npos||name.find("Zhengzhou")!=string::npos||name.find("Harbin")!=string::npos)
	  {
		  consumerNodes.Add ( *node );
	  }
	  
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
  char Collusiveconsumer[4][20]={"Lanzhou", "Chengdu", "Taiyuan", "Hangzhou"};
  char Collusiveproducer[1][20]={"Shenyang"};    
  //int goodPrefixNum=38;//g_CP.getGoodPrefixNum();
  string strTemp;
  int index = 0;
  NodeContainer::Iterator it;
  
  // ========================normal consumer====f(k;N,q,s) = 1/(k+q)^s/(sigma(1/(i+q)^s))
  ndn::AppHelper consumerHelper(g_CP.getConsumerType());
  consumerHelper.SetAttribute("Frequency", StringValue(g_CP.getFrequency()));//g_CP.getFrequency()
  consumerHelper.SetAttribute("NumberOfContents", StringValue(g_CP.getContents()));
  consumerHelper.SetAttribute("Randomize", StringValue("exponential"));
  consumerHelper.SetAttribute("q", StringValue("0.0"));
  consumerHelper.SetAttribute("s", StringValue("0.0"));
  
  for(it = consumerNodes.Begin(); it!=consumerNodes.end(); ++it)
  {
	  //it++;
	  consumerHelper.SetPrefix(normalServer[0]);
	  ns3::ApplicationContainer app = consumerHelper.Install(*it);
	  app.Start(Seconds(g_CP.getStartTime()));
	  //break;
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
  
  // ========================Collusive consumer====
  ndn::AppHelper consumerHelperCol("ns3::ndn::ConsumerBatches");
  
  for (index = 0; index < 4; index++)
  {
	  strTemp = string(normalServer[0]) + "/Collusive" + std::to_string(index);// ;
	  consumerHelperCol.SetPrefix(strTemp.c_str());
	  consumerHelperCol.SetAttribute("Batches", StringValue("0s 50 0.0025s 1 0.005s 1 0.0075s 1 0.01s 1 0.0125s 1 0.015s 1 0.0175s 1 0.02s 1 0.0225s 1 0.025s 1 0.0275s 1 0.03s 1 0.0325s 1 0.035s 1 0.0375s 1 0.04s 1 0.0425s 1 0.045s 1 0.0475s 1 0.05s 1 0.0525s 1 0.055s 1 0.0575s 1 0.06s 1 0.0625s 1 0.065s 1 0.0675s 1 0.07s 1 0.0725s 1 0.075s 1 0.0775s 1 0.08s 1 0.0825s 1 0.085s 1 0.0875s 1 0.09s 1 0.0925s 1 0.095s 1 0.0975s 1 0.1s 1 0.1025s 1 0.105s 1 0.1075s 1 0.11s 1 0.1125s 1 0.115s 1 0.1175s 1 0.12s 1 "));
	  ns3::ApplicationContainer appCol = consumerHelperCol.Install(Names::Find<Node>(Collusiveconsumer[index]));
	  appCol.Start(Seconds(100));// + x->GetValue ()
  }
  
  // ========================Collusive producer====  
  ndn::AppHelper producerHelperCol(g_CP.getProducerType());
  producerHelperCol.SetAttribute("PayloadSize", StringValue(g_CP.getDataSize()));
  for(index = 0; index < 1; index++)
  {
	  strTemp = string(normalServer[0]) + "/Collusive" + std::to_string(index);// ;
	  producerHelperCol.SetPrefix(strTemp);
	  producerHelperCol.Install(Names::Find<Node>(Collusiveproducer[0]));
	  ndnGlobalRoutingHelper.AddOrigins(strTemp, Names::Find<Node> (Collusiveproducer[0]));
	  ndn::GlobalRoutingHelper::CalculateRoutes();
  }  
  
  ndn::GlobalRoutingHelper::CalculateRoutes();
  Simulator::Stop(Seconds(g_CP.getStopTime()));// 
  
  //tracers   /usr/local/src/trace/
  ndn::L3RateTracer::InstallAll("/usr/local/src/trace/rate-trace-CIFA-4.txt", Seconds(0.2));
  //ndn::AppDelayTracer::InstallAll("/usr/local/src/trace/app-delays-trace.txt");
  //ndn::CsTracer::InstallAll("/usr/local/src/trace/cs-trace.txt", Seconds(50));
  ndn::PitTracer::InstallAll("/usr/local/src/trace/pit-trace-CIFA-4.txt", Seconds(0.2));
  
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
