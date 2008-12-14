#ifndef _TRDB_H
#define _TRDB_H
#pragma once

#include <vector>
#include <string>
#include "Transaction.h"


typedef std::vector<Transaction> TransactionSet;

class TrDB
{
	//事务集合
	TransactionSet m_transactionSet;

	//前缀集
	ItemSet m_prefix;

public:
	
	TransactionSet::const_iterator getBegin() const{return m_transactionSet.begin();}
	TransactionSet::const_iterator getEnd() const{return m_transactionSet.end();}
	const Transaction& operator[](size_t index)const {return m_transactionSet[index];}
	
	//获得前缀
	const ItemSet& getPrefix() const{return m_prefix;}

	//从文件中读取为训练数据库
	void createFromFile(std::string filename);
	
	//作为测试，读取最多maxLength行
	void createFromFile(std::string filename,int maxLength);

	TrDB(void);
	virtual ~TrDB(void);
};


#endif