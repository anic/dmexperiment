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

int FPTreeEx::getSupport(ItemAlias prefix, ClassLabel label) const
{
	
	set<fptree::Item>::const_iterator head = header.find(fptree::Item(prefix,0));
	if (head!= header.end())
		return head->getSupport();
	else
		return 0;
}

void FPTreeEx::createConditionalFPTree(const FPTreeEx& parent,ItemAlias prefix,int nMinSupport)
{
	if(parent.header.size() == 0) 
		return;

	set<fptree::Item>::const_iterator it = parent.header.find(fptree::Item(prefix,0));
	if (it == parent.header.end())
		return;

	//构造一个新的FPTree
	//设置最小支持度和输出
	setMinsup(nMinSupport);
	//setOutput(out);

	fptree::Item_ *i;
	//current[depth-1] = it->getId(); 
	int *tmp = new int[parent.header.size()];
	for(i = it->getNext(); i; i = i->nodelink) {
		int l=0;

		//TODO:有问题，不一定是最低的，投影出来会忽略了底下的项
		for(fptree::Item_ *p=i->parent; p; p = p->parent) tmp[l++] = p->id; //遍历父亲

		//获得事务的个数
		fptree::Transaction *t = new fptree::Transaction(l);
		for(int j=0; j<l; j++) t->t[j] = tmp[l-j-1]; //将事务赋值

		processItems(t,i->supp); //将事务插入新的tree中
		delete t;
	}
	Prune(); //剪枝

	for(i = it->getNext(); i; i = i->nodelink) {
		int l=0;
		for(fptree::Item_ *p=i->parent; p; p = p->parent) tmp[l++] = p->id;
		fptree::Transaction *t = new fptree::Transaction(l);
		for(int j=0; j<l; j++) t->t[j] = tmp[l-j-1];
		processTransaction(t,i->supp);
		delete t;
	}
	delete [] tmp;
}

void FPTreeEx::createConditionalFPTree(const FPTreeEx& parent,ItemAlias prefix)
{
	return createConditionalFPTree(parent,prefix,parent.minsup);
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

void FPTreeEx::createFromTrDB(const TrDB& trdb,int nMinSupport)
{

	setMinsup(nMinSupport);
	//fpt->setOutput(out);

	int tmin=1000000, tmax=0, ttotal=0, tnr=0;
	const TransactionSet & data = trdb.getTransaction();
	for(TransactionSet::const_iterator iter = data.begin();
		iter!=data.end();
		++iter)
	{

		fptree::Transaction *t = new fptree::Transaction((*iter)->items.size());

		int i=0;
		for(ItemSet::const_iterator it = (*iter)->items.begin();
			it !=(*iter)->items.end();++it,++i)
			t->t[i] = *it;

		if(t->length) {
			this->processItems(t);
			ttotal += t->length;
			if(t->length < tmin) tmin = t->length;
			if(t->length > tmax) tmax = t->length;
		}
		delete t;
		tnr++;
	}

	ReOrder();
	Prune();
	
	for(TransactionSet::const_iterator iter = data.begin();
		iter!=data.end();
		++iter)
	{
		fptree::Transaction *t = new fptree::Transaction((*iter)->items.size());
		int i=0;
		for(ItemSet::const_iterator it = (*iter)->items.begin();
			it !=(*iter)->items.end();++it,++i)
			t->t[i] = *it;


		vector<int> list;
		for(i=0; i<t->length; i++) {
			set<fptree::Element>::iterator it = this->relist->find(fptree::Element(t->t[i],0));
			if(it!=this->relist->end()) list.push_back(it->id); //在这里，id是支持度的反映
		}
		int size=list.size();
		sort(list.begin(), list.end()); //sort by support
		delete t;

		t = new fptree::Transaction(size);
		for(i=0; i<size; i++) t->t[i] = list[i]; //转化为他对应的的id
		if(t->length) this->processTransaction(t);
		delete t;
	}
}

void FPTreeEx::createFromTrDB(const TrDB& trdb)
{
	createFromTrDB(trdb,trdb.getMinSupport());
}

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

void FPTreeEx::removeItemAlias(::Item item)
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

	std::cout<<"|_"<<this->remap[item.getId()]<<":"<<item.getSupport();
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

int FPTreeEx::getRemapIndex(::Item item) const 
{
	for(int i=0;i<this->len_remap;++i)
		if (remap[i] == item)
			return i;
	return -1;
}

int FPTreeEx::generateTransaction(fptree::Transaction* parent,int nParentSup,
		const fptree::Item& itemalias,std::vector<fptree::Transaction*>& result)
{
	int nChildren = itemalias.getChildren()->size();
	if (nChildren == 0)
	{
		fptree::Transaction* newtrans = new fptree::Transaction(parent->length+1);
		//memset(newtrans->t,parent
		return 0;
	}


}