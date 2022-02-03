#include "utilities.h"
#include <iostream>
using namespace std;

LinkQueue::LinkQueue(int defLength)
{
	maxLength = defLength;        //指定队列长度
	pHead = new floatNode;     //为头节点分配空间
	pHead->data = -1;
	pHead->pNext = 0;
	pRear = pHead;			   //队列为空， 尾节点指针与头节点指针指向同一节点
	currentLength = 0;         //当前长度为0
}

LinkQueue::LinkQueue()
{
	maxLength = 10;            //默认长度为10
	pHead = new floatNode;     
	pRear = pHead;
	currentLength = 0;
}

LinkQueue::~LinkQueue()
{
	floatNode* p = pHead;
	while (p != 0)              //从头节点开始删除每个节点
	{
		floatNode* pTemp = p;
		p = p->pNext;
		delete pTemp;
	}
}

bool LinkQueue::AddElement(float newElement)
{
	if (currentLength < maxLength)
	{	
		//创建新节点
		floatNode* newNode = new floatNode;
		newNode->data = newElement;
		newNode->pNext = 0;

		//移动尾指针
		pRear->pNext = newNode;
		pRear = newNode;
		currentLength = currentLength + 1;
		return true;
	}
	else if(currentLength == maxLength)     //队列满
	{
		if (DeleteElement())       //删除队首节点
		{
			if (AddElement(newElement))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;   //未知错误
	}
}

bool LinkQueue::DeleteElement()
{
	if (currentLength == 0)
	{
		return false;
	}
	else
	{
		floatNode* pTemp = pHead->pNext;
		pHead->pNext = pTemp->pNext;
		if (pTemp == pRear)            //使尾节点指针归位
		{
			pRear = pHead;
		}
		delete pTemp;
		currentLength = currentLength - 1;
		return true;
	}
}

void LinkQueue::DisplayQueue()
{
	cout << "pHead: " << pHead << "   pRear: " << pRear << endl;
	floatNode* p;
	for (p = pHead; p != 0; p = p->pNext)
	{
		cout << "Data: " << p->data << "   pNext: " << p->pNext << endl;
	}
}

int LinkQueue::GetCurrentLength()
{
	return currentLength;
}

int LinkQueue::GetMaxLength()
{
	return maxLength;
}
