#include "StdAfx.h"
#include "FPTreeEx.h"
#include <algorithm>
#include <iostream>
#include <list>

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

bool supGreater(const fptree::Item* item1,const fptree::Item* item2)
{
	return item1->getSupport() > item2->getSupport();
}

void FPTreeEx::createFromSourceTrDB(const TrDB& trdb)
{
	setMinsup(trdb.getMinSupport());
	const TransactionSet & data = trdb.getTransaction();

	//for(TransactionSet::const_iterator iter = data.begin();
	//	iter!=data.end();
	//	++iter)
	//{
	//	this->processItems(*iter);
	//}

	////剪枝
	//Prune();

	this->processItems(trdb.getItemTable());

	for(TransactionSet::const_iterator iter = data.begin();
		iter!=data.end();
		++iter)
	{
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

		sort(list.begin(), list.end(),supGreater); //sort by support

		if(!list.empty()) 
			this->processTransaction(&list);

	}
}

int FPTreeEx::processItems(const ::Transaction *t, int times)
{
	//插入一个新事务“到头表中”

	set<fptree::Item>::iterator head;
	int added=0;
	for(ItemSet::const_iterator iter = t->items.begin();
		iter!=t->items.end();
		++iter)
	{
		head = header.find(fptree::Item(*iter, 0));
		if(head == header.end()) { //没有找到，添加一个
			head = header.insert(fptree::Item(*iter, 0)).first; //在头表中加入
			added++;
		}
		head->Increment(times); //增加在头表中的支持度（找到和没有找到都是）
	}
	return added;
}

int FPTreeEx::processItems(const ItemMap& itemMap)
{
	set<fptree::Item>::iterator head;
	for(ItemMap::const_iterator iter = itemMap.begin();
		iter!=itemMap.end();++iter)
	{
		head = header.insert(fptree::Item(iter->first, 0)).first;
		head->Increment(iter->second->size());
	}
	return 1;
}


int FPTreeEx::processTransaction(std::vector<const fptree::Item *> *t,int times)
{
	//插入一个新事务

	set<fptree::Item>::iterator it, head;
	set<fptree::Item>* items = root;
	fptree::Item_ *current = 0;
	int added=0;

	for(std::vector<const fptree::Item*>::const_iterator iter = t->begin();
		iter != t->end();
		++iter) 
	{
		int item = (*iter)->getId();

		head = header.find(fptree::Item(item, 0));		//在头表中找
		if(head != header.end()) {						//如果有，也就是如果没有被minsup删除prune掉
			it = items->find(fptree::Item(item, 0));		//开始从root开始找第一个项

			if(it == items->end()) {								//没有找到项
				it = items->insert(fptree::Item(item, current)).first;	//添加一个新的
				it->setNext(head->getNext());
				head->setNext(it->getItem());
				nodes++;
				added++;
				if(singlepath && (items->size()>1)) singlepath=false; 		
			}

			it->Increment(times);
			current = it->getItem();
			items = it->makeChildren();
		}
	}

	return added;
}


void FPTreeEx::createFromConditionalDB(const TrDB& trdb)
{
	setMinsup(trdb.getMinSupport());
	
	//从一个头表变成另一个头表
	this->processItems(trdb.getItemTable());

	//std::vector<fptree::Transaction* > buffer; //缓存所有Transacion，也许以后有用
	//TransactionIndexList transList;
	//const ClassMap& classTable = trdb.getClassTable();

	////从classtable中获得所有现在的Transacion id
	//for(ClassMap::const_iterator iter = classTable.begin();
	//	iter!=classTable.end();
	//	++iter)
	//	transList.insert(iter->second->begin(),iter->second->end());

	//for(TransactionIndexList::const_iterator iter = transList.begin();
	//	iter!=transList.end();
	//	++iter)
	//{
	//	const ::Transaction* pTransaction = trdb.getSourceTrDB()->getTransaction()[*iter];

	//	if(!pTransaction->items.empty()) 
	//		this->processItems(pTransaction,1);

	//}

	////从头表中删除前缀的Item
	//for(ItemSet::const_iterator iter = trdb.getPrefix().begin();
	//	iter != trdb.getPrefix().end();
	//	++iter)
	//{
	//	std::set<fptree::Item>::iterator iterf = header.find(fptree::Item(*iter,0));
	//	if (iterf != header.end())
	//		header.erase(iterf); 
	//}

	//std::vector<fptree::Transaction* > buffer; //缓存所有Transacion，也许以后有用
	const ClassMap& classTable = trdb.getClassTable();

	//从classtable中获得所有现在的Transacion id
	for(ClassMap::const_iterator iter = classTable.begin();
		iter!=classTable.end();
		++iter)
	{

		for(TransactionIndexList::const_iterator iterTrans = iter->second->begin();
			iterTrans!=iter->second->end();
			++iterTrans)
		{
			const ::Transaction* pTransaction = trdb.getSourceTrDB()->getTransaction()[*iterTrans];


			std::vector<const fptree::Item *> list;
			list.reserve(pTransaction->items.size());

			for(ItemSet::const_iterator it = pTransaction->items.begin();
				it !=pTransaction->items.end();++it)
			{
				//将::Item转为fptree::Item来查找
				std::set<fptree::Item>::const_iterator iterf = header.find(fptree::Item(*it,0));
				if (iterf!= header.end())
					list.push_back(&(*iterf)); 
			}

			sort(list.begin(), list.end(),supGreater); //sort by support

			if(!list.empty()) 
				this->processTransaction(&list);
		}
	}

	
	//for(TransactionIndexList::const_iterator iter = transList.begin();
	//	iter!=transList.end();
	//	++iter)
	//{
	//	const ::Transaction* pTransaction = trdb.getSourceTrDB()->getTransaction()[*iter];

	//	std::vector<const fptree::Item *> list;
	//	list.reserve(pTransaction->items.size());

	//	for(ItemSet::const_iterator it = pTransaction->items.begin();
	//		it !=pTransaction->items.end();++it)
	//	{
	//		//将::Item转为fptree::Item来查找
	//		std::set<fptree::Item>::const_iterator iterf = header.find(fptree::Item(*it,0));
	//		if ( iterf!= header.end())
	//			list.push_back(&(*iterf)); 
	//	}

	//	sort(list.begin(), list.end(),supGreater); //sort by support

	//	if(!list.empty()) 
	//		this->processTransaction(&list);
	//}
}

void FPTreeEx::createFromTrDB(const TrDB& trdb)
{
	if (trdb.isSource())
		createFromSourceTrDB(trdb);
	else
		createFromConditionalDB(trdb);

}

void FPTreeEx::printOnConsole() const
{
	if (this->root!=NULL)
		for(std::set<fptree::Item>::const_iterator iter = root->begin();
			iter!=root->end();
			++iter)
			printNode(*iter,0);
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

void FPTreeEx::getPotentialPrefix(::Item item,std::vector<ItemSet_Support>& out) const
{
	set<fptree::Item>::const_iterator it = header.find(fptree::Item(item,0));
	if (it == header.end())
		return;

	for(fptree::Item_ *i = it->getNext(); i; i = i->nodelink) 
	{
		out.push_back(ItemSet_Support());
		out.rbegin()->support = i->supp;
		for(fptree::Item_ *p=i->parent; p; p = p->parent) 
			out.rbegin()->items.insert(p->id); //遍历父亲
	}
}



