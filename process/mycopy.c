#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
int main(int aryc, char** aryv)
{
	if(aryc < 2)
	{
		printf("参数过少\n");
		exit(1);
	}
	//进程数目
	int n = 10;
	if(aryc > 2)
	{
		n = atoi(aryv[3]);
		if(n > 100 || n < 0)
		{
			printf("进程数应在0 到 100 之间\n");	
			exit(1);
		}
	}
	//创建子进程
	int i;
	char nstr[10] = {0};
	sprintf(nstr, "%d", n);
	for(i = 0; i < n; i++)
	{
		pid_t pid;
		pid = fork();
		if(pid > 0)
		{
			continue;
		}
		else if(pid == 0)
		{
			char istr[10] = {0};
			sprintf(istr,"%d", i);
			execl("/home/colin/0921晚班/Linux高级编程/process/ELT","ELT", aryv[1], aryv[2], istr, nstr, NULL);
		}
		else
		{
			perror("出错\n");
			exit(1);
		}
	}

	//等待复制完成
	while(i > 0)
	{
		pid_t pid;
		pid = waitpid(-1, NULL, WNOHANG);
		{
			if(pid > 0)
				i--;
		}
	}
	return 0;
}
