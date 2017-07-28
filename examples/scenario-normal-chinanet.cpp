// scenario.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#include "src/ndnSIM/utils/ndnsim-globe.hpp"

#define configFilePath "src/ndnSIM/examples/etc/config.xml"
ConfigurationParameter g_CP;

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
  AnnotatedTopologyReader topologyReader ("", 5);
  topologyReader.SetFileName(g_CP.getTopoFilePath()); 
  topologyReader.Read ();
  
  // 2. classify Nodes
  NodeContainer consumerNodes, producerNodes, attackerNodes;
  
  for (NodeList::Iterator node = NodeList::Begin(); node != NodeList::End(); node++) {
	  std::string name = Names::FindName(*node);
	  if(name.find("BJServer")!=string::npos||name.find("SHServer")!=string::npos||name.find("GZServer")!=string::npos||name.find("XAServer")!=string::npos)//if(name.find("Server")!=string::npos)
	  {
		  producerNodes.Add( *node );
	  }
	  else if(name.find("App")!=string::npos) // 用App标识的节点
	  {
		  consumerNodes.Add ( *node );
		  if(name.find("Xi'an")!=string::npos)
		  {
			   string name = Names::FindName (*node);
			   Names::Rename (name, "AppXiAn");
		  }
	  }
	  
	  else
	  {
		  if(name.find("Xi'an")!=string::npos)
		  {
			   string name = Names::FindName (*node);
			   Names::Rename (name, "XiAn");
		  }
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
  string prefix = "/China/";
  
  NodeContainer BJConsumers, SHConsumers, GZConsumers, XAConsumers;
  set< Ptr<Node> > BJConsumersSet;
  set< Ptr<Node> > SHConsumersSet;
  set< Ptr<Node> > GZConsumersSet;
  set< Ptr<Node> > XAConsumersSet;
  double ratioBJ = 0.6, ratioSH = 0.4, ratioGZ = 0.4, ratioXA = 0.4;
  
  // BJ consumers 100%consumerNodes
  while(BJConsumersSet.size() < consumerNodes.GetN()*ratioBJ)
  {
      Ptr<Node> node = consumerNodes.Get (randomUInt(0,consumerNodes.GetN()));

      if (BJConsumersSet.find (node) != BJConsumersSet.end ())
        continue;
      BJConsumersSet.insert (node);
  }
  for (set< Ptr<Node> >::iterator it = BJConsumersSet.begin (); it != BJConsumersSet.end (); it++)
  {
	  //std::cout<< "\t" << (*it)->GetId() << "\t" << Names::FindName(*it) << "\n";
	  BJConsumers.Add(*it);
  }
  // SH consumers %consumerNodes
  while(SHConsumersSet.size() < consumerNodes.GetN()*ratioSH)
  {
      Ptr<Node> node = consumerNodes.Get (randomUInt(0,consumerNodes.GetN()));

      if (SHConsumersSet.find (node) != SHConsumersSet.end ())
        continue;
      SHConsumersSet.insert (node);
  }
  for (set< Ptr<Node> >::iterator it = SHConsumersSet.begin (); it != SHConsumersSet.end (); it++)
  {
	  //std::cout<< "\t" << (*it)->GetId() << "\t" << Names::FindName(*it) << "\n";
	  SHConsumers.Add(*it);
  }
  // GZ consumers %consumerNodes
  while(GZConsumersSet.size() < consumerNodes.GetN()*ratioGZ)
  {
      Ptr<Node> node = consumerNodes.Get (randomUInt(0,consumerNodes.GetN()));

      if (GZConsumersSet.find (node) != GZConsumersSet.end ())
        continue;
      GZConsumersSet.insert (node);
  }
  for (set< Ptr<Node> >::iterator it = GZConsumersSet.begin (); it != GZConsumersSet.end (); it++)
  {
	  //std::cout<< "\t" << (*it)->GetId() << "\t" << Names::FindName(*it) << "\n";
	  GZConsumers.Add(*it);
  }
  // XA consumers %consumerNodes
  while(XAConsumersSet.size() < consumerNodes.GetN()*ratioXA)
  {
      Ptr<Node> node = consumerNodes.Get (randomUInt(0,consumerNodes.GetN()));

      if (XAConsumersSet.find (node) != XAConsumersSet.end ())
        continue;
      XAConsumersSet.insert (node);
  }
  for (set< Ptr<Node> >::iterator it = XAConsumersSet.begin (); it != XAConsumersSet.end (); it++)
  {
	  //std::cout<< "\t" << (*it)->GetId() << "\t" << Names::FindName(*it) << "\n";
	  XAConsumers.Add(*it);
  }
  
  int index = 0;
  NodeContainer::Iterator it;
  
  // ========================normal consumer====f(k;N,q,s) = 1/(k+q)^s/(sigma(1/(i+q)^s))
  string fre = g_CP.getFrequency();//;"400"
  ndn::AppHelper consumerHelper(g_CP.getConsumerType());// "ns3::ndn::ConsumerCbr"
  consumerHelper.SetAttribute("Frequency", StringValue(fre));//
  consumerHelper.SetAttribute("Randomize", StringValue("exponential")); //none  uniform
  consumerHelper.SetAttribute("NumberOfContents", StringValue(g_CP.getContents()));
  consumerHelper.SetAttribute("q", StringValue("0.0"));
  consumerHelper.SetAttribute("s", StringValue("0.0"));    //(1+q)^s
  
  for(it = producerNodes.Begin(); it!=producerNodes.end(); ++it)
  {
	  ns3::ApplicationContainer app;
	  consumerHelper.SetPrefix(prefix + Names::FindName (*it));
	  
	  if(Names::FindName (*it).compare(0, 8, "BJServer")==0)
	  {
		  app = consumerHelper.Install(BJConsumers);
		  app.Start(Seconds(randomTime() + g_CP.getStartTime()));
	  }
	  else if(Names::FindName (*it).compare(0, 8, "SHServer")==0)
	  {
		  app = consumerHelper.Install(SHConsumers);
		  app.Start(Seconds(randomTime() + g_CP.getStartTime()));
	  }
	  else if(Names::FindName (*it).compare(0, 8, "GZServer")==0)
	  {
		  app = consumerHelper.Install(GZConsumers);
		  app.Start(Seconds(randomTime() + g_CP.getStartTime()));	  
	  }
	  else if(Names::FindName (*it).compare(0, 8, "XAServer")==0)
	  {
		  app = consumerHelper.Install(XAConsumers);
		  app.Start(Seconds(randomTime() + g_CP.getStartTime()));	  
	  }
	  //break;BJConsumers, SHConsumers, GZConsumers, XAConsumers;
  }
  
  // ========================normal producer====  
  ndn::AppHelper producerHelper(g_CP.getProducerType());
  producerHelper.SetAttribute("PayloadSize", StringValue(g_CP.getDataSize()));
  for(it = producerNodes.Begin(); it!=producerNodes.end(); ++it)
  {
	  producerHelper.SetPrefix(prefix + Names::FindName (*it));
	  producerHelper.Install(*it);
      ndnGlobalRoutingHelper.AddOrigins(prefix + Names::FindName (*it), *it);
	  ndn::GlobalRoutingHelper::CalculateRoutes();
  }
  
  
  // ========================Collusive consumer====
/*
  NodeContainer Attackers, ColluProducers;
  set< Ptr<Node> > AttackersSet;
  double ratioAttak = g_CP.getAttackRatio();
  
  // Collusive consumers %consumerNodes
  while(AttackersSet.size() < consumerNodes.GetN()*ratioAttak)
  {
      Ptr<Node> node = consumerNodes.Get (randomUInt(0,consumerNodes.GetN()));

      if (AttackersSet.find (node) != AttackersSet.end ())
        continue;
	  Ptr<Node> node2 = NodeList::GetNode(node->GetId()-42);
      AttackersSet.insert (node2);
	  string name = Names::FindName (node2);
	  Names::Rename (name, "CIFA"+name);
  }
  for (set< Ptr<Node> >::iterator it = AttackersSet.begin (); it != AttackersSet.end (); it++)
  {
	  Attackers.Add(*it);
  }
  // ========================Collusive producer====
  ColluProducers.Add(Names::Find<Node>("Shenyang"));
  
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

  //std::cout << "strCIFA = " << strCIFA.c_str() << std::endl;
  
  // ========================Collusive consumer install====  
  ndn::AppHelper consumerHelperCol("ns3::ndn::ConsumerBatches");
  string strTemp = prefix + "BJServer/Collusive";
  consumerHelperCol.SetPrefix(strTemp.c_str());
  consumerHelperCol.SetAttribute("Batches", StringValue(strCIFA.c_str()));
  ns3::ApplicationContainer appCol = consumerHelperCol.Install(Attackers);
  appCol.Start(Seconds(g_CP.getAttackStartTime()));
  
  
  // ========================Collusive producer install====  
  ndn::AppHelper producerHelperCol(g_CP.getProducerType());
  producerHelperCol.SetAttribute("PayloadSize", StringValue(g_CP.getDataSize()));
  producerHelperCol.SetPrefix(strTemp.c_str());
  
  producerHelperCol.Install(ColluProducers);
  ndnGlobalRoutingHelper.AddOrigins(strTemp, ColluProducers.Get(0));
*/
  ndn::GlobalRoutingHelper::CalculateRoutes();
  
  Simulator::Stop(Seconds(g_CP.getStopTime()));
  /*
  ostringstream os2, os3, os4, os5;
  os2 << attackPeriod;
  os3 << burstT;
  os4 << g_CP.getAttackDelay();
  os5 << ratioAttak;
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
  
  // string rateTracerDir = traceDir + "/rate-trace-normal-" + fre + "-chinanet-CIFA-" + attackFreq + "-T-" + os2.str() + "-B-" + os3.str() + "-D-" + os4.str() + "-R-" + os5.str() + ".txt";
  // string pitTracerDir = traceDir + "/pit-trace-normal-" + fre + "-chinanet-CIFA-" + attackFreq + "-T-" + os2.str()  + "-B-" + os3.str() + "-D-" + os4.str() + "-R-" + os5.str() + ".txt";
	
  string rateTracerDir = traceDir + "/rate-trace-normal-" + fre + "-chinanet.txt";
  string pitTracerDir = traceDir + "/pit-trace-normal-" + fre + "-chinanet.txt";
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

