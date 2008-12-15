#ifndef _TRDB_H
#define _TRDB_H
#pragma once

#include <vector>
#include <string>
#include <map>
#include "Transaction.h"


typedef std::vector<Transaction> TransactionSet;
typedef std::vector<int> TransactionIndexList; //记录TransactionSet中的Id号
typedef std::map<Item,TransactionIndexList> ItemMap;
typedef std::map<ClassLabel,TransactionIndexList> ClassMap;

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
private:
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


	/*!
	 * \brief
	 * 项的表,格式为 {2,{0,1,2}} 表示 2这个item,对应m_transactionSet[0],m_transactionSet[1],m_transactionSet[2]
	 * 这个不是tid,而是对应的下标
	 *
	 * 这个表还存了classlabel {-1,{0,1,2,3,4}},{-2,{5,6,7,8}}，用相反数表示了classLabel对应的transaction
	 */
	ItemMap m_itemTable;

	ClassMap m_classTable;

	void setItemMap(Item item,int tIndex);
	void setClassMap(ClassLabel label,int tIndex);
	
public:

	const TransactionIndexList& getTransactionsByClass(ClassLabel label)const;

	const TransactionIndexList& getTransactionsByItem(Item item) const;

	//如果只有一个Item，用这个方法效率比较高
	int getSupport(Item prefix,ClassLabel label) const;

	//这个方法效率低，慎用
	int getSupport(const ItemSet& prefix,ClassLabel label) const ;

	//返回对应Label的支持度
	int getSupport(ClassLabel label) const;

	int getSize()const{return m_transactionSet.size(); }

	//返回Tid的事务，注意此方法效率较低，能不用不用
	//如果没有找到，返回id为-1的事务
	const Transaction& getTransactionByTid(int nTid)const;

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


	const ItemMap& getItemTable() const{return m_itemTable;}

	const ClassMap& getClassTable() const{return m_classTable;}

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
	 * \param nMinSupport
	 * 最小支持度
	 *
	 * \remarks
	 * 新的前缀是在上级投影数据库的前缀上，加上prefix；投影的时候会将不满足最小支持度的item删除
	 * 
	 */
	void createConditionalDB(const TrDB& parent,const ItemSet& prefix,int nMinSupport);

	
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
	 * \param nMinSupport
	 * 最小支持度
	 *
	 * \remarks
	 * 新的前缀是在上级投影数据库的前缀上，加上prefix；投影的时候会将不满足最小支持度的item删除
	 * 
	 */
	void createConditionalDB(const TrDB& parent,const Item& prefix,int nMinSupport);



	TrDB(void);
	virtual ~TrDB(void);
};


#endif