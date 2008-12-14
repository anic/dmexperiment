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

		if (maxLength >0 && index >=maxLength) //����Ѿ�������������������
			break;

		Transaction t(index);
		t.label = ::atoi(s.c_str());
		//������֧�ֶȱ�
		ClassSupportTable::iterator iterf = m_classSupport.find(t.label);
		if (iterf == m_classSupport.end()) //û��֮ǰ��
			m_classSupport.insert(::make_pair(t.label,1));
		else
			iterf->second++;

		while(ss>>s)
		{
			Item item = ::atoi(s.substr(0,s.find_first_of(':')).c_str());
			t.items.insert(item);

			//����ͷ��
			ItemMap::iterator iterf = m_itemTable.find(item);
			if (iterf == m_itemTable.end()) //û��֮ǰ��
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
	//��ǰ׺����
	this->m_prefix.insert(parent.m_prefix.begin(),parent.m_prefix.end());
	this->m_prefix.insert(prefix.begin(),prefix.end());

	int nNewPrefix = prefix.size();
	if (nNewPrefix >1 || nNewPrefix == 0)
	{
		const TransactionSet& transactions = parent.getTransaction();
		for(TransactionSet::const_iterator iter = transactions.begin();
			iter!=transactions.end();++iter)
		{
			if (set_contain(m_prefix,iter->items)) //��������µ�ǰ׺������뵽ͶӰ���ݿ�
				m_transactionSet.push_back(*iter);
		}
	}
	else if (nNewPrefix == 1) //��������һ���µ�ǰ׺�������������е�ͷ������
	{
		ItemMap::const_iterator iter = parent.m_itemTable.find(*prefix.begin());
		for(ItemList::const_iterator iter2 = iter->second.begin();
			iter2!=iter->second.end();
			++iter2)
		{
			m_transactionSet.push_back(parent.m_transactionSet[*iter2]);
		}
	}

	//����֧�ֶ�
	map<Item,int> header;
	for(TransactionSet::iterator iter = m_transactionSet.begin();
		iter != m_transactionSet.end();++iter)
	{
		for(ItemSet::iterator inner = (*iter).items.begin();
			inner != (*iter).items.end();
			++inner)
		{
			map<Item,int>::iterator iterf = header.find(*inner);
			if (iterf == header.end()) //û��֮ǰ��
				header.insert(::make_pair(*inner,1));
			else
				iterf->second++;
		}

		//������֧�ֶȱ�
		ClassSupportTable::iterator iterf = m_classSupport.find(iter->label);
		if (iterf == m_classSupport.end()) //û��֮ǰ��
			m_classSupport.insert(::make_pair(iter->label,1));
		else
			iterf->second++;

	}
	
	//����������֧�ֶȵ�Item
	ItemSet unsupported;
	for(map<Item,int>::iterator iter = header.begin();
		iter !=header.end();++iter)
	{
		if (iter->second < nMinSupport)
			unsupported.insert(iter->first);
	}
	unsupported.insert(m_prefix.begin(),m_prefix.end()); //ǰ׺ҲҪȥ��

	//ȥ��������֧�ֶȵ���
	if(!unsupported.empty())
	{
		for(TransactionSet::iterator iter = m_transactionSet.begin();
			iter != m_transactionSet.end();++iter)
		{
			ItemSet removed;

			//�󼯺ϲ�
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
	if (iterf != m_classSupport.end()) //û��֮ǰ��
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
		if (set_contain(prefix,iter->items)) //��������µ�ǰ׺
			++count;
	}
	return count;
}
