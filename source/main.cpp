////////////////////////////////////////////////////////////////////////////////
//  File Name: main.cpp
//
//  Functions:
//      SDK测试DEMO-纯接收端.
//
//  History:
//  Date        Modified_By  Reason  Description	
//
////////////////////////////////////////////////////////////////////////////////
#include "SDCommon.h"
#include "SDLog.h"
#include "SDIniFile.h"
#include "SDClient.h"
#include "SDConsoleIFace.h"
#include "SDTerminalSdk.h"

#if defined(WIN32) && defined(_DEBUG_)
#include <afx.h>
#endif

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <signal.h>
#endif



#define SECTION_CONFIG					"Config"
#define KEY_NAME_MEDIA_SERVER_IP		"ServerIp"			//远端服务器IP
#define KEY_DOMAIN_ID					"DomainId"			//服务器域ID	
#define KEY_NAME_ROOM_ID				"RoomId"			//远端服务器IP
#define KEY_NAME_DOWN_POSITION			"DownPosition"		//接收位置
#define KEY_NAME_RECV_FILE_SAVE			"SaveRecvData"		//是否保存收到的数据到文件


CSDClient	g_Client;
BOOL		g_bSaveRecvData				= TRUE;


//主函数
int main(int argc, char *argv[])
{
	int nRet = 0;

	//初始化DEMO日志模块
	char strIniFileName[MAX_PATH];
	GetSameExeFile(strIniFileName, ".ini");
	SDLOG_INIT("./log", SD_LOG_LEVEL_INFO, strIniFileName);


	//读取配置
	CSDIniFile *pIniFile = new CSDIniFile;
	pIniFile->ReadIniFile(strIniFileName);

	//服务器IP
	char strServerIp[64];
	memset(strServerIp, 0x0, sizeof(strServerIp));
	pIniFile->SDGetProfileString(SECTION_CONFIG, KEY_NAME_MEDIA_SERVER_IP, strServerIp, 64);

	//服务器域ID
	UINT unDomainId = (UINT)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_DOMAIN_ID, 1);

	//用户类型
	USER_ONLINE_TYPE eUserType = USER_ONLINE_TYPE_AV_RECV_ONLY;
	//用户ID，本DEMO使用随机数，实际业务建议使用有管理的唯一ID
	UINT unSec = 0;
	UINT unUsec = 0;
	SD_GetCurrentTime(unSec, unUsec);
	UINT unSeed = unSec^unUsec;
	SD_srandom(unSeed);
	UINT unUserId = SD_random32();

	//房间ID
	UINT unRoomId = (UINT)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_ROOM_ID, 1000);

	//接收位置
	BYTE byRecvPosition = (BYTE)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_DOWN_POSITION, 255);

	//是否保存收到的文件
	g_bSaveRecvData = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_RECV_FILE_SAVE, FALSE) != 0 ? TRUE:FALSE;

	delete pIniFile;
	
	//配置有效性校验
	if (strServerIp[0] == 0)
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "Please setup right server ip!");
		SDLOG_CLOSE();
		return FALSE;
	}

	SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "ServerIp:%s  DomainId:%d  RoomId:%u  UserId:%u  DownPosition:%d!", strServerIp, unDomainId, unRoomId, unUserId, byRecvPosition);

	//SDK环境初始化，整个系统中仅需调用一次
	SDTerminal_Enviroment_Init("./log", LOG_OUTPUT_LEVEL_INFO);

	//启动测试服务
	if (!g_Client.Start(strServerIp, unDomainId, unRoomId, unUserId, eUserType, byRecvPosition))
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "Test start fail...");
		SDTerminal_Enviroment_Free();
		SDLOG_CLOSE();
		return FALSE;
	}
	else
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "sdk test start success...input exit to end the test");
	}

	//循环
	CSDConsleIFace::RunCommandLine("sdk_test");

	g_Client.Close();
	
	SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "sdk test over success...");
	SDTerminal_Enviroment_Free();
	SDLOG_CLOSE();

	return TRUE;
}

