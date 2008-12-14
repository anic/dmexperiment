#ifndef _COMMON_H
#define _COMMON_H

#include <set>


/*!
 * \brief
 * Item�Ķ���
 */
typedef int Item;


/*!
 * \brief
 * Item���ϵĶ���
 */
typedef std::set<Item> ItemSet;



/*!
 * \brief
 * �����ǩ����
 * 
 */
typedef int ClassLabel;


/*!
 * \brief
 * ����Ķ���
*/
typedef struct _RULE
{
	//�����ʽ id: body ==> head, support, confidence

	int id;				//��������
	ItemSet body;		//�����ͷ��Ҳ����
	ClassLabel head;	//�����ǩ
	double support;		//֧�ֶ�
	double confidence;	//���Ŷ�

public:
	_RULE(int nId){
		id = nId;
		head = 0;
		support = 0.0;
		confidence = 0.0;
	}

	_RULE(){
		id = 0;
		head = 0;
		support = confidence = 0.0;

	}
}Rule;

typedef struct _ITEMSUPPORT
{
	Item item;		//������
	int support;	//֧�ֶ�
	_ITEMSUPPORT(Item item,int nSupport) {
		this->item = item;
		support = nSupport;
	}
}ItemSupport;


//����subSet�Ƿ������superSet��
bool set_contain(const ItemSet& subSet,const ItemSet& superSet);



#endif