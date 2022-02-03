#define DefineNodeType(type) typedef struct type##Node\
{\
type data;\
struct type##Node* pNext;\
}type##Node;

DefineNodeType(float)

class LinkQueue      //������У�����һ��ͷ�ڵ�����ɸ����ݽڵ�
{
public:
	LinkQueue(int defLength);
	LinkQueue();
	~LinkQueue();

public:
	bool AddElement(float newElement);  //����һ���µ����ݽڵ�   
	void DisplayQueue();				//�����ú�������ӡ������ÿ���ڵ����Ϣ
	int GetCurrentLength();             //��ȡ���е�ǰ����
	int GetMaxLength();                 //��ȡ������󳤶�

private:
	bool DeleteElement();						//ɾ�����׵����ݽڵ�

private:
	floatNode* pHead;       //ͷ�ڵ�ָ��
	floatNode* pRear;		//β�ڵ�ָ��
	int maxLength;			//��󳤶�
	int currentLength;		//��ǰ����
};
