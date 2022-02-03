#include "utilities.h"
#include <iostream>
using namespace std;

LinkQueue::LinkQueue(int defLength)
{
	maxLength = defLength;        //ָ�����г���
	pHead = new floatNode;     //Ϊͷ�ڵ����ռ�
	pHead->data = -1;
	pHead->pNext = 0;
	pRear = pHead;			   //����Ϊ�գ� β�ڵ�ָ����ͷ�ڵ�ָ��ָ��ͬһ�ڵ�
	currentLength = 0;         //��ǰ����Ϊ0
}

LinkQueue::LinkQueue()
{
	maxLength = 10;            //Ĭ�ϳ���Ϊ10
	pHead = new floatNode;     
	pRear = pHead;
	currentLength = 0;
}

LinkQueue::~LinkQueue()
{
	floatNode* p = pHead;
	while (p != 0)              //��ͷ�ڵ㿪ʼɾ��ÿ���ڵ�
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
		//�����½ڵ�
		floatNode* newNode = new floatNode;
		newNode->data = newElement;
		newNode->pNext = 0;

		//�ƶ�βָ��
		pRear->pNext = newNode;
		pRear = newNode;
		currentLength = currentLength + 1;
		return true;
	}
	else if(currentLength == maxLength)     //������
	{
		if (DeleteElement())       //ɾ�����׽ڵ�
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
		return false;   //δ֪����
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
		if (pTemp == pRear)            //ʹβ�ڵ�ָ���λ
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
