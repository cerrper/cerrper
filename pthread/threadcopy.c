#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>

pthread_mutex_t mutex;
int rfd, wfd;
int n;
int fileSize, segSize;
char str[26];
int num, count = 0;
void * t_copy(void * arg)
{
	int i = (int)arg * segSize;
	//内存共享映射
	void * rptr = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, rfd, 0);
	void * wptr = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, wfd, 0);
	//进行拷贝
	memcpy(wptr + i, rptr + i, segSize);
	//释放映射
	munmap(rptr, fileSize);
	munmap(wptr, fileSize);
	//进度条
	pthread_mutex_lock(&mutex);

	int j;
	for(j = 0; j < num; j++)
	{
		str[(count + j) / 4] = '=';
		printf("[%-25s][%d\%]\r", str, count + j);
		usleep(10000);
		fflush(stdout);
	}

	count += j;
	pthread_mutex_unlock(&mutex);
	return NULL;
}


int main(int argc, char** argv)
{
	
	if(argc < 3)
	{
		printf("参数过少\n");
		exit(1);
	}
	//线程数目
	n = 10;
	if(argc > 3)
	{
		n = atoi(argv[3]);
		if(n > 100 || n < 0)
		{
			printf("进程数应在0 到 100 之间\n");	
			exit(1);
		}
	}
	//打开文件
	rfd = open(argv[1], O_RDWR);
	wfd = open(argv[2], O_CREAT | O_RDWR, 0664);
	//获取文件大小和切片大小
	fileSize = lseek(rfd, 0, SEEK_END);
	segSize = fileSize / n;
	ftruncate(wfd, fileSize);
	//初始化进度条
	bzero(str,sizeof(str));
	num = 100 / n;
	//创建子线程
	int i;
	pthread_t tid[100];
	
	int err;
	for(i = 0; i < n; i++)
	{
		if((err = pthread_create(&(tid[i]), NULL, t_copy, (void *)i)) > 0)
		{
			printf("PTHREAD CREATE CALL FALIED: %s", strerror(err));
			exit(0);
		}
	}

	//等待复制完成
	for(int j = 0; j < n; j++)
		pthread_join(tid[j], NULL);
	if(count <= 100)
		for(count; count <= 100; count++)
		{
		str[count / 4] = '=';
		printf("[%-25s][%d\%]\r", str, count);
		usleep(10000);
		fflush(stdout);
		}
	printf("\n复制完成\n");
	//关闭文件标识符
	close(wfd);
	close(rfd);
	return 0;
}
