//TEST_OK
#include<stdio.h>
#include <winsock2.h>
#include <vfw.h>
#pragma comment(lib,"vfw32.lib")//for play mp3
#include<atlconv.h>//�����������

#define MPREPARE 'a'
#define MSTART 's'
#define MPAUSE 'd'
#define MRESTART 'f'
#define MSETPOINT 'g'
#define MHELP 'h'

/*ȫ�ֱ���*/

HWND hWnd=NULL;//���ֵ��߳�

//����ͨ��Mp3SetPath������ʼ��
char *mp3path=NULL;//mp3�ĵ�ַ

void Mp3Prepare();
void Mp3Start();
void Mp3Pause();
void Mp3Restart();
long long int Mp3GetPosition();
void Mp3Close();
void Mp3Setpoint();
void Mp3Setpoint2(int percent);
HWND Mp3GethWnd();
void Mp3ChooseFun1(char temp,char command,void (*fun)());
void Mp3SetPath(char *p);
//�����ʼ�� �����رտ��ܵ�hWnd ��ֵ ׼����mp3��Դ
void Mp3SetPath2(char *p);
void Mp3PrintCommand();
////�������� a s d f g
void Mp3ChooseFun2(char temp);

void Mp3ChooseFun3(char temp);
void Mp3Setpoint3(int point);
//����hWnd�����mp3�ļ��Ĺ���
void Mp3Prepare()

{
	if(mp3path!=NULL)
	{
		if(hWnd == NULL)
		{

			USES_CONVERSION;  //��
			hWnd = MCIWndCreate(NULL,NULL,WS_DISABLED,A2W(mp3path));
			TestMessageprint("Prepared OK\n");
			MCIWndPlayFrom(hWnd,0);
			MCIWndPause(hWnd);
		}
		else
			TestMessageprint("Prepared OK or Playing\n");
	}
	else
		TestMessageprint("mp3path NULL\n");

}

//MP3��ʼ���Ż�ָ�����
void Mp3Start(){
	if(hWnd != NULL)
	{

		MCIWndResume(hWnd); TestMessageprint("playing\n");}
	else
		TestMessageprint("hwnd NULL\n");


}
//MP3��ͣ����
void Mp3Pause(){
	if(hWnd != NULL)
	{
		MCIWndPause(hWnd);TestMessageprint("Pause\n");
	}
	else
		TestMessageprint("hwnd NULL\n");
}
//MP3���²��Ų��Զ�����
void Mp3Restart(){
	if(hWnd != NULL)
	{
		MCIWndPause(hWnd);
		MCIWndPlayFrom(hWnd,0);
		MCIWndPause(hWnd);
		TestMessageprint("Restart\n");
	}
	else
		TestMessageprint("hwnd NULL\n");
}
//��ȡMP3����ʱ���
 long long int Mp3GetPosition(){
	long long int point=NULL;
	if(hWnd != NULL)
	{
		long long int point=MCIWndGetPosition(hWnd);
			TestMessageprint("\npoint:%d\n",point);
			return point;
	}
	else
		TestMessageprint("hwnd NULL\n");

	return point;
}
//�ر�MP3���ž�����ͷ���Դ
void Mp3Close(){
	if(hWnd != NULL)
	{
		MCIWndStop(hWnd);
		MCIWndClose(hWnd);
		hWnd = NULL;
		TestMessageprint("Close Success\n");
	}
	else
		TestMessageprint("hwnd NULL\n");
}
//���ò��ŵ㵫���Զ�����server
void Mp3Setpoint(){
	if(hWnd != NULL)
	{
		int percent=-1;
		MCIWndPause(hWnd);
		while(percent<0||percent>300)
		{ 
			fflush(stdin);
			TestMessageprint("0<point<300:\n");
			scanf("%d",&percent);//��ȡpoint
		}
		long point=(long)(percent*MCIWndGetLength(hWnd)/300);
		MCIWndPlayFrom(hWnd,point);
		MCIWndPause(hWnd);
		TestMessageprint("SetPoint Success\n");
	}
	else
		TestMessageprint("hwnd NULL\n");
}
//���ò��ŵ㵫�Զ�����client
void Mp3Setpoint2(int percent){
	if(hWnd != NULL)
	{
		MCIWndPause(hWnd);
		long point=(long)(percent*MCIWndGetLength(hWnd)/300);
		MCIWndPlayFrom(hWnd,point);
		//MCIWndPause(hWnd);
		TestMessageprint("SetPoint Success\n");
	}
	else
		TestMessageprint("hwnd NULL\n");
}
//���ò��ŵ㵫���Զ�����client
void Mp3Setpoint3(int point){
	if(hWnd != NULL)
	{
		MCIWndPause(hWnd);
		//long point=(long)(percent*MCIWndGetLength(hWnd)/300);
		if(point>0&&(point<MCIWndGetLength(hWnd)))
		MCIWndPlayFrom(hWnd,point);
		else
			MCIWndPlayFrom(hWnd,0);
		MCIWndPause(hWnd);
		TestMessageprint("SetPoint Success\n");
	}
	else
		TestMessageprint("hwnd NULL\n");
}
//��ȡMP3���ž��
HWND Mp3GethWnd(){
	return hWnd;
}

//ѡ�����ķ������޺���������
//temp:���ڵ�ֵ
//command:ȷ����ֵ
void Mp3ChooseFun1(char temp,char command,void (*fun)())
{
	if(temp==command)
		(*fun)();

}
//�����ʼ��
void Mp3SetPath(char *p){
	mp3path=p;
}
//�����ʼ�� �����رտ��ܵ�hWnd ��ֵ ׼����mp3��Դ
void Mp3SetPath2(char *p){
	Mp3Close();
	mp3path=p;
	Mp3Prepare();
}
//��ӡ��Ӧ����
void Mp3PrintCommand()
{
	printf("-------------------------------\n");
	printf("a  prepare mp3 resource\n");
	printf("s  play mp3 or resume mp3\n");
	printf("d  pause mp3 play\n");
	printf("f  restart mp3 play\n");
	printf("g  according point play to point\n");
	printf("h  mp3 play help\n");
	printf("-------------------------------\n");
}
//�������� a s d f g
void Mp3ChooseFun2(char temp)
{
	switch (temp)
	{
	//server
	case MPREPARE :Mp3Prepare(); break;
	case MSTART:   Mp3Start();   break;
	case MPAUSE :  Mp3Pause();   break;
	case MRESTART :Mp3Restart(); break;
	case MSETPOINT:Mp3Setpoint();break;
	case MHELP:Mp3PrintCommand();break;
	default:break;
	}

}
//�������� q w e r //pΪ����out 
//n����
void Mp3ChooseFun3(char *n,char temp,char p[1024],int point)
{
	switch (temp)
	{
	//server
	case 'q' :     Mp3SetPath2(n);  sprintf(p,"Mp3Prepare"); break;
	case 'w':      Mp3Start();               sprintf(p,"Mp3Start");   break;
	case 'e' :     Mp3Pause();               sprintf(p,"Mp3Pause");   break;
	case 'r' :     Mp3Setpoint3(point);     
		sprintf(p,"Mp3GoodStart%d",point);break;
	//case 'r' :     Mp3Setpoint3(point);      sprintf(p,"Mp3Setpoint");break;
	case 'g' :     sprintf(p,"Position:%d",Mp3GetPosition());break;
	case 'l' :     sprintf(p,"Len:%d",MCIWndGetLength(hWnd));break;	
	
	default:break;
	}

}
