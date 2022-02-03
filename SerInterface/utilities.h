#define DefineNodeType(type) typedef struct type##Node\
{\
type data;\
struct type##Node* pNext;\
}type##Node;

DefineNodeType(float)

class LinkQueue      //链表队列，包含一个头节点和若干个数据节点
{
public:
	LinkQueue(int defLength);
	LinkQueue();
	~LinkQueue();

public:
	bool AddElement(float newElement);  //增加一个新的数据节点   
	void DisplayQueue();				//测试用函数，打印队列中每个节点的信息
	int GetCurrentLength();             //获取队列当前长度
	int GetMaxLength();                 //获取队列最大长度

private:
	bool DeleteElement();						//删除队首的数据节点

private:
	floatNode* pHead;       //头节点指针
	floatNode* pRear;		//尾节点指针
	int maxLength;			//最大长度
	int currentLength;		//当前长度
};
