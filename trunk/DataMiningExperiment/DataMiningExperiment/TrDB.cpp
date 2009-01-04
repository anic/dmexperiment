#include "StdAfx.h"
#include "TrDB.h"
#include "common.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <algorithm>
#include "windows.h"
using namespace std;
TrDB::TrDB(void)
{
	this->m_nMinSupport = 0;
	this->m_bSource = false;
	this->m_pRawTrDB = NULL;
	this->m_nTransactionSize = 0;
	this->m_nClassSize = 0; //除了原始数据库，其他数据库的都设置成0
	this->m_pClassRemap = NULL;
}

TrDB::TrDB(const TrDB& trdb)
{
	this->m_bSource = false;

	//TODO:这里使用拷贝好吗？
	for(ClassMap::const_iterator classIter = trdb.m_classTable.begin();
		classIter!= trdb.m_classTable.end();
		++classIter)
	{
		TransactionIndexList* p = new TransactionIndexList(classIter->second->begin(),classIter->second->end());
		this->m_classTable.insert(std::make_pair(classIter->first,p));
	}


	for(ItemMap::const_iterator itemIter = trdb.m_itemTable.begin();
		itemIter!= trdb.m_itemTable.end();
		++itemIter)
	{
		TransactionIndexList* p = new TransactionIndexList(itemIter->second->begin(),itemIter->second->end());
		this->m_itemTable.insert(std::make_pair(itemIter->first,p));
	}

	this->m_nClassSize = 0; //除了原始数据库，其他数据库的都设置成0
	this->m_nMinSupport = trdb.m_nMinSupport;
	this->m_nTransactionSize = trdb.m_nTransactionSize;
	this->m_pRawTrDB = trdb.m_pRawTrDB;
	this->m_prefix.insert(trdb.m_prefix.begin(),trdb.m_prefix.end());
	//this->m_transactionSet 无需设置
	allocateCached();

}

TrDB::~TrDB(void)
{
	if (m_pClassRemap!=NULL)
		delete m_pClassRemap;

	if (m_bSource)	
	{
		for(TransactionSet::const_iterator iter = m_transactionSet.begin();
			iter != m_transactionSet.end();
			++iter)
		{
			delete *iter;
		}
	}

	for(ItemMap::const_iterator iter = m_itemTable.begin();
		iter!=m_itemTable.end();++iter)
		delete iter->second;

	for(ClassMap::const_iterator iter = m_classTable.begin();
		iter!=m_classTable.end();++iter)
		delete iter->second;


}

void TrDB::createFromFile(std::string filename)
{
	createFromFile(filename,-1);
}

void TrDB::createFromFile(std::string filename,int maxLength)
{
	//1 2:1 3:1 4:1 5:1 6:1 7:1 8:1 9:1 10:1 11:1 12:1 13:1 14:1 15:1 16:1 17:1 18:1 19:1 20:1 21:1 22:1 23:1

	m_bSource = true;
	this->m_pRawTrDB = this;
	this->m_pClassRemap = new std::vector<int>();

	ifstream fs(filename.c_str());
	string line;

	//临时记住旧Class与新class的对应
	std::map<int,int> tempMap;

	int index = 0;
	while(std::getline(fs,line)){

		stringstream ss(line);
		string s;	
		ss>>s;

		if (maxLength >0 && index >=maxLength) //如果已经到达最大的行数，返回
			break;

		Transaction* t = new Transaction(index);
		t->label = ::atoi(s.c_str());

		
		std::map<int,int>::iterator iterf = tempMap.find(t->label);
		if (iterf != tempMap.end())
			t->label = iterf->second;
		else
		{
			int newLabel = m_pClassRemap->size();
			m_pClassRemap->push_back(t->label);
			tempMap.insert(std::make_pair(t->label,newLabel));
			t->label = newLabel;
		}



		//记录class与transaction的对应
		this->setClassMap(t->label,index);

		while(ss>>s)
		{
			Item item = ::atoi(s.substr(0,s.find_first_of(':')).c_str());
			t->items.insert(item);

			//记录item与transaction的对应
			this->setItemMap(item,index);
		}
		index++;
		m_transactionSet.push_back(t);
	}
	fs.close();

	m_nTransactionSize = m_transactionSet.size();
	m_nClassSize = m_classTable.size();
	allocateCached();

}


bool empty(const Transaction* t) {
	return t->items.empty();
}

void TrDB::createConditionalDB(const TrDB& parent,Item prefix,int nMinSupport,bool removeEmptyTrans)
{
	//DWORD start = GetTickCount();
	m_nMinSupport = nMinSupport;
	m_pRawTrDB = parent.m_pRawTrDB; //获得根的TrDB

	//将前缀放入
	this->m_prefix.insert(parent.m_prefix.begin(),parent.m_prefix.end());
	this->m_prefix.insert(prefix);

	
	ItemMap::const_iterator iterf = parent.m_itemTable.find(prefix);
	if (iterf == parent.m_itemTable.end())
		this->m_nTransactionSize = 0;
	else
		this->m_nTransactionSize = iterf->second->size();

	if (m_nTransactionSize == 0 || m_nTransactionSize < nMinSupport) //事务数目少于支持度，说明所有的Item都将不满足支持度
		return;


	TransactionIndexList possibleRemovedTrans,removedTrans;

	for(ItemMap::const_iterator iter = parent.m_itemTable.begin();
		iter!=parent.m_itemTable.end();
		++iter)
	{
		//对于每一个parent的itemtable，加入到新的m_itemTable中

		//将TransactionIndexList做求交运算
		if (iterf!=iter)
		{
			TransactionIndexList* temp = new TransactionIndexList();

			std::set_intersection(iterf->second->begin(),
				iterf->second->end(),
				iter->second->begin(),
				iter->second->end(),
				std::insert_iterator<TransactionIndexList>(*temp, temp->begin() ));

			int resultSize = temp->size();
			if (resultSize >0)
			{	
				if (resultSize>= nMinSupport)
					m_itemTable.insert(std::make_pair(iter->first,temp));
				else
				{
					possibleRemovedTrans.insert(temp->begin(),temp->end());
					delete temp;
				}
			}
			else
				delete temp;
		}
	}
		
	TransactionIndexList existTrans;
	if (removeEmptyTrans)
	{
		this->checkPossibleRemoveTrans(possibleRemovedTrans,removedTrans);
		std::set_difference(iterf->second->begin(),
			iterf->second->end(),
			removedTrans.begin(),
			removedTrans.end(),
			std::insert_iterator<TransactionIndexList>(existTrans,existTrans.begin()));
	}
	else
	{
		existTrans.insert(iterf->second->begin(),iterf->second->end());
	}
	m_nTransactionSize = existTrans.size();

	for(ClassMap::const_iterator iter = parent.m_classTable.begin();
		iter!=parent.m_classTable.end();
		++iter)
	{
		//对于每一个类表，做交运算
		TransactionIndexList* temp = new TransactionIndexList();

		std::set_intersection(existTrans.begin(),
			existTrans.end(),
			iter->second->begin(),
			iter->second->end(),
			std::insert_iterator<TransactionIndexList>(*temp, temp->begin() ));

		if (!temp->empty())
			m_classTable.insert(std::make_pair(iter->first,temp));
		else
			delete temp;
	}

	
	//建立Cache
	allocateCached();
}

const Transaction* TrDB::getTransactionByTid(int nTid) const
{
	if(m_bSource)
		return m_transactionSet[nTid];
	else
	{
		for(TransactionSet::const_iterator iter = m_pRawTrDB->m_transactionSet.begin();
			iter!=m_pRawTrDB->m_transactionSet.end();
			++iter)
		{
			if ((*iter)->id == nTid)
				return (*iter);
		}
		return NULL;
	}
}

int TrDB::getSupport(ClassLabel label) const
{
	ClassMap::const_iterator iterf = m_classTable.find(label);
	if (iterf != m_classTable.end())
		return iterf->second->size();
	else
		return 0;
}

int TrDB::getSupport(Item prefix,ClassLabel label) const
{
	int index = this->getCachedIndex(prefix,label);
	if (m_cachedSupport[index]!=-1)
		return m_cachedSupport[index];

	ItemMap::const_iterator iterf = m_itemTable.find(prefix);
	if (iterf!=m_itemTable.end())
	{
		int result =0;
		for(TransactionIndexList::const_iterator iter = iterf->second->begin();
			iter!=iterf->second->end();
			++iter)
		{
			if (m_pRawTrDB->m_transactionSet[*iter]->label == label)
				++result;
		}
		m_cachedSupport[index] = result;
		return result;
	}
	else
	{
		m_cachedSupport[index] = 0;
		return 0;
	}
}



void TrDB::setItemMap(Item item,int tIndex)
{
	//创建头表
	ItemMap::iterator iterf = m_itemTable.find(item);
	if (iterf == m_itemTable.end()) //没有之前的
	{
		TransactionIndexList* firstList = new TransactionIndexList();
		firstList->insert(tIndex);
		m_itemTable.insert(make_pair(item,firstList));
	}
	else
		iterf->second->insert(tIndex);
}

void TrDB::setClassMap(ClassLabel label,int tIndex)
{
	//创建头表
	ClassMap::iterator iterf = m_classTable.find(label);
	if (iterf == m_classTable.end()) //没有之前的
	{
		TransactionIndexList* firstList = new TransactionIndexList();
		firstList->insert(tIndex);
		m_classTable.insert(make_pair(label,firstList));
	}
	else
		iterf->second->insert(tIndex);
}

const TransactionIndexList* TrDB::getTransactionsByClass(ClassLabel label)const
{
	ClassMap::const_iterator iterf = m_classTable.find(label);
	if(iterf!=m_classTable.end())
		return iterf->second;
	else
		return NULL;
}

const TransactionIndexList* TrDB::getTransactionsByItem(Item item) const
{
	ItemMap::const_iterator iterf = m_itemTable.find(item);
	if(iterf!=m_itemTable.end())
		return (iterf->second);
	else
		return NULL;		
}

void TrDB::removeItem(Item item)
{
	ItemMap::iterator iterf = m_itemTable.find(item);
	if(iterf==m_itemTable.end())
		return;

	
	std::set<Item> clearedItem; //记录这一轮是否已经把该项做 交运算
	TransactionIndexList possibleRemovedTrans;//可能需要额外删除的Trans，自动检查唯一性

	Item rawItem = iterf->first;
	TransactionIndexList removedTrans(iterf->second->begin(),iterf->second->end()); //拷贝要移除的transacions 
	
	delete iterf->second;

	//删除原始的Item
	m_itemTable.erase(iterf); 

	//遍历所有要删除的Transacion
	for(TransactionIndexList::const_iterator iter = removedTrans.begin();
		iter!=removedTrans.end();
		++iter)
	{
		
		const ItemSet& items =  m_pRawTrDB->m_transactionSet[*iter]->items;

		//遍历每个transaction中的每个Item
		for(ItemSet::const_iterator iterItem = items.begin();
			iterItem!= items.end();++iterItem)
		{
			Item item = *iterItem; //原始的Item已经从ItemTable中删除了

			if (clearedItem.find(item)!= clearedItem.end()) //已经清除过了
				continue;
			
			iterf = m_itemTable.find(*iterItem);
			if(iterf==m_itemTable.end())
				continue; //该Item已经不存在了，不需要清除了

			TransactionIndexList* result = new TransactionIndexList();
			TransactionIndexList* pre = iterf->second;

			std::set_difference(pre->begin(),
			pre->end(),
			removedTrans.begin(),
			removedTrans.end(),
			std::insert_iterator<TransactionIndexList>(*result, result->begin() ));

			iterf->second = result;
			delete pre; //只有是非影子才能删除

			if(result->size() < (size_t)m_nMinSupport) //如果不满足支持度，下一个需要清除这个Item
			{
				for(TransactionIndexList::const_iterator iterTrans = result->begin();
					iterTrans!= result->end();
					++iterTrans)
					possibleRemovedTrans.insert(*iterTrans); //这里传入的trans一定不在原始的removedTrans中

				delete result; //这个类中创建的，所以可以清除
				m_itemTable.erase(item); //从m_itemTable中删除了item
			}
			clearedItem.insert(item); //清除了item了
		}
	}

	//检查有多少个可能被移除的Transacion真正被移除，插入removedTrans
	checkPossibleRemoveTrans(possibleRemovedTrans,removedTrans);

	//实际移除了多少个
	m_nTransactionSize -= removedTrans.size();
	
	//检查classTable
	removeTransFromClassTable(removedTrans);

}

const TransactionSet& TrDB::getTransaction() const
{
	if (m_bSource)
		return m_transactionSet;
	else
		throw exception("Fail to get transaction in none-raw db.");
}

int TrDB::getSize()const
{
	return m_nTransactionSize;
}

int TrDB::getCachedIndex(Item item,ClassLabel label) const
{
	int n = m_pRawTrDB->m_nClassSize;
	return item * n + label % n;
}

void TrDB::allocateCached() const
{
	if (!m_itemTable.empty())
		m_cachedSupport.assign(getCachedIndex(m_itemTable.rbegin()->first,m_pRawTrDB->m_nClassSize-1)+1,-1);
}



void TrDB::setMinSupport(int nMinSupport)
{
	//这里可以改成任意的了
	this->m_nMinSupport = nMinSupport;

	TransactionIndexList possibleRemovedTrans;
	TransactionIndexList removedTrans;

	for(ItemMap::iterator iter = m_itemTable.begin();
		iter!=m_itemTable.end();)
	{
		if (iter->second->size() < (size_t)nMinSupport) //小于最小支持度
		{
			possibleRemovedTrans.insert(iter->second->begin(),iter->second->end());
			m_itemTable.erase(iter++);	//从ItemTable中删除之
		}
		else
			++iter;
	}
	
	checkPossibleRemoveTrans(possibleRemovedTrans,removedTrans);
	//实际移除了多少个
	m_nTransactionSize -= removedTrans.size();
	removeTransFromClassTable(removedTrans);

}

//检查一下可能被删除的Trans是否需要被删除
void TrDB::checkPossibleRemoveTrans(TransactionIndexList& possibleRemovedTrans,TransactionIndexList& removedTrans)
{
	//检查一下possibleRemovedTrans，有多少已经没有了
	for(TransactionIndexList::const_iterator iter = possibleRemovedTrans.begin();
		iter!= possibleRemovedTrans.end();
		++iter)
	{
		const ItemSet& items = m_pRawTrDB->m_transactionSet[*iter]->items;
		ItemSet::const_iterator iterItem;
		for(iterItem = items.begin();
			iterItem!= items.end();
			++iterItem)
		{
			if (m_itemTable.find(*iterItem) != m_itemTable.end())
				break;
		}

		//如果Transacion空了，没有在itemTable中找到任何一个Item，则表明被这次删除了。
		if (iterItem == items.end())
			removedTrans.insert(*iter);
	}
}

void TrDB::removeTransFromClassTable(TransactionIndexList& removedTrans)
{
	//实际移除的已经确定，和classTable中的各项做 交集运算
	for(ClassMap::iterator iter = m_classTable.begin();
		iter!=m_classTable.end();
		)
	{
			TransactionIndexList* result = new TransactionIndexList();
			TransactionIndexList* pre = iter->second;

			std::set_difference(pre->begin(),
			pre->end(),
			removedTrans.begin(),
			removedTrans.end(),
			std::insert_iterator<TransactionIndexList>(*result, result->begin() ));

			iter->second = result;

			delete pre; 

			if (result->empty())
			{
				delete result;
				m_classTable.erase(iter++);
			}
			else
				++iter;
	}
}

ClassLabel TrDB::getClass(ClassLabel alias)const
{
	if (!m_bSource)
		throw exception("Only source trdb can invoke this method.");
	
	return (*m_pClassRemap)[alias];
}

int TrDB::getSupport(const ItemSet& items,ClassLabel label) const
{
	if (items.empty())
		return 0;

	TransactionIndexList* result = NULL;
	bool needDel = false; //是否需要del result这个指针
	for(ItemSet::const_iterator iter = items.begin();
		iter!=items.end();++iter)
	{
		ItemMap::const_iterator iterf = m_itemTable.find(*iter);
		if (iterf!=m_itemTable.end())
		{
			if(result == NULL)
				result = iterf->second;
			else
			{
				TransactionIndexList* newResult = new TransactionIndexList();

				//与之前的结果做交运算
				std::set_intersection(result->begin(),result->end(),iterf->second->begin(),iterf->second->end(),
					std::insert_iterator<TransactionIndexList>(*newResult, newResult->begin() ));
				
				if (needDel)
					delete result;
				
				result = newResult;
				needDel = true;

				//如果在其中交已经为空，则支持度为0，不需要继续了
				if (result->empty())
				{
					delete result;
					return 0;
				}

			}
		}
		else
		{
			//如果items中的项，不在数据库中出现，则表示这个items的支持度为0
			if (needDel)
				delete result;

			return 0;
		}
	}

	if (result == NULL)
		return 0;
	else
	{
		ClassMap::const_iterator iterClass = m_classTable.find(label);
		if (iterClass!=m_classTable.end())
		{
			TransactionIndexList newResult;

			//与之前的结果做交运算
			std::set_intersection(result->begin(),result->end(),iterClass->second->begin(),iterClass->second->end(),
			std::insert_iterator<TransactionIndexList>(newResult, newResult.begin() ));
				
			if (needDel)
				delete result;
				
			return newResult.size();
		}
		else
		{
			if (needDel)
				delete result;
			return 0; //没有标签
		}
	}

}

int TrDB::getSupport(const ItemSet& items) const
{
	if (items.empty())
		return 0;

	TransactionIndexList* result = NULL;
	bool needDel = false; //是否需要del result这个指针
	for(ItemSet::const_iterator iter = items.begin();
		iter!=items.end();++iter)
	{
		ItemMap::const_iterator iterf = m_itemTable.find(*iter);
		if (iterf!=m_itemTable.end())
		{
			if(result == NULL)
				result = iterf->second;
			else
			{
				TransactionIndexList* newResult = new TransactionIndexList();

				//与之前的结果做交运算
				std::set_intersection(result->begin(),result->end(),iterf->second->begin(),iterf->second->end(),
					std::insert_iterator<TransactionIndexList>(*newResult, newResult->begin() ));
				
				if (needDel)
					delete result;
				
				result = newResult;
				needDel = true;

				//如果在其中交已经为空，则支持度为0，不需要继续了
				if (result->empty())
				{
					delete result;
					return 0;
				}

			}
		}
		else
		{
			//如果items中的项，不在数据库中出现，则表示这个items的支持度为0
			if (needDel)
				delete result;

			return 0;
		}
	}

	if (result == NULL)
		return 0;
	else
	{
		int nResult = result->size();
		if (needDel)
			delete result;

		return nResult;
	}
}