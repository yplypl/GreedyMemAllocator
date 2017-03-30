// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "StdMemManager.h"
#include "GreedyMemManager.h"

#define TEST_TIMES 100000 //test 10000 times

StdMemManager stdMemManager;

void test(unsigned long begin, unsigned long  end, unsigned long times = TEST_TIMES)
{
	gGreedyMemManager.cleanReuseBlockHitRateData();

	unsigned long* size = new unsigned long[times];

	for (int i = 0; i < times; i++)
	{
		long rand1 = rand();
		long rand2 = rand();

		size[i] = (rand1 * rand2) % (end - begin) + begin;
	}

	long start1 = GetTickCount();
	for (int i = 0; i < times; i++)
	{
		int sizeI = size[i];

		byte * buffer = (byte *)stdMemManager.malloc(sizeI);
		/*memset(buffer, 0, sizeI);*/
		stdMemManager.free(buffer);
	}


	long stdTime = GetTickCount() - start1;


	long start2 = GetTickCount();
	for (int i = 0; i < times; i++)
	{
		int sizeI = size[i];

		byte * buffer = (byte *)gGreedyMemManager.malloc(sizeI);
		/*memset(buffer, 0, sizeI);*/
		gGreedyMemManager.free(buffer);
	}


	long greedyTime1 = GetTickCount() - start2;


	printf("random bytes length begin：%ld, end:%ld, test times:%ld, system api cost：%ld, greedyMemory cost:%ld,totalMallocCount:%ld, blockReuseCount:%ld, call system malloc count:%ld\n", begin, end, times, stdTime, greedyTime1, gGreedyMemManager.totalMallocCount, gGreedyMemManager.blockReuseCount, gGreedyMemManager.totalMallocCount - gGreedyMemManager.blockReuseCount);

	delete[] size;
	gGreedyMemManager.cleanReuseBlockHitRateData();


	//sleep waite greedyMemoryManager release all the long time not used memory

	if (end >= GREEDY_MEM_MANAGE_MIN_BLOCK_SIZE)
	{
		Sleep((GREEDY_KEEP_IDLE_MEM_SECONDS + 2) * 1000);
	}


}

int _tmain(int argc, _TCHAR* argv[])
{
	

	test(1, 64);

	test(65, 1024);

	test(1025, 10240);

	test(10241, 102400);

	test(102401, 1024*1024);

	test(1024 * 1024, 5 * 1024 * 1024, 10000);
	
	printf("press any key exist\n");
	char c = getchar();
	return 0;
}

