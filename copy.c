#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
int main(int aryc, char** aryv)
{
	if(aryc < 4)
	{
		printf("参数过少\n");
		return 0;
	}
	int rfd = open(aryv[1], O_RDONLY);
	int wfd = open(aryv[2], O_CREAT | O_RDWR, 0644);
	//获取文件大小和切片大小
	int size = lseek(rfd, 0, SEEK_END);
	int partSize = size / atoi(aryv[4]);
	int bufferSize = 8192;
	char buffer[bufferSize];
	bzero(buffer, sizeof(buffer));

	//偏移指针
	lseek(rfd, atoi(aryv[3]) * partSize, SEEK_SET);
	lseek(wfd, atoi(aryv[3]) * partSize, SEEK_SET);

	//复制 
	while(partSize > 0)
	{
		if(partSize > bufferSize)
		{
			read(rfd, buffer, bufferSize);
			write(wfd, buffer, bufferSize);
		}
		else
		{
			read(rfd, buffer, partSize);
			write(wfd, buffer, partSize);
		}
		partSize -= bufferSize;
	}
	close(rfd);
	close(wfd);
	exit(0);
}
