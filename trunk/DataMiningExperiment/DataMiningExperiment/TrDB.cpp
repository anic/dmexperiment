#include "StdAfx.h"
#include "TrDB.h"
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
	ifstream fs(filename.c_str());
	string line;
	
	int index = 0;
	while(std::getline(fs,line)){
		
		stringstream ss(line);
		string s;	
		ss>>s;

		Transaction t(index);
		t.label = ::atoi(s.c_str());
		while(ss>>s)
		{
			Item item = ::atoi(s.substr(0,s.find_first_of(':')).c_str());
			t.items.insert(item);
		}
		index++;
		//减少判断次数，所以重载没有修改
		m_transactionSet.push_back(t);
	}
	fs.close();
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

		Transaction t(index);
		t.label = ::atoi(s.c_str());
		while(ss>>s)
		{
			Item item = ::atoi(s.substr(0,s.find_first_of(':')).c_str());
			t.items.insert(item);
		}
		index++;

		if (maxLength >0 && index >maxLength)
			break;
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

	const TransactionSet& transactions = parent.getTransaction();
	for(TransactionSet::const_iterator iter = transactions.begin();
		iter!=transactions.end();++iter)
	{
		ItemSet temp;
		::set_difference(m_prefix.begin(),
			m_prefix.end(),
			iter->items.begin(),
			iter->items.end(),
			std::insert_iterator<ItemSet>(temp, temp.begin() ));

		if (temp.empty()) //事务包含新的前缀，则插入到投影数据库
			m_transactionSet.push_back(*iter);
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
	unsupported.insert(m_prefix.begin(),m_prefix.end());

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

