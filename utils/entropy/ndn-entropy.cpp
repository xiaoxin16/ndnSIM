// mytools.cpp
#include "ndn-entropy.hpp"
#include "src/ndnSIM/utils/parseFile/parseFile.hpp"
#define configFilePath "src/ndnSIM/examples/etc/config.xml"


using namespace std;
namespace nfd {
	
Entropy::Entropy()
{
	m_vec=new vector<string>();
	m_map=new map<string, int>();
	m_oldPrefixMap=new map<string, int>();
	m_nowPrefixMap=new map<string, int>();
	m_entropy=new vector<double>();

	ConfigurationParameter g_CP;
  	if(!g_CP.readConf())
  	{
  		printf("In %s:%d, Init configuration parameter error!", __FILE__, __LINE__);
 		//return -1;
 	}


    m_deltaN = 100;
	theta=g_CP.getTheta();
	cumulateEntropy=0;
	weight=0.5;
	alpha = g_CP.getAlpha();
	eThreshold=-1;
	cThreshold=g_CP.getCumulateTh();
	attackPrefix="";
}

Entropy::~Entropy()
{
	delete m_vec;
	delete m_map;
	delete m_oldPrefixMap;
	delete m_nowPrefixMap;
	delete m_entropy;
}

void Entropy::Add(char *s)
{
	string ss(s);
	Add(ss);
}


void Entropy::addOldPrefixMap(string ss)
{
    int pos=ss.find_last_of('/');
    string dstPrefix="";
    dstPrefix.assign(ss, 0, pos);
    map<string, int>::iterator map_it=m_oldPrefixMap->find(dstPrefix);
	if(map_it!=m_oldPrefixMap->end())                         // find frequency++
	{
		int j=map_it->second;
		map_it->second=++j;
	}
	else                                                      // insert str
	{
		m_oldPrefixMap->insert(pair<string, int>(dstPrefix, 1));
	}
}
void Entropy::addNowPrefixMap(string ss)
{
    int pos=ss.find_last_of('/');
    string dstPrefix="";
    dstPrefix.assign(ss, 0, pos);
    map<string, int>::iterator map_it=m_nowPrefixMap->find(dstPrefix);
	if(map_it!=m_nowPrefixMap->end())                         // find frequency++
	{
		int j=map_it->second;
		map_it->second=++j;
	}
	else                                                      // insert str
	{
		m_nowPrefixMap->insert(pair<string, int>(dstPrefix, 1));
	}
}
void Entropy::addmMap(string ss)
{
    map<string, int>::iterator map_it=m_map->find(ss);
	if(map_it!=m_map->end())                         // find frequency++
	{
		int j=map_it->second;
		map_it->second=++j;
	}
	else                                                      // insert str
	{
		m_map->insert(pair<string, int>(ss, 1));
	}
}
void Entropy::delOldPrefixMap(string ss)
{
    int pos=ss.find_last_of('/');
    string dstPrefix="";
    dstPrefix.assign(ss, 0, pos);
    map<string, int>::iterator map_it=m_oldPrefixMap->find(dstPrefix);
    if(map_it!=m_oldPrefixMap->end())              //   update the delete prefix in m_oldPrefixMap
    {
        if(map_it->second<2)
        {
            m_oldPrefixMap->erase(map_it);
        }
        else
        {
            int j=map_it->second;
            map_it->second=--j;
        }
    }
}
void Entropy::delNowPrefixMap(string ss)
{
    int pos=ss.find_last_of('/');
    string dstPrefix="";
    dstPrefix.assign(ss, 0, pos);
    map<string, int>::iterator map_it=m_nowPrefixMap->find(dstPrefix);
    if(map_it!=m_nowPrefixMap->end())              //   update the delete prefix in m_newPrefixMap
    {
        if(map_it->second<2)
        {
            m_nowPrefixMap->erase(map_it);
        }
        else
        {
            int j=map_it->second;
            map_it->second=--j;
        }
    }
}
void Entropy::delmMap(string ss)
{
    map<string, int>::iterator map_it=m_map->find(ss);
    if(map_it!=m_map->end())              //   update the delete prefix in m_newPrefixMap
    {
        if(map_it->second<2)
        {
            m_map->erase(map_it);
        }
        else
        {
            int j=map_it->second;
            map_it->second=--j;
        }
    }
}
void Entropy::Add(string ss)
{
	if(m_vec->size()==(m_wind+m_deltaN))               //m_vec->size() = m_wind+m_deltaN
    {
        //del
        delNowPrefixMap(m_vec->at(m_vec->size()-m_wind));
        delOldPrefixMap(m_vec->at(0));
        delmMap(m_vec->at(m_vec->size()-m_wind));
        m_vec->erase(m_vec->begin());

        //add
        m_vec->push_back(ss);
        addmMap(ss);
        addNowPrefixMap(ss);
        addOldPrefixMap(ss);

        //calculate entropy
		double entropyValue=0;
		for(map<string, int>::iterator v= m_map->begin(); v != m_map->end(); v++)
		{
			double vp = (double)v->second/m_vec->size();
			entropyValue+=-vp*log(vp)/log(2);
		}
		m_entropy->push_back(entropyValue);
		newEnp = entropyValue;

		if(entropyValue>7)
		{
			//KLDiv(m_nowPrefixMap, m_oldPrefixMap);
			//string str=GetAttackPrefix();
			//std::cout<<"*****warning waning warning waning warning waning*****"<<str<<std::endl;
		}
		if(eThreshold>0)
		{
			//double d = (entropyValue>=(eThreshold + eMinThreshold)/2) ? (entropyValue - alpha*eThreshold):(eMinThreshold - entropyValue);
			double d = entropyValue - alpha*eThreshold;
			
			//double d = entropyValue-1.01*eThreshold;
			cumulateEntropy += d;
			cumulateEntropy = cumulateEntropy>0 ? cumulateEntropy:0;
		}
        if( cumulateEntropy > cThreshold && attackPrefix.empty())
            attackPrefix=GetAttackPrefix();
    }
    else                                               //m_vec->size() < m_wind+m_deltaN
    {
        m_vec->push_back(ss);
        if (m_vec->size()<m_deltaN)                    //        m_vec->size() < m_deltaN
        {
            //m_nowPrefixMap add
            addNowPrefixMap(ss);
        }
        else if (m_vec->size()<=m_wind)                 //        m_vec->size() < m_wind
        {
            //m_oldPrefixMap add
            addOldPrefixMap(m_vec->at(m_vec->size()-m_deltaN));
            //m_nowPrefixMap add
            addNowPrefixMap(ss);
        }
        else                                           //        m_vec->size() >= m_wind
        {
            //m_oldPrefixMap add
            addOldPrefixMap(m_vec->at(m_vec->size()-m_deltaN));
            //m_nowPrefixMap del
            delNowPrefixMap(m_vec->at(m_vec->size()-m_wind));
            //m_nowPrefixMap add
            addNowPrefixMap(ss);
        }
    }
}


void Entropy::Del(char *s)
{

}

void Entropy::PrintVec()
{
	vector<string>::iterator v = m_vec->begin();
	while( v != m_vec->end())
	{
		cout << "value of v = " << *v << endl;
		v++;
	}
}
void Entropy::PrintMap()
{
	map<string, int>::iterator v= m_map->begin();
	while( v != m_map->end())
	{
		cout <<v->first.c_str()<<":\t"<<v->second<< endl;
		v++;
	}
}
void Entropy::PrintOldPrefixMap()
{
	map<string, int>::iterator v= m_oldPrefixMap->begin();
	while( v != m_oldPrefixMap->end())
	{
		cout <<v->first.c_str()<<":\t"<<v->second<< endl;
		v++;
	}
}

void Entropy::PrintNowPrefixMap()
{
	map<string, int>::iterator v= m_nowPrefixMap->begin();
	while( v != m_nowPrefixMap->end())
	{
		cout <<v->first.c_str()<<":\t"<<v->second<< endl;
		v++;
	}
}

void Entropy::PrintEntropy()
{
	if(m_vec->size() < m_wind)
	{
		cout<<" vec size < wind"<<endl;
	}
	else
	{
		vector<double>::iterator v = m_entropy->begin();
		cout << " Entropy = ";
		while( v != m_entropy->end())
		{
			cout <<" "<<*v;
			v++;
		}
		cout<<endl;
	}
}



void Entropy::SetWind(unsigned int w)
{
	m_wind=w;
}
int Entropy::GetWind()
{
	return m_wind;
}
void Entropy::SetEthreshold(double d)
{
	eThreshold=d;
}

void Entropy::SetEthreshold()
{
	if(m_entropy->size()>0)
	{
		eThreshold = *std::max_element(m_entropy->begin(), m_entropy->end());
		eMinThreshold = *std::min_element(m_entropy->begin(), m_entropy->end());
	}
		
}

double Entropy::GetLatestEntropy()
{
	if(m_entropy->size()>0)
	{
		return m_entropy->back();
	}
	else
	{
		return 0.0;
	}
}
vector<double>* Entropy::GetEntropy()
{
	return m_entropy;
}
string Entropy::GetAttackPrefix()
{
	if(cumulateEntropy < cThreshold)
		return "ABC";
	map<double, string> *m_KLDivMap = new map<double, string>();
	double o_kldiv = KLDiv(m_nowPrefixMap, m_oldPrefixMap);

	map<string, int>::iterator v;
	map<string, double>::iterator fv;
	// now'||old
	for(v=m_oldPrefixMap->begin(); v!=m_oldPrefixMap->end(); v++)
	{
		map<string, int>::iterator v_temp=m_nowPrefixMap->find(v->first);
		if(v_temp!=m_nowPrefixMap->end())
		{
			int now_value=v_temp->second;
			v_temp->second=v->second;
			m_KLDivMap->insert(pair<double, string>(fabs(KLDiv(m_nowPrefixMap, m_oldPrefixMap)-o_kldiv), v_temp->first));
			v_temp->second=now_value;
		}
	}
	map<double, string>::iterator vd;

	for(vd=m_KLDivMap->begin(); vd!=m_KLDivMap->end(); vd++)
	{
		//std::cout<<"*****in m_KLDivMap, element= "<<vd->first<<"\tpro= "<<vd->second<<std::endl;
	}

	string str = (--vd)->second;
	delete m_KLDivMap;
	return str;
}
double Entropy::KLDiv(map<string, int> *p, map<string, int> *q)
{
    map<string, double>* pPro= new map<string, double>();
	map<string, double>* qPro= new map<string, double>();
	map<string, int>::iterator v;
	map<string, double>::iterator fv;
	//pPro
	for(v=p->begin(); v!=p->end(); v++)
	{
		double vp = (double)v->second/m_wind;
		pPro->insert(pair<string, double>(v->first, vp));
		//std::cout<<"*****in p, element= "<<v->first<<"\tpro= "<<vp<<std::endl;
	}
	//qPro
	for(v=q->begin(); v!=q->end(); v++)
	{
		double vp = (double)v->second/m_wind;
		qPro->insert(pair<string, double>(v->first, vp));
		//std::cout<<"*****in q, element= "<<v->first<<"\tpro= "<<vp<<std::endl;
	}

	//KLDiv
	double kldiv=0;
	for(fv=pPro->begin();fv!=pPro->end();fv++)
	{
		map<string, double>::iterator it_temp;
		it_temp=qPro->find(fv->first);
		if(it_temp!=qPro->end())
		{
			kldiv+=fv->second*(log(fv->second)/log(2) - log(it_temp->second)/log(2));
		}
		else
		{
			//std::cout<<"*****element "<<fv->first<<" are not appear in q"<<std::endl;
			delete pPro;
			delete qPro;
			return -1000;
		}
	}
	delete pPro;
	delete qPro;
	return kldiv;
}
}
//}
//}
