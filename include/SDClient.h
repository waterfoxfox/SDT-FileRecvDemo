////////////////////////////////////////////////////////////////////////////////
//  File Name: CSDClient.h
//
//  Functions:
//      客户端对象类.
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
	// SDK回调接口实现
	//来自底层的状态变更反馈（比如异步登录成功、异步登录失败、启动重连、重连成功、账号被顶下去等）
	static void SystemStatusNotifyCallback(void* pObject, STATUS_CHANGE_NOTIFY unStatus);

	// 收到服务器发来的视频
	static void RecvRemoteVideoCallback(void* pObject, unsigned char ucPosition, unsigned char* data, unsigned int unLen, unsigned int unPTS, VideoFrameInfor* pFrameInfo);

	// 收到服务器发来的音频
	static void RecvRemoteAudioCallback(void* pObject, unsigned char ucPosition, unsigned char* data, unsigned int unLen, unsigned int unPTS, AudioFrameInfor* pFrameInfo);


private:
	void*				 m_pTerminal;

	BOOL				 m_bClosed;
	FILE*				 m_pfRecvH264File;

};

#endif // !defined(_SDCLIENT_H)
