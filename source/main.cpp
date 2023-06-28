////////////////////////////////////////////////////////////////////////////////
//  File Name: main.cpp
//
//  Functions:
//      SDK����DEMO-�����ն�.
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
#define KEY_NAME_MEDIA_SERVER_IP		"ServerIp"			//Զ�˷�����IP
#define KEY_DOMAIN_ID					"DomainId"			//��������ID	
#define KEY_NAME_ROOM_ID				"RoomId"			//Զ�˷�����IP
#define KEY_NAME_DOWN_POSITION			"DownPosition"		//����λ��
#define KEY_NAME_RECV_FILE_SAVE			"SaveRecvData"		//�Ƿ񱣴��յ������ݵ��ļ�


CSDClient	g_Client;
BOOL		g_bSaveRecvData				= TRUE;


//������
int main(int argc, char *argv[])
{
	int nRet = 0;

	//��ʼ��DEMO��־ģ��
	char strIniFileName[MAX_PATH];
	GetSameExeFile(strIniFileName, ".ini");
	SDLOG_INIT("./log", SD_LOG_LEVEL_INFO, strIniFileName);


	//��ȡ����
	CSDIniFile *pIniFile = new CSDIniFile;
	pIniFile->ReadIniFile(strIniFileName);

	//������IP
	char strServerIp[64];
	memset(strServerIp, 0x0, sizeof(strServerIp));
	pIniFile->SDGetProfileString(SECTION_CONFIG, KEY_NAME_MEDIA_SERVER_IP, strServerIp, 64);

	//��������ID
	UINT unDomainId = (UINT)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_DOMAIN_ID, 1);

	//�û�����
	USER_ONLINE_TYPE eUserType = USER_ONLINE_TYPE_AV_RECV_ONLY;
	//�û�ID����DEMOʹ���������ʵ��ҵ����ʹ���й����ΨһID
	UINT unSec = 0;
	UINT unUsec = 0;
	SD_GetCurrentTime(unSec, unUsec);
	UINT unSeed = unSec^unUsec;
	SD_srandom(unSeed);
	UINT unUserId = SD_random32();

	//����ID
	UINT unRoomId = (UINT)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_ROOM_ID, 1000);

	//����λ��
	BYTE byRecvPosition = (BYTE)pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_DOWN_POSITION, 255);

	//�Ƿ񱣴��յ����ļ�
	g_bSaveRecvData = pIniFile->SDGetProfileInt(SECTION_CONFIG, KEY_NAME_RECV_FILE_SAVE, FALSE) != 0 ? TRUE:FALSE;

	delete pIniFile;
	
	//������Ч��У��
	if (strServerIp[0] == 0)
	{
		SDLOG_PRINTF("Test", SD_LOG_LEVEL_ERROR, "Please setup right server ip!");
		SDLOG_CLOSE();
		return FALSE;
	}

	SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "ServerIp:%s  DomainId:%d  RoomId:%u  UserId:%u  DownPosition:%d!", strServerIp, unDomainId, unRoomId, unUserId, byRecvPosition);

	//SDK������ʼ��������ϵͳ�н������һ��
	SDTerminal_Enviroment_Init("./log", LOG_OUTPUT_LEVEL_INFO);

	//�������Է���
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

	//ѭ��
	CSDConsleIFace::RunCommandLine("sdk_test");

	g_Client.Close();
	
	SDLOG_PRINTF("Test", SD_LOG_LEVEL_INFO, "sdk test over success...");
	SDTerminal_Enviroment_Free();
	SDLOG_CLOSE();

	return TRUE;
}

