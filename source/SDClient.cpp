////////////////////////////////////////////////////////////////////////////////
//  File Name: CSDClient.cpp
//
//  Functions:
//      客户端对象类.
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


	//设置传输相关参数
	//纯接收端需要配置的参数，JitterBuff缓存时间。若需要极低延时，可设置为0
	UINT unJitterBuffDelay = 200;
	BOOL bEnableNack = TRUE;
	//以下参数仅对发送生效
	FEC_REDUN_TYPE eFecRedunMethod = FEC_FIX_REDUN;
	UINT unFecRedunRatio = 30;
	UINT unFecMinGroupSize = 16;
	UINT unFecMaxGroupSize = 64;

	SDTerminal_SetTransParams(m_pTerminal, unJitterBuffDelay, eFecRedunMethod, unFecRedunRatio, unFecMinGroupSize, unFecMaxGroupSize, bEnableNack);

	//设置回调接口
	SDTerminal_SetSystemStatusNotifyCallback(m_pTerminal, SystemStatusNotifyCallback, this);
	SDTerminal_SetRecvRemoteVideoCallback(m_pTerminal, RecvRemoteVideoCallback, this);
	SDTerminal_SetRecvRemoteAudioCallback(m_pTerminal, RecvRemoteAudioCallback, this);


	//设置接收掩码
	UINT unMask = 0xFFFFFFFF;
	if (byRecvPosition < MAX_SUPPORT_POSITION_NUM)
	{
		//仅接收byRecvPosition位置
		unMask = 0x1 << (byRecvPosition);
	}
	else
	{
		//不接收任何数据
		unMask = 0;
	}
	SDTerminal_SetAvDownMasks(m_pTerminal, unMask, unMask);

	//登录服务器
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


// SDK回调接口实现
// 【注意事项】
//	1、通知型回调函数中应尽可能快的退出，不进行耗时操作，不调用SDTerminal系列API接口。
//  2、数据型回调函数中允许进行解码处理


//通知型回调：来自底层的状态变更反馈（比如异步登录成功、异步登录失败、启动重连、重连成功、账号被顶下去等）
void CSDClient::SystemStatusNotifyCallback(void* pObject, STATUS_CHANGE_NOTIFY unStatus)
{
	switch (unStatus) 
	{
	case STATUS_NOTIFY_EXIT_AUTH_FAILED:
		// 用户账号(uid)在登录失败：鉴权失败。本回调消息属于严重级别，应通知上层
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "online auth failed, offline...");
		break;
	case STATUS_NOTIFY_EXIT_KICKED:
		// 用户账号(uid)在其他位置登录，被踢出房间。本回调消息属于严重级别，应通知上层
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "kicked by other user, offline...");
		break;
	case STATUS_NOTIFY_RECON_START:
		// 客户端网络掉线，内部开始自动重连。可忽略本消息，若仅做日志提醒
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "starting reconnect");
		break;
	case STATUS_NOTIFY_RECON_SUCCESS:
		// 内部自动重连网络成功。可忽略本消息，若仅做日志提醒
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "reconnect success");
		break;
	}
}

//数据型回调：收到服务器发来的视频
void CSDClient::RecvRemoteVideoCallback(void* pObject, unsigned char ucPosition, unsigned char* data, unsigned int unLen, unsigned int unPTS, VideoFrameInfor* pFrameInfo)
{
	CSDClient* pClient = (CSDClient*)pObject;
	if (pClient->m_pfRecvH264File)
	{
		fwrite(data, sizeof(unsigned char), unLen, pClient->m_pfRecvH264File);
	}
	
}

//数据型回调：收到服务器发来的音频
void CSDClient::RecvRemoteAudioCallback(void* pObject, unsigned char ucPosition, unsigned char* data, unsigned int unLen, unsigned int unPTS, AudioFrameInfor* pFrameInfo)
{
	CSDClient* pClient = (CSDClient*)pObject;
	return;
}

