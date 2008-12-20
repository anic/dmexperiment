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

	//����һ���µ�FPTree
	//������С֧�ֶȺ����
	setMinsup(nMinSupport);
	
	fptree::Item_ *i;
	std::vector<fptree::Transaction*> result;
	fptree::Transaction t(parent.header.size());
	
	for(i = it->getNext(); i; i = i->nodelink) {
		int l=0;

		//TODO:�����⣬��һ������͵ģ�ͶӰ����������˵��µ���
		//����Ҫ����2���ˣ�������Ҳ����Ҫsort��
		int sup = i->supp;
		for(fptree::Item_ *p=i->parent; p; p = p->parent) t.t[l++] = p->id; //��������
		t.length = l;

		generateTransaction(&t,sup,*i,result,parent.header.size());

		//֧�ֶ���Ӧ���ǵ�ǰ��
		
		processItems(&t,sup); //����������µ�tree��
	}
	Prune(); //��֦

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

	 //һ��һ���ڴ棬����������ͷ�
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

	//����ReOrder()��
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
			//��::ItemתΪfptree::Item������
			std::set<fptree::Item>::const_iterator iterf = header.find(fptree::Item(*it,0));
			if ( iterf!= header.end())
				list.push_back(&(*iterf)); 
		}

		int size=list.size();
		sort(list.begin(), list.end(),supGreater); //sort by support
		
		//��0��ʼ��ÿ����ʽ�����
		for(int i=0; i<size; i++) t.t[i] = list[i]->getId(); 
		
		//ȷ��ʵ��ʹ����Ŀ
		t.length = size;

		//TODO:���processTransactionֱ��֧�ֶ���list
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

////���Item�ı���Ҫ����֧�ֶȣ�����Item��Ȼ���򣬲��ܿ��ٶ�λ
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
////���Item�ı���Ҫ����֧�ֶȣ�������ѡ�����֧�ֶ��Ž���
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
	//TODO: ��ûʵ��
	
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

		if (sumsup < itemNode.supp) //��һ��ȵ�ǰС
		{
			fptree::Transaction* newTrans = new fptree::Transaction(maxItemSize);
			memcpy(newTrans->t,parent->t,parent->length);
			newTrans->length = parent->length;
			newTrans->t[(newTrans->length)++] = itemNode.id;
			//�����ס���е�transaction
			result.push_back(newTrans);
			processItems(newTrans, itemNode.supp - sumsup); //����������µ�tree��
			lastTrans = newTrans;

		}
		//else sumsup == itemNode.supp������һ����
	}

	int nChildren = itemNode.children->size();
	if(nChildren == 1)
	{
		//�����ң��ҵ�����֮һ��
		//1 �͵�ǰ֧�ֶȲ�ͬ�ģ�
		//2 ��֦
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

		if(p->supp != sup) //���֧�ֶȲ�ͬ��
		{
			result.push_back(newTrans);
			processItems(newTrans,sup); //����������µ�tree��
			generateTransaction(newTrans,sup,*p,result,maxItemSize);

			lastTrans = newTrans;
		}
		else //֧�ֶ���ͬ������
		{
			newTrans->t[(newTrans->length)++] = p->id;
			result.push_back(newTrans);
			processItems(newTrans,sup); 

			lastTrans = newTrans;

			if (p->children->size() > 1) 
				last = p;
	
			//else p->getChildren() == 1 || 0 ����������Ϊ֧�ֶ���ͬ���϶���������
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


