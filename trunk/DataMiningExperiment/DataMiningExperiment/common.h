#ifndef _COMMON_H
#define _COMMON_H

#include <set>


/*!
 * \brief
 * Item的定义
 */
typedef int Item;


/*!
 * \brief
 * Item集合的定义
 */
typedef std::set<Item> ItemSet;



/*!
 * \brief
 * 分类标签定义
 * 
 */
typedef int ClassLabel;


/*!
 * \brief
 * 规则的定义
*/
typedef struct _RULE
{
	//规则格式 id: body ==> head, support, confidence

	int id;				//规则的序号
	ItemSet body;		//规则的头，也就是
	ClassLabel head;	//分类标签
	double support;		//支持度
	double confidence;	//可信度

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
	Item item;		//项名称
	int support;	//支持度
	_ITEMSUPPORT(Item item,int nSupport) {
		this->item = item;
		support = nSupport;
	}
}ItemSupport;


//集合subSet是否包含在superSet中
bool set_contain(const ItemSet& subSet,const ItemSet& superSet);



#endif