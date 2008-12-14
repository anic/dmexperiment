#ifndef _TRDB_H
#define _TRDB_H
#pragma once

#include <vector>
#include <string>
#include "Transaction.h"


typedef std::vector<Transaction> TransactionSet;

class TrDB
{
	//���񼯺�
	TransactionSet m_transactionSet;

	//ǰ׺��
	ItemSet m_prefix;

public:
	
	TransactionSet::const_iterator getBegin() const{return m_transactionSet.begin();}
	TransactionSet::const_iterator getEnd() const{return m_transactionSet.end();}
	const Transaction& operator[](size_t index)const {return m_transactionSet[index];}
	
	//���ǰ׺
	const ItemSet& getPrefix() const{return m_prefix;}

	//���ļ��ж�ȡΪѵ�����ݿ�
	void createFromFile(std::string filename);
	
	//��Ϊ���ԣ���ȡ���maxLength��
	void createFromFile(std::string filename,int maxLength);

	TrDB(void);
	virtual ~TrDB(void);
};


#endif