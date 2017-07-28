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
  AnnotatedTopologyReader topologyReader ("", 10);
  topologyReader.SetFileName(g_CP.getTopoFilePath()); 
  topologyReader.Read ();
  
  // 2. classify Nodes
  NodeContainer consumerNodes, producerNodes, attackerNodes;
  
  consumerNodes.Add(Names::Find<Node>("Src1"));
  attackerNodes.Add(Names::Find<Node>("Src2"));
  
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
  char normalServer[6]={"/root"};
  string strTemp;
  NodeContainer::Iterator it;
  int index = 0;
  
  // ========================normal consumer====f(k;N,q,s) = 1/(k+q)^s/(sigma(1/(i+q)^s))
  // Src1        Rtr1        10Mbps      1        10ms    20
  string fre = g_CP.getFrequency();//;"400"
  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");//g_CP.getConsumerType() 
  consumerHelper.SetAttribute("Frequency", StringValue(fre));//
  consumerHelper.SetAttribute("Randomize", StringValue("exponential")); //none  uniform
/*   consumerHelper.SetAttribute("NumberOfContents", StringValue(g_CP.getContents()));
  consumerHelper.SetAttribute("q", StringValue("0.0"));
  consumerHelper.SetAttribute("s", StringValue("1.0"));    //(1+q)^s */
  
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
  
 /* 
  string strCIFA="";
  int attackTime = g_CP.getStopTime() - g_CP.getAttackStartTime();
  double attackPeriod =  g_CP.getAttackPeriod();
  string attackFreq = g_CP.getAttackFrequency();
  double burstT = g_CP.getAttackBurstT();
  uint32_t num = (uint32_t)(burstT*atof(attackFreq.c_str()));
  for(index=0; index < attackTime/attackPeriod; index++)
  {
          ostringstream os;
          os << (index*attackPeriod);
          strCIFA.append(os.str());
          strCIFA.append("s ");
          strCIFA.append("1 ");
          for (int index2=1; index2<num; index2++)
          {
              ostringstream oss;
              oss << (index*attackPeriod+index2/atof(attackFreq.c_str()));
              strCIFA.append(oss.str());
              strCIFA.append("s ");
              strCIFA.append("1 ");
          }
          //strCIFA.append(" ");
  }

  std::cout << "strCIFA = " << strCIFA.c_str() << std::endl;
  //return 0;
  // ========================Collusive consumer====
  ndn::AppHelper consumerHelperCol("ns3::ndn::ConsumerBatches");
  string attack = "/Collusive";
  for (index = 0; index < 1; index++)
  {
	  strTemp = string(normalServer) + attack;// ;
	  consumerHelperCol.SetPrefix(strTemp.c_str());
	  consumerHelperCol.SetAttribute("Batches", StringValue(strCIFA.c_str()));
	  ns3::ApplicationContainer appCol = consumerHelperCol.Install(Names::Find<Node>("Src2"));
	  appCol.Start(Seconds(100));// + x->GetValue ()
  }
  
  // ========================Collusive producer====  
  ndn::AppHelper producerHelperCol(g_CP.getProducerType());
  producerHelperCol.SetAttribute("PayloadSize", StringValue(g_CP.getDataSize()));
  for(index = 0; index < 1; index++)
  {
	  strTemp = string(normalServer) + attack;// ;
	  producerHelperCol.SetPrefix(strTemp);
	  producerHelperCol.Install(*(producerNodes.begin()));
	  ndnGlobalRoutingHelper.AddOrigins(strTemp, *(producerNodes.begin()));
	  ndn::GlobalRoutingHelper::CalculateRoutes();
  }  
  */
  ndn::GlobalRoutingHelper::CalculateRoutes();
  Simulator::Stop(Seconds(g_CP.getStopTime()));// 
  
  //tracers   /usr/local/src/trace/    
  //
  /*
  ostringstream os2, os3, os4;
  os2 << attackPeriod;
  os3 << burstT;
  os4 << g_CP.getAttackDelay();
  */
  string scenario = g_CP.getScenario();
  string traceDir = "";
  string globalDir = "/usr/local/src/trace/" + scenario + "/";
  
  if(strlen(g_CP.getTraceDir()) == 0)
  {
	  // std::cout << "trace_dir = \"\"" << std::endl;
	  traceDir = globalDir;
  }
  else
  {
	  traceDir = globalDir + g_CP.getTraceDir();
  }
  system(("mkdir -p \"" + traceDir + "\"").c_str());
  
  // string rateTracerDir = traceDir + "/rate-trace-normal-" + fre + "-simple-CIFA-" + attackFreq + "-T-" + os2.str() + "-B-" + os3.str() + "-D-" + os4.str() + ".txt";
  // string pitTracerDir = traceDir + "/pit-trace-normal-" + fre + "-simple-CIFA-" + attackFreq + "-T-" + os2.str()  + "-B-" + os3.str() + "-D-" + os4.str() + ".txt";
	
  string rateTracerDir = traceDir + "/rate-trace-normal-" + fre + "-simple.txt";
  string pitTracerDir = traceDir + "/pit-trace-normal-" + fre + "-simple.txt";
  ndn::L3RateTracer::InstallAll(rateTracerDir.c_str(), Seconds(1));
  ndn::PitTracer::InstallAll(pitTracerDir.c_str(), Seconds(1));
  
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
