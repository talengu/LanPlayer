#include"HandlesServer.h"
#include"HandlesClient.h"
void gprint()
{
	printf("Server  OR  Client\n");
	printf("---------------------------\n");
	printf("s   server                 \n");
	printf("c   client                \n");
	printf("@   pass                   \n");
	printf("q   prepare                \n");
	printf("w   start or resume        \n");
	printf("e   pause                  \n");
	printf("r   set point              \n");
	printf("g   get position           \n");
	printf("l   get length             \n");
	printf("---------------------------\n");
}
int main()
{
	//printf("**\n���������趨�������ĵ�ַ���������ĵ�ַ)\n���ø�mp3����\nStep1:ѡ��server��client\nStep2:p��������\n      z�����ڴ�׼������\n      a����\n");
	//printf("      k����client�Ĳ��ŵ�\n      x�ر�����\n      eʹclient�˳�\n**\n");
	gprint();
	char key=' ';
	while(key!='s'&&key!='c')
	{    
		fflush(stdin);
		
		printf("Choose :[s/c]");
		scanf("%c",&key);
	}
	if(key=='s')
	{
		printf("Start Server...\nWaiting Client...\n");
		lserver();
	}
	if(key=='c')
	{
		printf("Start Client...\nWaiting Server...\n");
		lclient();
	}

	return 0;
}




