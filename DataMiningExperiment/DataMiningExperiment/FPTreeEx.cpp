#include "StdAfx.h"
#include "FPTreeEx.h"
#include <algorithm>
#include <iostream>

FPTreeEx::FPTreeEx(void)
{
}

FPTreeEx::~FPTreeEx(void)
{
}

int FPTreeEx::getSupport(::Item prefix, ClassLabel label) const
{
	
	set<fptree::Item>::const_iterator head = header.find(fptree::Item(prefix,0));
	if (head!= header.end())
		return head->getSupport();
	else
		return 0;
}

void FPTreeEx::createConditionalFPTree(const FPTreeEx& parent,::Item prefix,int nMinSupport)
{
	if(parent.header.size() == 0) 
		return;

	set<fptree::Item>::const_iterator it = parent.header.find(fptree::Item(prefix,0));
	if (it == parent.header.end())
		return;

	//构造一个新的FPTree
	//设置最小支持度和输出
	setMinsup(nMinSupport);
	
	fptree::Item_ *i;
	std::vector<fptree::Transaction*> result;
	fptree::Transaction t(parent.header.size());
	
	for(i = it->getNext(); i; i = i->nodelink) {
		int l=0;

		//TODO:有问题，不一定是最低的，投影出来会忽略了底下的项
		//不需要遍历2次了，在这里也是需要sort的
		int sup = i->supp;
		for(fptree::Item_ *p=i->parent; p; p = p->parent) t.t[l++] = p->id; //遍历父亲
		t.length = l;

		generateTransaction(&t,sup,*i,result,parent.header.size());

		//支持度是应该是当前的
		
		processItems(&t,sup); //将事务插入新的tree中
	}
	Prune(); //剪枝

	//for(i = it->getNext(); i; i = i->nodelink) {
	//	int l=0;
	//	for(fptree::Item_ *p=i->parent; p; p = p->parent) tmp[l++] = p->id;
	//	fptree::Transaction *t = new fptree::Transaction(l);
	//	for(int j=0; j<l; j++) t->t[j] = tmp[l-j-1];
	//	processTransaction(t,i->supp);
	//	delete t;
	//}
	//delete [] tmp;
}

void FPTreeEx::createConditionalFPTree(const FPTreeEx& parent,::Item prefix)
{
	return createConditionalFPTree(parent,prefix,parent.minsup);
}


bool supGreater(const fptree::Item* item1,const fptree::Item* item2)
{
	return item1->getSupport() > item2->getSupport();
}

void FPTreeEx::createFromTrDB(const TrDB& trdb,int nMinSupport)
{
	setMinsup(nMinSupport);
	const TransactionSet & data = trdb.getTransaction();

	 //一次一块内存，最后用完再释放
	for(TransactionSet::const_iterator iter = data.begin();
		iter!=data.end();
		++iter)
	{
		fptree::Transaction t((*iter)->items.size());
		int i=0;
		for(ItemSet::const_iterator it = (*iter)->items.begin();
			it !=(*iter)->items.end();++it,++i)
			t.t[i] = *it;

		t.length = i; 

		if(t.length) {
			this->processItems(&t);
		}
	}

	//不用ReOrder()了
	Prune();
	
	for(TransactionSet::const_iterator iter = data.begin();
		iter!=data.end();
		++iter)
	{
		fptree::Transaction t((*iter)->items.size());

		std::vector<const fptree::Item*> list;
		list.reserve((*iter)->items.size());

		for(ItemSet::const_iterator it = (*iter)->items.begin();
			it !=(*iter)->items.end();++it)
		{
			//将::Item转为fptree::Item来查找
			std::set<fptree::Item>::const_iterator iterf = header.find(fptree::Item(*it,0));
			if ( iterf!= header.end())
				list.push_back(&(*iterf)); 
		}

		int size=list.size();
		sort(list.begin(), list.end(),supGreater); //sort by support
		
		//从0开始，每次形式上清空
		for(int i=0; i<size; i++) t.t[i] = list[i]->getId(); 
		
		//确定实际使用数目
		t.length = size;

		//TODO:最好processTransaction直接支持读入list
		if(t.length) this->processTransaction(&t);
		
	}
	
}

void FPTreeEx::createFromTrDB(const TrDB& trdb)
{
	createFromTrDB(trdb,trdb.getMinSupport());
}

//bool supGreater(const fptree::Item_ * item1,const fptree::Item_ * item2)
//{
//	return item1->supp > item2->supp;
//}
//
//
//void FPTreeEx::getHeader(std::vector<::Item>& output,bool bSortedSupport) const
//{
//	if (!bSortedSupport)
//	{
//		output.reserve(header.size());
//		for(set<fptree::Item>::const_iterator iter = header.begin();
//			iter!=header.end();
//			++iter)
//			output.push_back(this->remap[iter->getId()]);
//		return;
//	}
//	else
//	{
//		std::vector<fptree::Item_ *> list;
//		list.reserve(header.size());
//		for(set<fptree::Item>::const_iterator iter = header.begin();
//			iter!=header.end();
//			++iter)
//		{
//			list.push_back(iter->getItem());
//		}
//		std::sort(list.begin(),list.end(),supGreater);
//
//		output.reserve(list.size());
//		for(std::vector<fptree::Item_ *>::const_iterator iter = list.begin();
//			iter!=list.end();
//			++iter)
//		{
//			output.push_back(this->remap[(*iter)->id]);
//		}
//		return;
//	}
//}

////获得Item的表，需要访问支持度，根据Item自然排序，并能快速定位
//void FPTreeEx::getHeader(std::map<::Item,int>& output) const
//{
//	for(std::set<fptree::Item>::const_iterator iter = header.begin();
//			iter!=header.end();
//			++iter)
//	{
//		output.insert(std::make_pair(this->remap[iter->getId()],iter->getSupport()));
//	}
//	
//}
//
//bool itemSupGreater(const Item_Support& item1,const Item_Support& item2)
//{
//	return item1.support>item2.support;
//}
//
////获得Item的表，需要访问支持度，并可以选择根据支持度排降序
//void FPTreeEx::getHeader(std::vector<Item_Support>& output,bool bSortedBySupport) const
//{
//	output.reserve(header.size());
//	for(std::set<fptree::Item>::const_iterator iter = header.begin();
//			iter!=header.end();
//			++iter)
//	{
//		output.push_back(Item_Support(this->remap[iter->getId()],iter->getSupport()));
//	}
//
//	if (bSortedBySupport)
//		std::sort(output.begin(),output.end(),itemSupGreater);
//}


void FPTreeEx::removeItem(::Item item)
{
	//TODO: 还没实现
	
}

void FPTreeEx::printOnConsole() const
{
	if (this->root!=NULL && !root->empty())
		printNode(*(root->begin()),0);
}

void FPTreeEx::printNode(const fptree::Item& item, int level) const
{
	for(int i=0;i<level;++i)
		std::cout<<"    ";

	std::cout<<"|_"<<item.getId()<<":"<<item.getSupport();
	std::cout<<std::endl;

	std::set<fptree::Item> * pChildren = item.getChildren();
	if (pChildren !=NULL)
	{
		for(std::set<fptree::Item>::const_iterator iter = pChildren->begin();
			iter != pChildren->end();
			++iter)
			printNode(*iter,level+1);
	}
}



int FPTreeEx::generateTransaction(fptree::Transaction* parent,int nParentSup,
								   fptree::Item_& itemNode,std::vector<fptree::Transaction*>& result,int maxItemSize)
{
	
	fptree::Transaction* lastTrans = parent;
	fptree::Item_* last = &itemNode;
	int sup = nParentSup;
	
	if (itemNode.supp!= nParentSup)
	{
		int sumsup = 0;
		for(std::set<fptree::Item>::const_iterator iter = itemNode.children->begin();
			iter!=itemNode.children->end();
			++iter)
		{
			sumsup += iter->getSupport();
		}

		if (sumsup < itemNode.supp) //下一层比当前小
		{
			fptree::Transaction* newTrans = new fptree::Transaction(maxItemSize);
			memcpy(newTrans->t,parent->t,parent->length);
			newTrans->length = parent->length;
			newTrans->t[(newTrans->length)++] = itemNode.id;
			//缓存记住所有的transaction
			result.push_back(newTrans);
			processItems(newTrans, itemNode.supp - sumsup); //将事务插入新的tree中
			lastTrans = newTrans;

		}
		//else sumsup == itemNode.supp留待下一层解决
	}

	int nChildren = itemNode.children->size();
	if(nChildren == 1)
	{
		//向下找，找到其中之一：
		//1 和当前支持度不同的，
		//2 分枝
		fptree::Transaction* newTrans = new fptree::Transaction(maxItemSize);
		memcpy(newTrans->t,parent->t,parent->length);
		newTrans->length = parent->length;

		sup = itemNode.supp;
		fptree::Item_* p = &itemNode;
		while((p->children->size()) == 1)
		{
			newTrans->t[(newTrans->length)++] = p->id;

			p = (p->children->begin())->getItem();
			if (p->supp != sup)
				break;
		}

		if(p->supp != sup) //如果支持度不同，
		{
			result.push_back(newTrans);
			processItems(newTrans,sup); //将事务插入新的tree中
			generateTransaction(newTrans,sup,*p,result,maxItemSize);

			lastTrans = newTrans;
		}
		else //支持度相同，但是
		{
			newTrans->t[(newTrans->length)++] = p->id;
			result.push_back(newTrans);
			processItems(newTrans,sup); 

			lastTrans = newTrans;

			if (p->children->size() > 1) 
				last = p;
	
			//else p->getChildren() == 1 || 0 不用做，因为支持度相同，肯定会往下走
		}
	}
	else if(nChildren > 1)
	{
		fptree::Transaction* newTrans = new fptree::Transaction(maxItemSize);
		memcpy(newTrans->t,parent->t,parent->length);
		newTrans->length = parent->length;
		newTrans->t[(newTrans->length)++] = itemNode.id;
		lastTrans = newTrans;
	}
	
	if (last->children->size() > 0)
	{
		for(std::set<fptree::Item>::const_iterator iter = last->children->begin();
			iter!=last->children->end();
			++iter)
		{
			generateTransaction(lastTrans,sup,*iter->getItem(),result,maxItemSize);

		}
	}

	return 0;
}


