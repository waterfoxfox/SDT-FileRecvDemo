////////////////////////////////////////////////////////////////////////////////
//  File Name: CSDClient.cpp
//
//  Functions:
//      �ͻ��˶�����.
//
//  History:
//  Date        Modified_By  Reason  Description	
//
////////////////////////////////////////////////////////////////////////////////

#include "SDClient.h"


extern BOOL		g_bSaveRecvData;


CSDClient::CSDClient()
{
	m_pTerminal = SDTerminal_Create();
	m_bClosed = TRUE;
	m_pfRecvH264File = NULL;
}

CSDClient::~CSDClient()
{
	SDTerminal_Delete(&m_pTerminal);
	if (m_pfRecvH264File)
	{
		fclose(m_pfRecvH264File);
		m_pfRecvH264File = NULL;
	}
}


BOOL CSDClient::Start(char* strServerIp, UINT unDomainId, UINT unRoomId, UINT unUserId, USER_ONLINE_TYPE eUserType, BYTE byRecvPosition)
{

	m_bClosed = FALSE;

	if (g_bSaveRecvData)
	{
		m_pfRecvH264File = fopen("recv.h264", "wb");
		if (m_pfRecvH264File == NULL)
		{
			SDLOG_PRINTF("Test", SD_LOG_LEVEL_WARNING, "Open file for recv bitstream save failed!");
		}
	}


	//���ô�����ز���
	//�����ն���Ҫ���õĲ�����JitterBuff����ʱ�䡣����Ҫ������ʱ��������Ϊ0
	UINT unJitterBuffDelay = 200;
	BOOL bEnableNack = TRUE;
	//���²������Է�����Ч
	FEC_REDUN_TYPE eFecRedunMethod = FEC_FIX_REDUN;
	UINT unFecRedunRatio = 30;
	UINT unFecMinGroupSize = 16;
	UINT unFecMaxGroupSize = 64;

	SDTerminal_SetTransParams(m_pTerminal, unJitterBuffDelay, eFecRedunMethod, unFecRedunRatio, unFecMinGroupSize, unFecMaxGroupSize, bEnableNack);

	//���ûص��ӿ�
	SDTerminal_SetSystemStatusNotifyCallback(m_pTerminal, SystemStatusNotifyCallback, this);
	SDTerminal_SetRecvRemoteVideoCallback(m_pTerminal, RecvRemoteVideoCallback, this);
	SDTerminal_SetRecvRemoteAudioCallback(m_pTerminal, RecvRemoteAudioCallback, this);


	//���ý�������
	UINT unMask = 0xFFFFFFFF;
	if (byRecvPosition < MAX_SUPPORT_POSITION_NUM)
	{
		//������byRecvPositionλ��
		unMask = 0x1 << (byRecvPosition);
	}
	else
	{
		//�������κ�����
		unMask = 0;
	}
	SDTerminal_SetAvDownMasks(m_pTerminal, unMask, unMask);

	//��¼������
	int nRet = SDTerminal_Online(m_pTerminal, strServerIp, unDomainId, unUserId, unRoomId, eUserType);
	if (nRet < 0)
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "SDTerminal_Online Failed return:%d!", nRet);
		m_bClosed = TRUE;
		return FALSE;
	}

	return TRUE;
}


void CSDClient::Close()
{
	m_bClosed = TRUE;

	SDTerminal_Offline(m_pTerminal);

	if (m_pfRecvH264File)
	{
		fclose(m_pfRecvH264File);
		m_pfRecvH264File = NULL;
	}
}


// SDK�ص��ӿ�ʵ��
// ��ע�����
//	1��֪ͨ�ͻص�������Ӧ�����ܿ���˳��������к�ʱ������������SDTerminalϵ��API�ӿڡ�
//  2�������ͻص�������������н��봦��


//֪ͨ�ͻص������Եײ��״̬��������������첽��¼�ɹ����첽��¼ʧ�ܡ����������������ɹ����˺ű�����ȥ�ȣ�
void CSDClient::SystemStatusNotifyCallback(void* pObject, STATUS_CHANGE_NOTIFY unStatus)
{
	switch (unStatus) 
	{
	case STATUS_NOTIFY_EXIT_AUTH_FAILED:
		// �û��˺�(uid)�ڵ�¼ʧ�ܣ���Ȩʧ�ܡ����ص���Ϣ�������ؼ���Ӧ֪ͨ�ϲ�
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "online auth failed, offline...");
		break;
	case STATUS_NOTIFY_EXIT_KICKED:
		// �û��˺�(uid)������λ�õ�¼�����߳����䡣���ص���Ϣ�������ؼ���Ӧ֪ͨ�ϲ�
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "kicked by other user, offline...");
		break;
	case STATUS_NOTIFY_RECON_START:
		// �ͻ���������ߣ��ڲ���ʼ�Զ��������ɺ��Ա���Ϣ����������־����
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "starting reconnect");
		break;
	case STATUS_NOTIFY_RECON_SUCCESS:
		// �ڲ��Զ���������ɹ����ɺ��Ա���Ϣ����������־����
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "reconnect success");
		break;
	}
}

//�����ͻص����յ���������������Ƶ
void CSDClient::RecvRemoteVideoCallback(void* pObject, unsigned char ucPosition, unsigned char* data, unsigned int unLen, unsigned int unPTS, VideoFrameInfor* pFrameInfo)
{
	CSDClient* pClient = (CSDClient*)pObject;
	if (pClient->m_pfRecvH264File)
	{
		fwrite(data, sizeof(unsigned char), unLen, pClient->m_pfRecvH264File);
	}
	
}

//�����ͻص����յ���������������Ƶ
void CSDClient::RecvRemoteAudioCallback(void* pObject, unsigned char ucPosition, unsigned char* data, unsigned int unLen, unsigned int unPTS, AudioFrameInfor* pFrameInfo)
{
	CSDClient* pClient = (CSDClient*)pObject;
	return;
}

