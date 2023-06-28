////////////////////////////////////////////////////////////////////////////////
//  File Name: CSDClient.h
//
//  Functions:
//      �ͻ��˶�����.
//
//  History:
//  Date        Modified_By  Reason  Description	
//
////////////////////////////////////////////////////////////////////////////////

#if !defined(_SDCLIENT_H)
#define _SDCLIENT_H

#include "SDCommon.h"
#include "SDLog.h"
#include "SDTerminalSdk.h"


class CSDClient
{
public:
	CSDClient();
	virtual ~CSDClient();

public:
	BOOL Start(char* strServerIp, UINT unDomainId, UINT unRoomId, UINT unUserId, USER_ONLINE_TYPE eUserType, BYTE byRecvPosition);
	void Close();
	
private:
	// SDK�ص��ӿ�ʵ��
	//���Եײ��״̬��������������첽��¼�ɹ����첽��¼ʧ�ܡ����������������ɹ����˺ű�����ȥ�ȣ�
	static void SystemStatusNotifyCallback(void* pObject, STATUS_CHANGE_NOTIFY unStatus);

	// �յ���������������Ƶ
	static void RecvRemoteVideoCallback(void* pObject, unsigned char ucPosition, unsigned char* data, unsigned int unLen, unsigned int unPTS, VideoFrameInfor* pFrameInfo);

	// �յ���������������Ƶ
	static void RecvRemoteAudioCallback(void* pObject, unsigned char ucPosition, unsigned char* data, unsigned int unLen, unsigned int unPTS, AudioFrameInfor* pFrameInfo);


private:
	void*				 m_pTerminal;

	BOOL				 m_bClosed;
	FILE*				 m_pfRecvH264File;

};

#endif // !defined(_SDCLIENT_H)
