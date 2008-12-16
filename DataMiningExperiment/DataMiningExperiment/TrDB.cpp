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
}

TrDB::~TrDB(void)
{
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

	ifstream fs(filename.c_str());
	string line;

	int index = 0;
	while(std::getline(fs,line)){

		stringstream ss(line);
		string s;	
		ss>>s;

		if (maxLength >0 && index >=maxLength) //����Ѿ�������������������
			break;

		Transaction* t = new Transaction(index);
		t->label = ::atoi(s.c_str());

		//��¼class��transaction�Ķ�Ӧ
		this->setClassMap(t->label,index);

		while(ss>>s)
		{
			Item item = ::atoi(s.substr(0,s.find_first_of(':')).c_str());
			t->items.insert(item);

			//��¼item��transaction�Ķ�Ӧ
			this->setItemMap(item,index);
		}
		index++;
		m_transactionSet.push_back(t);
	}
	fs.close();

}

void TrDB::createConditionalDB(const TrDB &parent, Item prefix,int nMinSupport)
{
	ItemSet prefixSet;
	prefixSet.insert(prefix);
	createConditionalDB(parent,prefixSet,nMinSupport);
}


bool empty(const Transaction* t) {
	return t->items.empty();
}



void TrDB::createConditionalDB(const TrDB &parent, const ItemSet &prefix,int nMinSupport)
{

	m_nMinSupport = nMinSupport;
	m_pRawTrDB = parent.m_pRawTrDB; //��ø���TrDB

	//��ǰ׺����
	this->m_prefix.insert(parent.m_prefix.begin(),parent.m_prefix.end());
	this->m_prefix.insert(prefix.begin(),prefix.end());
	/* //�ɴ���
	int nNewPrefix = prefix.size();
	if (nNewPrefix >1 || nNewPrefix == 0)
	{
	const TransactionSet& transactions = parent.getTransaction();
	for(TransactionSet::const_iterator iter = transactions.begin();
	iter!=transactions.end();++iter)
	{
	if (set_contain(m_prefix,(*iter)->items)) //��������µ�ǰ׺������뵽ͶӰ���ݿ�
	m_transactionSet.push_back(*iter);
	}
	}
	else if (nNewPrefix == 1) //��������һ���µ�ǰ׺�������������е�ͷ����������
	{
	ItemMap::const_iterator iter = parent.m_itemTable.find(*prefix.begin());
	for(TransactionIndexList::const_iterator iter2 = iter->second.begin();
	iter2!=iter->second.end();
	++iter2)
	{
	//�������ݵ�ָ��
	m_transactionSet.push_back(parent.m_transactionSet[*iter2]);
	}
	}

	//����֧�ֶ�
	map<Item,int> header;
	for(TransactionSet::iterator iter = m_transactionSet.begin();
	iter != m_transactionSet.end();++iter)
	{
	for(ItemSet::iterator inner = (*iter)->items.begin();
	inner != (*iter)->items.end();
	++inner)
	{
	map<Item,int>::iterator iterf = header.find(*inner);
	if (iterf == header.end()) //û��֮ǰ��
	header.insert(::make_pair(*inner,1));
	else
	iterf->second++;
	}
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
	(*iter)->items.begin(),
	(*iter)->items.end(),
	unsupported.begin(),
	unsupported.end(),
	std::insert_iterator<ItemSet>(removed, removed.begin() ));

	(*iter)->items.swap(removed);
	}

	//ȥ���յ�����
	TransactionSet::iterator i = remove_if(m_transactionSet.begin(),m_transactionSet.end(),empty);
	m_transactionSet.erase(i, m_transactionSet.end());


	//�����µ�Item�б�
	for(TransactionSet::iterator iter = m_transactionSet.begin();
	iter != m_transactionSet.end();++iter)
	{
	for(ItemSet::iterator inner = (*iter)->items.begin();
	inner!=(*iter)->items.end();
	++inner)
	{
	setItemMap(*inner,iter - m_transactionSet.begin());
	}

	setClassMap((*iter)->label,iter - m_transactionSet.begin());
	}
	}
	*/

	int nNewPrefix = prefix.size();
	if (nNewPrefix >1 || nNewPrefix == 0)
	{
		throw exception("prefix must contain only 1 element.");
	}
	else if (nNewPrefix == 1) //��������һ���µ�ǰ׺�������������е�ͷ����������
	{
		ItemMap::const_iterator iterf = parent.m_itemTable.find(*prefix.begin());
		if (iterf == parent.m_itemTable.end())
			this->m_nTransactionSize = 0;
		else
			this->m_nTransactionSize = iterf->second->size();

		if (m_nTransactionSize == 0 || m_nTransactionSize < nMinSupport) //������Ŀ����֧�ֶȣ�˵�����е�Item����������֧�ֶ�
			return;

		for(ItemMap::const_iterator iter = parent.m_itemTable.begin();
			iter!=parent.m_itemTable.end();
			++iter)
		{
			//����ÿһ��parent��itemtable�����뵽�µ�m_itemTable��

			//��TransactionIndexList��������
			if (iterf!=iter)
			{
				TransactionIndexList* temp = new TransactionIndexList();

				std::set_intersection(iterf->second->begin(),
					iterf->second->end(),
					iter->second->begin(),
					iter->second->end(),
					std::insert_iterator<ItemSet>(*temp, temp->begin() ));

				int resultSize = temp->size();
				if (resultSize >0 && resultSize>= nMinSupport)
					m_itemTable.insert(std::make_pair(iter->first,temp));
				else
					delete temp;
			}
		}

		for(ClassMap::const_iterator iter = parent.m_classTable.begin();
			iter!=parent.m_classTable.end();
			++iter)
		{
			//����ÿһ�������������
			TransactionIndexList* temp = new TransactionIndexList();

			std::set_intersection(iterf->second->begin(),
				iterf->second->end(),
				iter->second->begin(),
				iter->second->end(),
				std::insert_iterator<ItemSet>(*temp, temp->begin() ));

			if (!temp->empty())
				m_classTable.insert(std::make_pair(iter->first,temp));
			else
				delete temp;
		}



	}
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
	ItemMap::const_iterator iterf = m_itemTable.find(prefix);
	if (iterf!=m_itemTable.end())
	{
		int result =0;
		for(TransactionIndexList::const_iterator iter = iterf->second->begin();
			iter!=iterf->second->end();
			++iter)
		{
			if (m_bSource)
			{
				if (m_transactionSet[*iter]->label == label)
					++result;
			}
			else if (m_pRawTrDB->m_transactionSet[*iter]->label == label)
			{
				++result;
			}
		}
		return result;
	}
	else
		return 0;
}



void TrDB::setItemMap(Item item,int tIndex)
{
	//����ͷ��
	ItemMap::iterator iterf = m_itemTable.find(item);
	if (iterf == m_itemTable.end()) //û��֮ǰ��
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
	//����ͷ��
	ClassMap::iterator iterf = m_classTable.find(label);
	if (iterf == m_classTable.end()) //û��֮ǰ��
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
	ItemMap::const_iterator iterf = m_itemTable.find(item);
	if(iterf==m_itemTable.end())
		return;

	//std::vector<Item> removedItem;

	TransactionIndexList* removedTrans = iterf->second; //Ҫ�Ƴ���transacions 
	for(ItemMap::iterator iter = m_itemTable.begin();
		iter!=m_itemTable.end();
		)
	{
		TransactionIndexList* result = new TransactionIndexList();
		TransactionIndexList* pre = iter->second;

		std::set_difference(pre->begin(),
			pre->end(),
			removedTrans->begin(),
			removedTrans->end(),
			std::insert_iterator<TransactionIndexList>(*result, result->begin() ));

		iter->second = result;
		delete pre;

		/*if (result->empty())
			removedItem.push_back(iter->first);*/
	}

	//for(std::vector<Item>::const_iterator iter = removedItem.begin();
	//	iter != removedItem.end();++iter)
	//	m_itemTable.re);
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