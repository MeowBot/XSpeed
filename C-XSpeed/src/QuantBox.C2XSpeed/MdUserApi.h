#pragma once

#include "QuantBox.C2XSpeed.h"
#include "include\XSpeed\DFITCMdApi.h"

#include <list>
#include <set>
#include <map>
#include <string>

using namespace std;
using namespace DFITCXSPEEDMDAPI;

class CXSpeedMsgQueue;

class CMdUserApi :
	public DFITCMdSpi
{
	//�������ݰ�����
	enum RequestType
	{
		E_Init,
		E_UserLoginField,
	};

	//�������ݰ��ṹ��
	struct SRequest
	{
		RequestType type;
		void* pBuf;
	};

public:
	CMdUserApi(void);
	virtual ~CMdUserApi(void);

	void RegisterMsgQueue(CXSpeedMsgQueue* pMsgQueue);

	void Connect(
		const string& szSvrAddr,
		const string& szAccountID,
		const string& szPassword,
		short sCompanyID);
	void Disconnect();

	void Subscribe(const string& szInstrumentIDs);
	void Unsubscribe(const string& szInstrumentIDs);

private:
		//���ݰ������߳�
	friend DWORD WINAPI SendThread_MD(LPVOID lpParam);
	void RunInThread();
	void StopThread();

	//ָ�����ݰ����ͣ����ɶ�Ӧ���ݰ�
	SRequest * MakeRequestBuf(RequestType type);
	//������������������
	void ReleaseRequestListBuf();
	//����ѷ����������
	void ReleaseRequestMapBuf();
	//���ָ�����������ָ����
	void ReleaseRequestMapBuf(int nRequestID);
	//��ӵ��Ѿ��������
	void AddRequestMapBuf(int nRequestID,SRequest* pRequest);
	//��ӵ������Ͱ�����
	void AddToSendQueue(SRequest * pRequest);


	//��������
	void Subscribe(const set<string>& instrumentIDs);
	int ReqInit();
	//��¼����
	void ReqUserLogin();

	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnRspUserLogin(struct DFITCUserLoginInfoRtnField *pRspUserLogin, struct DFITCErrorRtnField *pRspInfo);
	virtual void OnRspUserLogout(struct DFITCUserLogoutInfoRtnField * pRspUsrLogout, struct DFITCErrorRtnField * pRspInfo);
	virtual void OnRspError(struct DFITCErrorRtnField *pRspInfo);
	virtual void OnRspSubMarketData(struct DFITCSpecificInstrumentField *pSpecificInstrument, struct DFITCErrorRtnField *pRspInfo);
	virtual void OnRspUnSubMarketData(struct DFITCSpecificInstrumentField *pSpecificInstrument, struct DFITCErrorRtnField *pRspInfo);
	virtual void OnMarketData(struct DFITCDepthMarketDataField *pMarketDataField);
	virtual void OnRspTradingDay(struct DFITCTradingDayRtnField * pTradingDayRtnData);
	//����Ƿ����
	bool IsErrorRspInfo_Output(struct DFITCErrorRtnField *pRspInfo);//��������Ϣ�͵���Ϣ����
	bool IsErrorRspInfo(struct DFITCErrorRtnField *pRspInfo); //�������Ϣ

private:
	CRITICAL_SECTION			m_csMapInstrumentIDs;

	ConnectionStatus			m_status;				//����״̬
	long						m_lRequestID;			//����ID��ÿ������ǰ����
	
	set<string>					m_setInstrumentIDs;		//���ڶ��ĵĺ�Լ
	DFITCMdApi*					m_pApi;					//����API
	CXSpeedMsgQueue*			m_msgQueue;				//��Ϣ����ָ��

	string						m_szSvrAddr;
	string						m_szAccountID;			//Ͷ����ID
	string						m_szPassword;			//����
	short						m_sCompanyID;			//����ID

	int							m_nSleep;
	volatile bool				m_bRunning;
	HANDLE						m_hThread;

	CRITICAL_SECTION			m_csList;
	list<SRequest*>				m_reqList;				//�������������

	CRITICAL_SECTION			m_csMap;
	map<int,SRequest*>			m_reqMap;				//�ѷ��������
};

