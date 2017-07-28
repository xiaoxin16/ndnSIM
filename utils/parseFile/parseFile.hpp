/**
 @file parseFile.hpp
 @brief parse etc para
 
 @author xyh
 @version 0.10
 @date 2017-02-06
*/
#ifndef PARSE_FILE_H
#define PARSE_FILE_H

#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>   
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <vector>
#include <algorithm>

#define DEFAULTDIR "src/ndnSIM/examples/"
#define ETCDIR "etc"
#define CONFIGFILE "config.xml"

class ConfigurationParameter{
public:
	ConfigurationParameter();
	~ConfigurationParameter();

public:
    ///note for readConf()
	/**
	@param none
	@return the result of readConf
	*/
	bool readConf();
	char * getConfigFilePath();
	char * getScenario();
	char * getTraceDir();
	char * getTopoFilePath();
	char * getCsStrategy();
	int getCsSize();
	char *getPrefix();
	char *getFwStrategy();
	char *getConsumerType();
	int getDetectStrategy();
	int getPitSize();
	int getPitExpiredTh();
	double getTheta();
	uint32_t getWindow();
	///note for getAlpha()
	/**
	@param none
	@return the result of getAlpha
	*/
	double getAlpha();
	double getCumulateTh();
	
	char *getFrequency();
	char *getContents();
	char *getDataSize();
	char *getProducerType();
	int getGoodPrefixNum();
	int getAttackPrefixNum();
	char *getAttackFrequency();
	char *getAttackContents();	
	double getAttackStartTime();
	double getAttackPeriod();
	double getAttackBurstT();
	double getAttackDelay();
	double getAttackRatio();
	double getAttackStopTime();	
	
	double getStartTime();
	double getStopTime();

private:
	char configFilePath[512];                   /** 配置文件位置 */
	char scenario[512];                         /** 场景名字 */
	char trace_dir[512];                         /** trace路径 /usr/local/src/trace/ */
	char topo_file_path[512];                   /** 拓扑文件位置 */
	char cs_strategy[512];                      /** 缓存策略 */
	int cs_size;                                /** 缓存大小 */
	char prefix[512];                           /** 前缀 */
	char fw_strategy[512];                      /** 转发策略 */
	char consumer_type[512];                    /** 请求类型 */
	
	int detect_strategy;                        /** 检测策略 */
	int pit_size;                               /** PIT大小 */
	int pit_expired_th;                         /** PIT超时率 */
	double theta;                               /** 场景名字 */
	uint32_t entropy_window;                     /**window*/
	double alpha;                               /** 正常熵门限系数 */
	double cumulate_th;                         /** 累积熵值 */
	 
	char frequency[10];                         /** 正常请求频率 */
	char contents[10];                          /** 正常内容数目 */
	char data_size[10];                         /** 数据包大小 */
	char producer_type[512];                    /** 服务类型 */
	int good_prefix_num;                        /** 正常前缀数目 */
	
	int attack_prefix_num;                      /** 攻击前缀数目 */
	char attack_frequency[10];                  /** 攻击频率 */
	char attack_contents[10];                   /** 攻击内容数目 */
	double attack_start_time;                   /** 攻击开始时间 */
	double attack_stop_time;                    /** 攻击停止时间 */
	double attack_period;                       /** 攻击周期 */
	double attack_burstT;                       /** 攻击突发长度 */
	double attack_delay;                        /** 攻击server回复时延 */
	double attack_ratio;                        /** 攻击用户占比 */
	
	double start_time;                          /** 仿真开始时间 */
	double stop_time;                           /** 仿真停止时间 */
};

#endif

