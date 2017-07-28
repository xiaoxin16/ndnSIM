/**
 @file parseFile.cpp
 @brief parse etc para
 
 @author xyh
 @version 0.10
 @date 2017-02-06
*/
#include "parseFile.hpp"
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;
ConfigurationParameter::ConfigurationParameter()
{
	memset(configFilePath, 0, sizeof(configFilePath));
	memset(scenario, 0, sizeof(scenario));
	memset(trace_dir, 0, sizeof(trace_dir));
	memset(topo_file_path, 0, sizeof(topo_file_path));
	memset(cs_strategy, 0, sizeof(cs_strategy));
	memset(prefix, 0, sizeof(prefix));
	memset(fw_strategy, 0, sizeof(fw_strategy));
	memset(frequency, 0, sizeof(frequency));
	memset(contents, 0, sizeof(contents));
	memset(data_size, 0, sizeof(data_size));
	memset(producer_type, 0, sizeof(producer_type));
	memset(attack_frequency, 0, sizeof(attack_frequency));
	memset(attack_contents, 0, sizeof(attack_contents));	
}

ConfigurationParameter::~ConfigurationParameter()
{
}

char * ConfigurationParameter::getConfigFilePath()
{
	return configFilePath;
}

char * ConfigurationParameter::getTopoFilePath()
{
	return topo_file_path;
}

char * ConfigurationParameter::getScenario()
{
	return scenario;
}

char * ConfigurationParameter::getTraceDir()
{
	return trace_dir;
}

char * ConfigurationParameter::getCsStrategy()
{
	return cs_strategy;
}

int ConfigurationParameter::getCsSize()
{
	return cs_size;
}

char * ConfigurationParameter::getPrefix()
{
	return prefix;
}

char * ConfigurationParameter::getFwStrategy()
{
	return fw_strategy;
}
char *ConfigurationParameter::getConsumerType()
{
	return consumer_type;
}


int ConfigurationParameter::getDetectStrategy()
{
	return detect_strategy;
}
int ConfigurationParameter::getPitSize()
{
	return pit_size;
}

int ConfigurationParameter::getPitExpiredTh()
{
	return pit_expired_th;
}


double ConfigurationParameter::getTheta()
{
	return theta;
}

uint32_t ConfigurationParameter::getWindow()
{
	return entropy_window;
}

double ConfigurationParameter::getAlpha()
{
	return alpha;
}

double ConfigurationParameter::getCumulateTh()
{
	return cumulate_th;
}

char *ConfigurationParameter::getFrequency()
{
	return frequency;
}

char *ConfigurationParameter::getContents()
{
	return contents;
}

char *ConfigurationParameter::getDataSize()
{
	return data_size;
}
char *ConfigurationParameter::getProducerType()
{
	return producer_type;
}
int ConfigurationParameter::getGoodPrefixNum()
{
	return good_prefix_num;
}
int ConfigurationParameter::getAttackPrefixNum()
{
	return attack_prefix_num;
}
char *ConfigurationParameter::getAttackFrequency()
{
	return attack_frequency;
}

char *ConfigurationParameter::getAttackContents()
{
	return attack_contents;
}

double ConfigurationParameter::getAttackStartTime()
{
	return attack_start_time;
}

double ConfigurationParameter::getAttackPeriod()
{
	return attack_period;
}
	
double ConfigurationParameter::getAttackBurstT()
{
	return attack_burstT;
}

double ConfigurationParameter::getAttackDelay()
{
	return attack_delay;
}

double ConfigurationParameter::getAttackRatio()
{
	return attack_ratio;
}

double ConfigurationParameter::getAttackStopTime()
{
	return attack_stop_time;
}

double ConfigurationParameter::getStartTime()
{
	return start_time;
}

double ConfigurationParameter::getStopTime()
{
	return stop_time;
}



bool ConfigurationParameter::readConf()
{	

	if (snprintf(configFilePath, sizeof(configFilePath), "%s%s/%s", DEFAULTDIR, ETCDIR, CONFIGFILE) < 0){
		printf("In %s:%d, Can't not find config file %s", __FILE__, __LINE__, configFilePath);
		return false;
	}

	XMLDocument doc;
	doc.LoadFile(configFilePath);
	int errorID = doc.ErrorID();
	if (errorID){
		printf("In %s:%d, Can't not load config file %s, ErrorID=%d\n", __FILE__, __LINE__, configFilePath, errorID);
		return false;
	}

  	

	// SCENARIO
	XMLElement * pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("SCENARIO");
/**/
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value")){
			printf("In %s:%d, Can't read scenario from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

    	strcpy(scenario, pItemElement->Attribute("value"));
  	}

	// TRACE_DIR
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("TRACE_DIR");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value")){
			printf("In %s:%d, Can't read scenario from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

    	strcpy(trace_dir, pItemElement->Attribute("value"));
  	}
	
	// TOPO_FILE_PATH
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("TOPO_FILE_PATH");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value")){
			printf("In %s:%d, Can't read topo_file_path from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		strcpy(topo_file_path, pItemElement->Attribute("value"));
  	}

	// CS_STRATEGY
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("CS_STRATEGY");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value")){
			printf("In %s:%d, Can't read cs_strategy from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		strcpy(cs_strategy, pItemElement->Attribute("value"));
  	}

	// CS_SIZE
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("CS_SIZE");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read cs_size from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(cacheRoot, pItemElement->Attribute("value"));
		cs_size=atoi(pItemElement->Attribute("value"));
	}

	// PREFIX
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("PREFIX");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read prefix from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		strcpy(prefix, pItemElement->Attribute("value"));
	}
	// FW_STRATEGY
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("FW_STRATEGY");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read fw_strategy from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		strcpy(fw_strategy, pItemElement->Attribute("value"));
	}
	// CONSUMER_TYPE
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("CONSUMER_TYPE");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read consumer_type from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		strcpy(consumer_type, pItemElement->Attribute("value"));
	}
	
	// DETECTION_STRATEGY
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("DETECTION_STRATEGY");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read detect_strategy from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(cacheRoot, pItemElement->Attribute("value"));
		detect_strategy=atoi(pItemElement->Attribute("value"));
	}	
	
	// PIT_SIZE
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("PIT_SIZE");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read pit_size from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(cacheRoot, pItemElement->Attribute("value"));
		pit_size=atoi(pItemElement->Attribute("value"));
	}	

	// PIT_EXPIRED_TH
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("PIT_EXPIRED_TH");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read pit_expired_th from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(cacheRoot, pItemElement->Attribute("value"));
		pit_expired_th=atoi(pItemElement->Attribute("value"));
	}	


	// THETA
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("THETA");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read theta from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(cacheRoot, pItemElement->Attribute("value"));
		theta=atof(pItemElement->Attribute("value"));
	}	

	// WINDOW
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("ENTROPY_WINDOW");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read theta from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(cacheRoot, pItemElement->Attribute("value"));
		entropy_window=atoi(pItemElement->Attribute("value"));
	}		

	// ALPHA
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("ALPHA");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read theta from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(cacheRoot, pItemElement->Attribute("value"));
		alpha=atof(pItemElement->Attribute("value"));
	}		
	
	// CUMULATE_TH
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("CUMULATE_TH");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read cumulate_th from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(cacheRoot, pItemElement->Attribute("value"));
		cumulate_th=atof(pItemElement->Attribute("value"));
	}	


	
	// FREQUENCY
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("FREQUENCY");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read frequency from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		strcpy(frequency, pItemElement->Attribute("value"));
	}
	// CONTENTS
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("CONTENTS");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read contents from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		strcpy(contents, pItemElement->Attribute("value"));
	}

	// DATA_SIZE
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("DATA_SIZE");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read data_size from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		strcpy(data_size, pItemElement->Attribute("value"));
	}

	// PRODUCER_TYPE
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("PRODUCER_TYPE");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read producer_type from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		strcpy(producer_type, pItemElement->Attribute("value"));
	}
	
	// GOOD_PREFIX_NUM
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("GOOD_PREFIX_NUM");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read cs_size from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(cacheRoot, pItemElement->Attribute("value"));
		good_prefix_num=atoi(pItemElement->Attribute("value"));
	}

	// ATTACK_PREFIX_NUM
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("ATTACK_PREFIX_NUM");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read cs_size from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(cacheRoot, pItemElement->Attribute("value"));
		attack_prefix_num=atoi(pItemElement->Attribute("value"));
	}
	// ATTACK_FREQUENCY
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("ATTACK_FREQUENCY");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read frequency from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		strcpy(attack_frequency, pItemElement->Attribute("value"));
	}
	// ATTACK_CONTENTS
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("ATTACK_CONTENTS");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read contents from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		strcpy(attack_contents, pItemElement->Attribute("value"));
	}	
	
	// ATTACK_START_TIME
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("ATTACK_START_TIME");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read start_time from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(contentCacheRoot, pItemElement->Attribute("value"));
		attack_start_time=atof(pItemElement->Attribute("value"));
	}
	
	// ATTACK_STOP_TIME
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("ATTACK_STOP_TIME");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read stop_time from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(contentCacheRoot, pItemElement->Attribute("value"));
		attack_stop_time=atof(pItemElement->Attribute("value"));
	}
	
	// ATTACK_PERIOD
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("ATTACK_PERIOD");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read stop_time from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(contentCacheRoot, pItemElement->Attribute("value"));
		attack_period=atof(pItemElement->Attribute("value"));
	}
	
	// ATTACK_BURSTT
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("ATTACK_BURSTT");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read stop_time from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(contentCacheRoot, pItemElement->Attribute("value"));
		attack_burstT=atof(pItemElement->Attribute("value"));
	}
	
	// ATTACK_DELAY
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("ATTACK_DELAY");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read stop_time from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(contentCacheRoot, pItemElement->Attribute("value"));
		attack_delay=atof(pItemElement->Attribute("value"));
	}

	// ATTACK_RATIO
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("ATTACK_RATIO");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read stop_time from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(contentCacheRoot, pItemElement->Attribute("value"));
		attack_ratio=atof(pItemElement->Attribute("value"));
	}
	
	// START_TIMES
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("START_TIME");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read start_time from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(contentCacheRoot, pItemElement->Attribute("value"));
		start_time=atof(pItemElement->Attribute("value"));
	}
	
	// START_TIME
	pItemElement = doc.FirstChildElement("XML")->FirstChildElement("CONFIG")->FirstChildElement("STOP_TIME");
	if (NULL != pItemElement){
		if(NULL == pItemElement->Attribute("value") ){
			printf("In %s:%d, Can't read stop_time from config file %s", __FILE__, __LINE__, CONFIGFILE);
			return false;
		}

		//strcpy(contentCacheRoot, pItemElement->Attribute("value"));
		stop_time=atof(pItemElement->Attribute("value"));
	}
	
	pItemElement=NULL;
	return true;
}

