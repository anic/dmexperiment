#ifndef _TRDB_H
#define _TRDB_H
#pragma once

#include <vector>
#include <string>
#include "Transaction.h"


typedef std::vector<Transaction> TransactionSet;

/*!
 * \brief
 * 数据库
 * 
 * 既是训练数据库，又是投影数据库；训练数据库中的prefix没有item，而投影数据库中有
 * 
 * \remarks
 * 训练数据库中的prefix没有item，而投影数据库中有
 * 
 */
class TrDB
{
	
	/*!
	 * \brief
	 * 事务集合
	 */
	TransactionSet m_transactionSet;

	
	/*!
	 * \brief
	 * 前缀集
	 */
	ItemSet m_prefix;

public:
	
	/*!
	 * \brief
	 * 获得事务集
	 * 
	 * \returns
	 * 获得事务集
	 * 
	 */
	const TransactionSet& getTransaction() const{return m_transactionSet;}
	
	/*!
	 * \brief
	 * 获得前缀
	 * 
	 * \returns
	 * 前缀集合
	 * 
	 */
	const ItemSet& getPrefix() const{return m_prefix;}

	//从文件中读取为训练数据库
	void createFromFile(std::string filename);
	
	//作为测试，读取最多maxLength行
	void createFromFile(std::string filename,int maxLength);

	/*!
	 * \brief
	 * 创建投影数据库
	 * 
	 * \param parent
	 * 上级投影数据库
	 * 
	 * \param prefix
	 * 投影“添加的”前缀集合
	 * 
	 * \remarks
	 * 新的前缀是在上级投影数据库的前缀上，加上prefix
	 * 
	 */
	void createConditionalDB(const TrDB& parent,const ItemSet& prefix);

	
	/*!
	 * \brief
	 * 创建投影数据库
	 * 
	 * \param parent
	 * 上级投影数据库
	 * 
	 * \param prefix
	 * 投影“添加的”前缀集合
	 * 
	 * \remarks
	 * 新的前缀是在上级投影数据库的前缀上，加上prefix
	 * 
	 */
	void createConditionalDB(const TrDB& parent,const Item& prefix);



	TrDB(void);
	virtual ~TrDB(void);
};


#endif