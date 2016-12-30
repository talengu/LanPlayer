
#include"Conf.h"//���ÿ�����

DWORD WINAPI client_broadcastthread(LPVOID);

DWORD WINAPI client_listerthread(LPVOID);

DWORD WINAPI client_senderthread(LPVOID);

//������������
void jiexi(char Message[MSGSIZE]);

DWORD WINAPI mp3receiver(LPVOID lpParamter);//�����ļ��߳�
int lclient();

/*******************************************************************************
* ������        : DWORD WINAPI client_broadcastthread(LPVOID)
* ��������    	: �ͻ�����7001�ڽ��չ㲥,���serverIP����client_senderthread�߳�
* �������      : ��
* ������     	: ��������SOCKADDR_IN  c_serverbroadcastaddr
* ����ֵ        : ��                                                   TEST_OK
*******************************************************************************/
SOCKADDR_IN Client_Broadcast_Geted_Serveraddr;//client�ӹ㲥���õ�serveraddr
DWORD WINAPI client_broadcastthread(LPVOID)
{  
	//���ȼ���service��broadcast
	WSADATA     c_broadcastwsaData; 
	SOCKET c_broadcastsocket;
	SOCKADDR_IN c_localsockaddr;
	char cRecvBuff[MSGSIZE];
	int nSize,nbSize;

	// ��ʼ��winsock�⣬ʹ��socket��ǰ��
	if ( WSAStartup( MAKEWORD(2,2), &c_broadcastwsaData ) != NO_ERROR )
		ErrorMessageprint("Error at WSAStartup()");


	c_broadcastsocket=socket(AF_INET, SOCK_DGRAM,0);

	c_localsockaddr.sin_family = AF_INET;
	c_localsockaddr.sin_port = htons(7001);//���Ͷ�ʹ�õķ��Ͷ˿ڣ����Ը�����Ҫ����
	c_localsockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// �󶨵�7001�˿ڣ��Լ����������������
	if(bind( c_broadcastsocket, (SOCKADDR FAR *)&c_localsockaddr, sizeof(SOCKADDR_IN))==SOCKET_ERROR)
		ErrorMessageprint("Can't bind socket to local port!Program stop.\n");

	//������Ϣ
	while(TRUE)
	{
		nSize = sizeof ( SOCKADDR_IN );

		//��nbSize��¼���յ��������С
		//recvfrom(
		//  _In_ SOCKET s,
		//  _Out_writes_bytes_to_(len, return) __out_data_source(NETWORK) char FAR * buf,
		//  _In_ int len,
		//  _In_ int flags,
		//  _Out_writes_bytes_to_opt_(*fromlen, *fromlen) struct sockaddr FAR * from,
		//  _Inout_opt_ int FAR * fromlen
		//  );
		if((nbSize=recvfrom (c_broadcastsocket,cRecvBuff,MSGSIZE,0,(SOCKADDR FAR *) &Client_Broadcast_Geted_Serveraddr,&nSize))==SOCKET_ERROR)
		{
			ErrorMessageprint("Broadcast recive Error");
			break;
			//exit(1);
		}
		cRecvBuff[nbSize] = '\0';
		TestMessageprint("%s",cRecvBuff);
		if(nbSize>2)//��Ϊ�������㲥����Ϣ����2 ��Message X��
			//��ӡ�����㲥�������ĵ�ַ�Ͷ˿�
		{ 
			printf("Find A Server!\n");
			TestMessageprint("Server IP_and_PORT:%s:%d", inet_ntoa(Client_Broadcast_Geted_Serveraddr.sin_addr), ntohs(Client_Broadcast_Geted_Serveraddr.sin_port));

			//����client�����߳�
			//////////////////////////////////////////////////////////////////////////////////
			HANDLE client_sender_handle=CreateThread(NULL,0,client_senderthread,0,0,0);
			CloseHandle(client_sender_handle);

			///////////////////////////////////////////////////////////////////////////////////
			break;
			//�˳�client���չ㲥���߳�
		}
	}

	// �ͷ����Ӻͽ��н�������
	closesocket(c_broadcastsocket);
	WSACleanup();
	return 0;
}
/*******************************************************************************
* ������        : DWORD WINAPI client_senderthread(LPVOID)
* ��������    	: �ͻ��˷�����Ϣ��������
* �������      : ��
* ������     	: SOCKADDR_IN Client_Broadcast_Geted_Serveraddr
* ����ֵ        : ��
*******************************************************************************/

SOCKET ClientSocket;
DWORD WINAPI client_senderthread(LPVOID)
{  

	WSADATA     c_listerwsaData; 

	SOCKADDR_IN serveraddr;

	char      c_Message[MSGSIZE];

	// ��ʼ��winsock�⣬ʹ��socket��ǰ��
	if ( WSAStartup( MAKEWORD(2,2), &c_listerwsaData ) != NO_ERROR )
		ErrorMessageprint("Error at WSAStartup()");


	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    

	memset(&serveraddr, 0, sizeof(SOCKADDR_IN));               //�Ƚ������ַ��server��Ϊȫ0
	serveraddr.sin_family = PF_INET;                           //������ַ��ʽ��TCP/IP��ַ��ʽ
	serveraddr.sin_port = htons(CONTROLPORT);                         //ָ�����ӷ������Ķ˿ں�
	//printf("Accepte server:%s:%d\n", inet_ntoa(saClient.sin_addr), ntohs(saClient.sin_port));//����þ�
	serveraddr.sin_addr= Client_Broadcast_Geted_Serveraddr.sin_addr;    //ָ�����ӷ�������IP��ַ
	if (SOCKET_ERROR==connect( ClientSocket, (struct sockaddr *)&serveraddr, sizeof(SOCKADDR_IN))) //�����ղ�ָ���ķ�������
	{
		ErrorMessageprint("Can't connect Server.");//��ʼ��ʧ�ܷ���-1
		//����server�˵Ŀͻ��ѵ����ֵ
	}
	TestMessageprint("connect server success");

	/////////////////////////////////////////////////////////////////////////
	HANDLE client_lister_handle=CreateThread(NULL,0,client_listerthread,0,0,0);
	CloseHandle(client_lister_handle);
	////////////////////////////////////////////////////////////////////////



	while (TRUE)
	{
		fflush(stdin);
		//printf(">>");
		gets(c_Message); 
		if(SOCKET_ERROR==send( ClientSocket, c_Message, strlen(c_Message), 0))
			ErrorMessageprint("send() failed");

		// printf("[SendMessage_OK]\n");
		// if(!strcmp(c_Message,"exit"))//client�����˳�
		//  exit(1);
	}

	// �ͷ����Ӻͽ��н�������
	closesocket( ClientSocket);
	WSACleanup();
	getchar();
	return 0;
}

/*******************************************************************************
* ������        : DWORD WINAPI client_listerthread(LPVOID)
* ��������    	: ��Ҫ��ClientSocket
* �������      : ��
* ������     	: 
* ����ֵ        : ��
*******************************************************************************/
DWORD WINAPI client_listerthread(LPVOID)
{
	char   s_Message[MSGSIZE];
	int    ret;
	while (TRUE)
	{
		ret = recv(ClientSocket, s_Message, MSGSIZE, 0);
		if(SOCKET_ERROR==ret)
		{
			printf("Server is closed!\n");
			printf("Waiting for another server...\n");
			//////////////////////////////
			//////////////////////////
			//////////////////probrom
			lclient();
			////////////////////////
			/////////////////
			//////////////////
		}
		s_Message[ret] = '\0';  
		printf("Server: %s\n", s_Message);
		//////////////////////////////////////
		jiexi(s_Message);
		//////////////////////////////////////
	}
	closesocket(ClientSocket);
	WSACleanup();
}
/*******************************************************************************
* ������        : lclient()
* ��������    	: �ͻ�������
* �������      : ��
* ������     	: ��
* ����ֵ        : ��
*******************************************************************************/
int lclient(){
	HANDLE client_broadcast_handle=CreateThread(NULL,0,client_broadcastthread,0,0,0);
	CloseHandle(client_broadcast_handle);


	///////////////////////////////////////////////////////////////////
	/*HANDLE client_lister_handle=CreateThread(NULL,0,client_listerthread,0,0,0);
	CloseHandle(client_lister_handle);*/
	////////////////////////////////////////////////////////////////////
	while(1)
	{}
	getchar();
	return 0;
}
//������������
void jiexi(char Message[MSGSIZE])
{
	char outmessage[1024];
	     //��������
	      // Mp3ChooseFun3("temp.mp3",Message[0],outmessage,atoi(Message+1));
		   Mp3ChooseFun3("temp.mp3",Message[0],outmessage,atoi(Message+1));
		   if(outmessage!=NULL)
		if(SOCKET_ERROR==send( ClientSocket, outmessage, strlen(outmessage), 0))
			ErrorMessageprint("send() failed");
		    memset(outmessage, 0, strlen(outmessage) );//�������
		if(Message[0]=='@')//����
	{
		        // Mp3Close();
				 if(NowReceiveRun())
				{HANDLE hThread=CreateThread(NULL,0,mp3receiver,0,0,0);//���������߳�
				 CloseHandle(hThread);}	
		
	}
		if(Message[0]=='x')//����impottant
	{
				 Mp3Close();
	}
}
// #q==����mp3
// @pxxxx==����mp3�Ͷ˿�
//

// #q==����mp3
// @pxxxx==����mp3�Ͷ˿�
//
DWORD WINAPI mp3receiver(LPVOID)//�����ļ��߳�
{
	travalreceiver("temp.mp3");
	return 0;
}