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
		//创建类支持度表
		ClassSupportTable::iterator iterf = m_classSupport.find(t.label);
		if (iterf == m_classSupport.end()) //没有之前的
			m_classSupport.insert(::make_pair(t.label,1));
		else
			iterf->second++;

		while(ss>>s)
		{
			Item item = ::atoi(s.substr(0,s.find_first_of(':')).c_str());
			t.items.insert(item);

			//创建头表
			ItemMap::iterator iterf = m_itemTable.find(item);
			if (iterf == m_itemTable.end()) //没有之前的
			{
				ItemList firstList;
				firstList.push_back(index);
				m_itemTable.insert(::make_pair(item,firstList));
			}
			else
				iterf->second.push_back(index);
			
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
	else if (nNewPrefix == 1) //如果是添加一个新的前缀，可以利用已有的头表索引
	{
		ItemMap::const_iterator iter = parent.m_itemTable.find(*prefix.begin());
		for(ItemList::const_iterator iter2 = iter->second.begin();
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

		//创建类支持度表
		ClassSupportTable::iterator iterf = m_classSupport.find(iter->label);
		if (iterf == m_classSupport.end()) //没有之前的
			m_classSupport.insert(::make_pair(iter->label,1));
		else
			iterf->second++;

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
	ClassSupportTable::const_iterator iterf = m_classSupport.find(label);
	if (iterf != m_classSupport.end()) //没有之前的
		return iterf->second;
	else
		return 0;
			
}

int TrDB::getSupport(const Item& prefix,ClassLabel label) const
{
	ItemSet items;
	items.insert(prefix);
	return getSupport(items,label);
}

int TrDB::getSupport(const ItemSet& prefix,ClassLabel label) const 
{
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
