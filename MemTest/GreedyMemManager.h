#pragma once

#include <list>
#include <map>
#include <hash_map>

using namespace std;


#include <windows.h>

#include "BaseMemManager.h"

typedef unsigned char byte;

#define SafeDeleteObj(obj) \
{\
if (obj)\
{\
	delete obj;\
	obj = NULL;\
}\
}


struct CriticalSection
{
	CriticalSection()
	{
		InitializeCriticalSection(&cs);
	}
	~CriticalSection() { DeleteCriticalSection(&cs); }
	void lock() { EnterCriticalSection(&cs); }
	void unlock() { LeaveCriticalSection(&cs); }
	bool trylock() { return TryEnterCriticalSection(&cs) != 0; }

	CRITICAL_SECTION cs;
};


#define GREEDY_MALLOC_ALIGN 16 //内存对齐

const unsigned int GREEDY_MEM_MANAGE_MIN_BLOCK_SIZE = 1; //最小化的内存块，如果比这个小则不缓存，直接由操作系统申请和释放

const unsigned int GREEDY_MEM_MANAGE_MAX_BLOCK_SIZE = 1024 * 1024 * 1024 * 1; //最大化的内存块，如果比这个大则不缓存，直接由操作系统申请和释放

//有借有还，再借不难
const unsigned int GREEDY_KEEP_IDLE_MEM_SECONDS = 15; //30秒没有被重用的内存将归还给系统

const unsigned int GREEDY_CLEAN_IDLE_MEM_TIMER_INTERVAL = 2000;//单位毫秒,清理空闲内存定时器的时长

#define MAX_TOTAL_REUSE_COUNT 200000//统计命中率的时候，如果次数打到了此限制，则次数减半。

struct GreedyMemBlock;

//typedef list<GreedyMemBlock *> GREEDY_READY_BLOCKS;
//typedef list<GreedyMemBlock *>::iterator GREEDY_READY_BLOCKS_ITER;
//typedef list<GreedyMemBlock *>::reverse_iterator GREEDY_READY_BLOCKS_RITER;
//
//typedef hash_map<unsigned int, GREEDY_READY_BLOCKS *> GREEDY_READY_SIZE_BLOCK_MAP;
//typedef hash_map<unsigned int, GREEDY_READY_BLOCKS *>::iterator GREEDY_READY_SIZE_BLOCK_MAP_ITER;
//typedef hash_map<unsigned int, GREEDY_READY_BLOCKS *>::value_type GREEDY_READY_SIZE_BLOCK_MAP_VALUE_TYPE;

typedef struct SimpleListNode
{

	SimpleListNode(void * data)
	{
		this->next = NULL;
		this->pre = NULL;
		this->data = data;
	}
	SimpleListNode * next = NULL;

	SimpleListNode * pre = NULL;

	void * data = NULL;

}SimpleListNode;

typedef struct SimpleList
{
	int len = 0;

	SimpleListNode * front = NULL;

	SimpleListNode * end = NULL;

	void erase(SimpleListNode * node)
	{
		if (node == NULL)
		{
			return;
		}
		
		SimpleListNode * nodePre = node->pre;


		SimpleListNode * nodeNext = node->next;

		node->pre = NULL;

		node->next = NULL;

		
		if (nodePre)
		{
			nodePre->next = nodeNext;
		}
		else
		{
			front = nodeNext;
		}


		if (nodeNext)
		{
			nodeNext->pre = nodePre;
		}
		else
		{
			end = nodePre;
		}



		len--;
	}

	SimpleListNode * push_back_data(void * data)
	{
		return push_back_node(new SimpleListNode(data));
	}


	SimpleListNode * push_back_node(SimpleListNode * node)
	{
		if (len == 0)
		{
			front = node;
			end = node;
		}
		else
		{
			end->next = node;
			node->pre = end;
			end = node;
		}
		len++;

		return node;
	};

	SimpleListNode * push_front_data(void * data)
	{
		return push_front_node(new SimpleListNode(data));
	}

	SimpleListNode * push_front_node(SimpleListNode * node)
	{
		if (len == 0)
		{
			front = node;
			end = node;
		}
		else
		{
			front->pre = node;
			node->next = front;
			front = node;
		}

		len++;

		return node;
	};

	(void *)pop_back_data()
	{
		SimpleListNode * node = pop_back_node();
		if (node == NULL)
		{
			return NULL;
		}
		else
		{
			void * data = node->data;
			SafeDeleteObj(node);
			return data;
		}
	}

	(SimpleListNode *)pop_back_node()
	{

		SimpleListNode * node;

		if (len == 0)
		{
			return NULL;
		}

		else
		{
			node = end;			
			end = end->pre;
			if (node == front)
			{
				front = NULL;
			}
			len--;
		}
		node->pre = NULL;

		node->next = NULL;

		return node;
	};

	(void *)pop_front_data()
	{
		SimpleListNode * node = pop_front_node();
		if (node == NULL)
		{
			return NULL;
		}
		else
		{
			void * data = node->data;
			SafeDeleteObj(node);
			return data;
		}
	}

	(SimpleListNode *)pop_front_node()
	{

		SimpleListNode * node;

		if (len == 0)
		{
			return NULL;
		}

		else
		{
			node = front;
			front = front->next;
			if (node == end)
			{
				end = NULL;
			}
			len--;
		}
		
		node->pre = NULL;

		node->next = NULL;

		return node;
	};

}SimpleList;

typedef struct GreedyMemBlock
{
	GreedyMemBlock(unsigned int lastReadyTime, void * p, unsigned int len) :lastReadyTime(lastReadyTime), p(p), len(len){
		nodeInReadyArray = new SimpleListNode(this);
	};

	~GreedyMemBlock() { 

		SafeDeleteObj(nodeInReadyArray);
		byte* udata = (byte *)p - 1 - sizeof(GreedyMemBlock *);
	    ::free(udata);
	};
	unsigned int lastReadyTime;

	void * p;

	unsigned int len;

	SimpleListNode * nodeInReadyArray;

}GreedyMemBlock;

typedef struct LenAndBlocks
{
	unsigned int len;
	SimpleList  blockArray;

	LenAndBlocks()
	{
		len = -1;
	};

}LenAndBlocks;


#define MANAGER_MEM_SIZES_COUNT  3000

class GreedyMemManager : public BaseMemManager
{
public:
	GreedyMemManager();
	virtual ~GreedyMemManager();

	virtual void * malloc(int len);

	virtual void free(void * p);

	void onCleanTimerFired(); //清理多余的长时间没有被重用的内存

	LenAndBlocks* getLenAndBLocks(unsigned int mallocSize);

	//unsigned int getManageSize(unsigned int mallocSize);

	float reuseBlockHitRate();

	void cleanReuseBlockHitRateData();

	CriticalSection csLock;

	MMRESULT timer_id;


	long totalMallocCount = 0;

	long blockReuseCount = 0;



private:

	void initManageSizes();

	LenAndBlocks manageSizes[MANAGER_MEM_SIZES_COUNT];

	//GREEDY_READY_SIZE_BLOCK_MAP readySize2BlockMaps;

	//GREEDY_READY_SIZE_BLOCK_MAP usingSize2BlockMaps;

	 //SimpleList allReadyBlockes;


};

extern GreedyMemManager gGreedyMemManager;
