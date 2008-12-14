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



#endif