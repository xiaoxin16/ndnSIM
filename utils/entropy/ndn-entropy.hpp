// mytools.h
#ifndef NDN_ENTROPY_H
#define NDN_ENTROPY_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <stdlib.h>
#include <math.h>
#include <algorithm>

using namespace std;

namespace nfd {
	
class Entropy
{
	public:
		Entropy();
		~Entropy();
		void Add(char *s);
		void Add(string s);
		void Del(char *s);
		void PrintVec();
		void PrintMap();
		void PrintOldPrefixMap();
		void PrintNowPrefixMap();
		void PrintEntropy();
		void SetWind(unsigned int w);
		void SetEthreshold(double d);
		void SetEthreshold();
		void addOldPrefixMap(string ss);
		void addNowPrefixMap(string ss);
		void delOldPrefixMap(string ss);
		void delNowPrefixMap(string ss);
        void addmMap(string ss);
		void delmMap(string ss);
		int GetWind();
		double GetLatestEntropy();
		string GetAttackPrefix();
		double KLDiv(map<string, int> *p, map<string, int> *q);
		vector<double>* GetEntropy();

		double weight;
		double alpha;
		double theta;
		double eThreshold;
		double eMinThreshold;
		double cumulateEntropy;
		double cThreshold;
		double newEnp;
		string attackPrefix;

		unsigned int m_wind;
		unsigned int m_deltaN;
		vector<double> *m_entropy;             //record entropy values

		vector<string> *m_vec;                 //record prefix pass-by,
		map<string, int> *m_map;               //record unique prefix statistics of the latest m_wind
		map<string, int> *m_oldPrefixMap;      //record old prefix receive, len = m_wind before alarm
		map<string, int> *m_nowPrefixMap;      //record now prefix receive, len = m_wind when alarm
};
}
#endif
