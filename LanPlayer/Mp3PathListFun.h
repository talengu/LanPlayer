#include<stdio.h>
#include <windows.h>

#include<io.h>//ʹ��io.hͷ�ļ���������_findfirst(),_findnext()����

#define PATH "*.mp3"//����ʱ��
//#define PATH "*.mp3"

////////////////////////////////////��Ҫ����
void Mp3FinalList(char *p[200]);
//////////////////////////////////


//��ȡ��ǰĿ¼��mp3������
int Nameofmp3CurrentPath(char *mp3currentpathall[200])
{
	//���200��MP3�ļ�
	long file;
	struct _finddata_t find;
	int num=0;

	while((file=_findfirst(PATH, &find))==-1L)
	{
		printf("Null!No mp3\n");
		printf("Put mp3 in current path,then press Enter\n");
		fflush(stdin);
		getchar();
	}

	mp3currentpathall[0]=(char *)malloc(sizeof(find.name));
	strcpy(mp3currentpathall[0], find.name);
	while(_findnext(file, &find)==0)
	{
		num++;
		mp3currentpathall[num]=(char *)malloc(sizeof(find.name));
		strcpy(mp3currentpathall[num], find.name);
		//printf("%d:%s\n", num,find.name);

	}
	_findclose(file);
	return num;
}
//���ļ���ӡ
void Mp3printf(int num,char*p[])
{
	printf("---------------------\n");
	for(int i=0;i<=num;i++)
		printf("%d.%s\n",i,p[i]);
	printf("---------------------\n");
}
//��mp3˳���������
void Mp3LisPpai(int ch,int num,char*p[])
{
	if(ch<num&&ch>=0)
	{
		char *temp=p[ch];
	for(int i=0;i<ch;i++)
		p[ch-i]=p[ch-1-i];
	p[0]=temp;
	}
	else
		printf("0-%d\n",num);
}
//��Ҫ�ĺ���
void Mp3FinalList(char *p[200])
{
	int num=Nameofmp3CurrentPath(p);
	int ch=0;
	Mp3printf(num,p);
	while(ch!=-1)
	{    
		fflush(stdin);
		printf("Input -1 confirm>>");
		scanf("%d",&ch);
		Mp3LisPpai(ch,num,p);
		Mp3printf(num,p);
	}
}