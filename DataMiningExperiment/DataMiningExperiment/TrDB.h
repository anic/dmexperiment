#ifndef _TRDB_H
#define _TRDB_H
#pragma once

#include <vector>
#include <string>
#include <map>
#include "Transaction.h"


typedef std::vector<Transaction> TransactionSet;
typedef std::vector<int> TransactionIndexList; //��¼TransactionSet�е�Id��
typedef std::map<Item,TransactionIndexList> ItemMap;
typedef std::map<ClassLabel,TransactionIndexList> ClassMap;

/*!
 * \brief
 * ���ݿ�
 * 
 * ����ѵ�����ݿ⣬����ͶӰ���ݿ⣻ѵ�����ݿ��е�prefixû��item����ͶӰ���ݿ�����
 * 
 * \remarks
 * ѵ�����ݿ��е�prefixû��item����ͶӰ���ݿ�����
 * 
 */
class TrDB
{
private:
	/*!
	 * \brief
	 * ���񼯺�
	 */
	TransactionSet m_transactionSet;

	
	/*!
	 * \brief
	 * ǰ׺��
	 */
	ItemSet m_prefix;


	/*!
	 * \brief
	 * ��ı�,��ʽΪ {2,{0,1,2}} ��ʾ 2���item,��Ӧm_transactionSet[0],m_transactionSet[1],m_transactionSet[2]
	 * �������tid,���Ƕ�Ӧ���±�
	 *
	 * ���������classlabel {-1,{0,1,2,3,4}},{-2,{5,6,7,8}}�����෴����ʾ��classLabel��Ӧ��transaction
	 */
	ItemMap m_itemTable;

	ClassMap m_classTable;

	void setItemMap(Item item,int tIndex);
	void setClassMap(ClassLabel label,int tIndex);
	
public:

	const TransactionIndexList& getTransactionsByClass(ClassLabel label)const;

	const TransactionIndexList& getTransactionsByItem(Item item) const;

	//���ֻ��һ��Item�����������Ч�ʱȽϸ�
	int getSupport(Item prefix,ClassLabel label) const;

	//�������Ч�ʵͣ�����
	int getSupport(const ItemSet& prefix,ClassLabel label) const ;

	//���ض�ӦLabel��֧�ֶ�
	int getSupport(ClassLabel label) const;

	int getSize()const{return m_transactionSet.size(); }

	//����Tid������ע��˷���Ч�ʽϵͣ��ܲ��ò���
	//���û���ҵ�������idΪ-1������
	const Transaction& getTransactionByTid(int nTid)const;

	/*!
	 * \brief
	 * �������
	 * 
	 * \returns
	 * �������
	 * 
	 */
	const TransactionSet& getTransaction() const{return m_transactionSet;}
	
	/*!
	 * \brief
	 * ���ǰ׺
	 * 
	 * \returns
	 * ǰ׺����
	 * 
	 */
	const ItemSet& getPrefix() const{return m_prefix;}


	const ItemMap& getItemTable() const{return m_itemTable;}

	const ClassMap& getClassTable() const{return m_classTable;}

	//���ļ��ж�ȡΪѵ�����ݿ�
	void createFromFile(std::string filename);
	
	//��Ϊ���ԣ���ȡ���maxLength��
	void createFromFile(std::string filename,int maxLength);

	/*!
	 * \brief
	 * ����ͶӰ���ݿ�
	 * 
	 * \param parent
	 * �ϼ�ͶӰ���ݿ�
	 * 
	 * \param prefix
	 * ͶӰ����ӵġ�ǰ׺����
	 * 
	 * \param nMinSupport
	 * ��С֧�ֶ�
	 *
	 * \remarks
	 * �µ�ǰ׺�����ϼ�ͶӰ���ݿ��ǰ׺�ϣ�����prefix��ͶӰ��ʱ��Ὣ��������С֧�ֶȵ�itemɾ��
	 * 
	 */
	void createConditionalDB(const TrDB& parent,const ItemSet& prefix,int nMinSupport);

	
	/*!
	 * \brief
	 * ����ͶӰ���ݿ�
	 * 
	 * \param parent
	 * �ϼ�ͶӰ���ݿ�
	 * 
	 * \param prefix
	 * ͶӰ����ӵġ�ǰ׺����
	 * 
	 * \param nMinSupport
	 * ��С֧�ֶ�
	 *
	 * \remarks
	 * �µ�ǰ׺�����ϼ�ͶӰ���ݿ��ǰ׺�ϣ�����prefix��ͶӰ��ʱ��Ὣ��������С֧�ֶȵ�itemɾ��
	 * 
	 */
	void createConditionalDB(const TrDB& parent,const Item& prefix,int nMinSupport);



	TrDB(void);
	virtual ~TrDB(void);
};


#endif