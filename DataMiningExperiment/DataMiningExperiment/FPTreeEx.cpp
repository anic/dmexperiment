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

//void FPTreeEx::createConditionalFPTree(const FPTreeEx& parent,::Item prefix,int nMinSupport)
//{
//	if(parent.header.size() == 0) 
//		return;
//
//	set<fptree::Item>::const_iterator it = parent.header.find(fptree::Item(prefix,0));
//	if (it == parent.header.end())
//		return;
//
//	//构造一个新的FPTree
//	//设置最小支持度和输出
//	setMinsup(nMinSupport);
//	
//	fptree::Item_ *i;
//	std::vector<fptree::Transaction*> result;
//	fptree::Transaction t(parent.header.size());
//	
//	for(i = it->getNext(); i; i = i->nodelink) {
//		int l=0;
//
//		//TODO:有问题，不一定是最低的，投影出来会忽略了底下的项
//		//不需要遍历2次了，在这里也是需要sort的
//		int sup = i->supp;
//		for(fptree::Item_ *p=i->parent; p; p = p->parent) t.t[l++] = p->id; //遍历父亲
//		t.length = l;
//
//		generateTransaction(&t,sup,*i,result,parent.header.size());
//
//		//支持度是应该是当前的
//		
//		processItems(&t,sup); //将事务插入新的tree中
//	}
//	Prune(); //剪枝
//
//	for(i = it->getNext(); i; i = i->nodelink) {
//		int l=0;
//		for(fptree::Item_ *p=i->parent; p; p = p->parent) tmp[l++] = p->id;
//		fptree::Transaction *t = new fptree::Transaction(l);
//		for(int j=0; j<l; j++) t->t[j] = tmp[l-j-1];
//		processTransaction(t,i->supp);
//		delete t;
//	}
//	delete [] tmp;
//}


//void FPTreeEx::createConditionalFPTree(const FPTreeEx& parent,::Item prefix)
//{
//	return createConditionalFPTree(parent,prefix,parent.minsup);
//}


bool supGreater(const fptree::Item* item1,const fptree::Item* item2)
{
	return item1->getSupport() > item2->getSupport();
}

void FPTreeEx::createFromSourceTrDB(const TrDB& trdb,int nMinSupport)
{
	setMinsup(nMinSupport);
	const TransactionSet & data = trdb.getTransaction();

	for(TransactionSet::const_iterator iter = data.begin();
		iter!=data.end();
		++iter)
	{
		this->processItems(*iter);
	}

	Prune();

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


void FPTreeEx::createFromConditionalDB(const TrDB& trdb,int nMinSupport)
{
	setMinsup(nMinSupport);

	std::vector<fptree::Transaction* > buffer; //缓存所有Transacion
	TransactionIndexList transList;
	const ClassMap& classTable = trdb.getClassTable();

	//从classtable中获得所有现在的Transacion id
	for(ClassMap::const_iterator iter = classTable.begin();
		iter!=classTable.end();
		++iter)
		transList.insert(iter->second->begin(),iter->second->end());

	for(TransactionIndexList::const_iterator iter = transList.begin();
		iter!=transList.end();
		++iter)
	{
		const ::Transaction* pTransaction = trdb.getSourceTrDB()->getTransaction()[*iter];

		if(!pTransaction->items.empty()) 
			this->processItems(pTransaction,1);

	}

	//从头表中删除前缀的Item
	for(ItemSet::const_iterator iter = trdb.getPrefix().begin();
		iter != trdb.getPrefix().end();
		++iter)
	{
		std::set<fptree::Item>::iterator iterf = header.find(fptree::Item(*iter,0));
		if (iterf != header.end())
			header.erase(iterf); 
	}

	Prune();

	for(TransactionIndexList::const_iterator iter = transList.begin();
		iter!=transList.end();
		++iter)
	{
		const ::Transaction* pTransaction = trdb.getSourceTrDB()->getTransaction()[*iter];

		std::vector<const fptree::Item *> list;
		list.reserve(pTransaction->items.size());

		for(ItemSet::const_iterator it = pTransaction->items.begin();
			it !=pTransaction->items.end();++it)
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

void FPTreeEx::createFromTrDB(const TrDB& trdb,int nMinSupport)
{
	if (trdb.isSource())
		createFromSourceTrDB(trdb,nMinSupport);
	else
		createFromConditionalDB(trdb,nMinSupport);

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


//void FPTreeEx::removeItem(::Item item)
//{
//	//TODO: 还没实现
//}

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

//int FPTreeEx::generateTransaction(fptree::Transaction* parent,int nParentSup,
//								   fptree::Item_& itemNode,std::vector<fptree::Transaction*>& result,int maxItemSize)
//{
//	
//	fptree::Transaction* lastTrans = parent;
//	fptree::Item_* last = &itemNode;
//	int sup = nParentSup;
//	
//	if (itemNode.supp!= nParentSup)
//	{
//		int sumsup = 0;
//		for(std::set<fptree::Item>::const_iterator iter = itemNode.children->begin();
//			iter!=itemNode.children->end();
//			++iter)
//		{
//			sumsup += iter->getSupport();
//		}
//
//		if (sumsup < itemNode.supp) //下一层比当前小
//		{
//			fptree::Transaction* newTrans = new fptree::Transaction(maxItemSize);
//			memcpy(newTrans->t,parent->t,parent->length);
//			newTrans->length = parent->length;
//			newTrans->t[(newTrans->length)++] = itemNode.id;
//			//缓存记住所有的transaction
//			result.push_back(newTrans);
//			processItems(newTrans, itemNode.supp - sumsup); //将事务插入新的tree中
//			lastTrans = newTrans;
//
//		}
//		//else sumsup == itemNode.supp留待下一层解决
//	}
//
//	int nChildren = itemNode.children->size();
//	if(nChildren == 1)
//	{
//		//向下找，找到其中之一：
//		//1 和当前支持度不同的，
//		//2 分枝
//		fptree::Transaction* newTrans = new fptree::Transaction(maxItemSize);
//		memcpy(newTrans->t,parent->t,parent->length);
//		newTrans->length = parent->length;
//
//		sup = itemNode.supp;
//		fptree::Item_* p = &itemNode;
//		while((p->children->size()) == 1)
//		{
//			newTrans->t[(newTrans->length)++] = p->id;
//
//			p = (p->children->begin())->getItem();
//			if (p->supp != sup)
//				break;
//		}
//
//		if(p->supp != sup) //如果支持度不同，
//		{
//			result.push_back(newTrans);
//			processItems(newTrans,sup); //将事务插入新的tree中
//			generateTransaction(newTrans,sup,*p,result,maxItemSize);
//
//			lastTrans = newTrans;
//		}
//		else //支持度相同，但是
//		{
//			newTrans->t[(newTrans->length)++] = p->id;
//			result.push_back(newTrans);
//			processItems(newTrans,sup); 
//
//			lastTrans = newTrans;
//
//			if (p->children->size() > 1) 
//				last = p;
//	
//			//else p->getChildren() == 1 || 0 不用做，因为支持度相同，肯定会往下走
//		}
//	}
//	else if(nChildren > 1)
//	{
//		fptree::Transaction* newTrans = new fptree::Transaction(maxItemSize);
//		memcpy(newTrans->t,parent->t,parent->length);
//		newTrans->length = parent->length;
//		newTrans->t[(newTrans->length)++] = itemNode.id;
//		lastTrans = newTrans;
//	}
//	
//	if (last->children->size() > 0)
//	{
//		for(std::set<fptree::Item>::const_iterator iter = last->children->begin();
//			iter!=last->children->end();
//			++iter)
//		{
//			generateTransaction(lastTrans,sup,*iter->getItem(),result,maxItemSize);
//
//		}
//	}
//
//	return 0;
//}


