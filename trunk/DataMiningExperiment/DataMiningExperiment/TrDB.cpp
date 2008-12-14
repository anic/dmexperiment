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

}

TrDB::~TrDB(void)
{
	
}

void TrDB::createFromFile(std::string filename)
{
	createFromFile(filename,-1);
}

void TrDB::createFromFile(std::string filename,int maxLength)
{
	//1 2:1 3:1 4:1 5:1 6:1 7:1 8:1 9:1 10:1 11:1 12:1 13:1 14:1 15:1 16:1 17:1 18:1 19:1 20:1 21:1 22:1 23:1

	ifstream fs(filename.c_str());
	string line;
	
	int index = 0;
	while(std::getline(fs,line)){
		
		stringstream ss(line);
		string s;	
		ss>>s;

		if (maxLength >0 && index >=maxLength) //如果已经到达最大的行数，返回
			break;

		Transaction t(index);
		t.label = ::atoi(s.c_str());
		
		//记录class与transaction的对应
		this->setClassMap(t.label,index);

		while(ss>>s)
		{
			Item item = ::atoi(s.substr(0,s.find_first_of(':')).c_str());
			t.items.insert(item);

			//记录item与transaction的对应
			this->setItemMap(item,index);
		}
		index++;
		m_transactionSet.push_back(t);
	}
	fs.close();

}

void TrDB::createConditionalDB(const TrDB &parent, const Item &prefix,int nMinSupport)
{
	ItemSet prefixSet;
	prefixSet.insert(prefix);
	createConditionalDB(parent,prefixSet,nMinSupport);
}


bool empty(const Transaction& t) {
	return t.items.empty();
	//return false;
}



void TrDB::createConditionalDB(const TrDB &parent, const ItemSet &prefix,int nMinSupport)
{
	//将前缀放入
	this->m_prefix.insert(parent.m_prefix.begin(),parent.m_prefix.end());
	this->m_prefix.insert(prefix.begin(),prefix.end());

	int nNewPrefix = prefix.size();
	if (nNewPrefix >1 || nNewPrefix == 0)
	{
		const TransactionSet& transactions = parent.getTransaction();
		for(TransactionSet::const_iterator iter = transactions.begin();
			iter!=transactions.end();++iter)
		{
			if (set_contain(m_prefix,iter->items)) //事务包含新的前缀，则插入到投影数据库
				m_transactionSet.push_back(*iter);
		}
	}
	else if (nNewPrefix == 1) //如果是添加一个新的前缀，可以利用已有的头表索引加速
	{
		ItemMap::const_iterator iter = parent.m_itemTable.find(*prefix.begin());
		for(TransactionIndexList::const_iterator iter2 = iter->second.begin();
			iter2!=iter->second.end();
			++iter2)
		{
			m_transactionSet.push_back(parent.m_transactionSet[*iter2]);
		}
	}

	//计算支持度
	map<Item,int> header;
	for(TransactionSet::iterator iter = m_transactionSet.begin();
		iter != m_transactionSet.end();++iter)
	{
		for(ItemSet::iterator inner = (*iter).items.begin();
			inner != (*iter).items.end();
			++inner)
		{
			map<Item,int>::iterator iterf = header.find(*inner);
			if (iterf == header.end()) //没有之前的
				header.insert(::make_pair(*inner,1));
			else
				iterf->second++;
		}
	}
	
	//挑出不满足支持度的Item
	ItemSet unsupported;
	for(map<Item,int>::iterator iter = header.begin();
		iter !=header.end();++iter)
	{
		if (iter->second < nMinSupport)
			unsupported.insert(iter->first);
	}
	unsupported.insert(m_prefix.begin(),m_prefix.end()); //前缀也要去除

	//去除不满足支持度的项
	if(!unsupported.empty())
	{
		for(TransactionSet::iterator iter = m_transactionSet.begin();
			iter != m_transactionSet.end();++iter)
		{
			ItemSet removed;

			//求集合差
			::set_difference(
				iter->items.begin(),
				iter->items.end(),
				unsupported.begin(),
				unsupported.end(),
				std::insert_iterator<ItemSet>(removed, removed.begin() ));

			iter->items.swap(removed);
		}

		//去除空的事务
		TransactionSet::iterator i = remove_if(m_transactionSet.begin(),m_transactionSet.end(),empty);
		m_transactionSet.erase(i, m_transactionSet.end());
		

		//索引新的Item列表
		for(TransactionSet::iterator iter = m_transactionSet.begin();
			iter != m_transactionSet.end();++iter)
		{
			for(ItemSet::iterator inner = iter->items.begin();
				inner!=iter->items.end();
				++inner)
			{
				setItemMap(*inner,iter->id);
			}

			setClassMap(iter->label,iter->id);
		}
	}
}

const Transaction& TrDB::getTransactionByTid(int nTid) const
{
	for(TransactionSet::const_iterator iter = m_transactionSet.begin();
		iter!=m_transactionSet.end();
		++iter)
	{
		if (iter->id == nTid)
			return *iter;
	}
	return Transaction(-1);
}

int TrDB::getSupport(ClassLabel label) const
{
	//在m_itemTable中，label是以其相反数的形式存储的
	ClassMap::const_iterator iterf = m_classTable.find(label);
	if (iterf != m_classTable.end())
		return iterf->second.size();
	else
		return 0;
}

int TrDB::getSupport(const Item& prefix,ClassLabel label) const
{
	ItemMap::const_iterator iterf = m_itemTable.find(prefix);
	if (iterf!=m_itemTable.end())
	{
		int result =0;
		for(TransactionIndexList::const_iterator iter = iterf->second.begin();
			iter!=iterf->second.end();
			++iter)
		{
			if (m_transactionSet[*iter].label == label)
				++result;
		}
		return result;
	}
	else
		return 0;
}

int TrDB::getSupport(const ItemSet& prefix,ClassLabel label) const 
{
	if (prefix.size() == 1)
		return getSupport(*prefix.begin(),label);

	int count =0;
	for(TransactionSet::const_iterator iter = m_transactionSet.begin();
		iter!=m_transactionSet.end();
		++iter)
	{
		if (set_contain(prefix,iter->items)) //事务包含新的前缀
			++count;
	}
	return count;
}


void TrDB::setItemMap(const Item& item,int tid)
{
	//创建头表
	ItemMap::iterator iterf = m_itemTable.find(item);
	if (iterf == m_itemTable.end()) //没有之前的
	{
		TransactionIndexList firstList;
		firstList.push_back(tid);
		m_itemTable.insert(make_pair(item,firstList));
	}
	else
		iterf->second.push_back(tid);
}

void TrDB::setClassMap(const ClassLabel& label,int tid)
{
	//创建头表
	ClassMap::iterator iterf = m_classTable.find(label);
	if (iterf == m_classTable.end()) //没有之前的
	{
		TransactionIndexList firstList;
		firstList.push_back(tid);
		m_classTable.insert(make_pair(label,firstList));
	}
	else
		iterf->second.push_back(tid);
}

const TransactionIndexList& TrDB::getTransactionsByClass(const ClassLabel& label)const
{
	ClassMap::const_iterator iterf = m_classTable.find(label);
	if(iterf!=m_classTable.end())
		return iterf->second;
	else
		return TransactionIndexList();
}

const TransactionIndexList& TrDB::getTransactionsByItem(const Item& item) const
{
	ItemMap::const_iterator iterf = m_itemTable.find(item);
	if(iterf!=m_itemTable.end())
		return iterf->second;
	else
		return TransactionIndexList();
}
