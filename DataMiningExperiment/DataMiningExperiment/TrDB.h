#ifndef _TRDB_H
#define _TRDB_H
#pragma once

#include <vector>
#include <string>
#include <map>
#include "Transaction.h"


typedef std::vector<Transaction*> TransactionSet;
typedef std::set<int> TransactionIndexList; //��¼RawTrDB��TransactionSet�е��±��
typedef std::map<Item,TransactionIndexList*> ItemMap;
typedef std::map<ClassLabel,TransactionIndexList*> ClassMap;

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
protected:
	/*!
	 * \brief
	 * ���񼯺�
	 */
	TransactionSet m_transactionSet;

	TrDB * m_pRawTrDB;
	
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

	mutable std::vector<int> m_cachedSupport;

	int m_nMinSupport;

	void setItemMap(Item item,int tIndex);

	void setClassMap(ClassLabel label,int tIndex);

	bool m_bSource; //�Ƿ���Դ

	int m_nTransactionSize;
	
	int getCachedIndex(Item item,ClassLabel label) const;

	void allocateCached() const;

	int m_nClassSize; //����ԭʼ���ݿ⣬�������ݿ�Ķ����ó�0

	//���һ�¿�����Ҫɾ����Transacion�Ƿ���Ҫɾ��������ǣ����뵽removedTrans��
	void checkPossibleRemoveTrans(TransactionIndexList& possibleRemovedTrans,TransactionIndexList& removedTrans);

	//��classtable��remove Transacion
	void removeTransFromClassTable(TransactionIndexList& removedTrans);

	std::vector<int>* m_pClassRemap;



public:

	ClassLabel getClass(ClassLabel alias)const;

	void setMinSupport(int nMinSupport);

	int getMinSupport() const{ return m_nMinSupport;}

	const TransactionIndexList* getTransactionsByClass(ClassLabel label)const;

	const TransactionIndexList* getTransactionsByItem(Item item) const;

	//���ֻ��һ��Item�����������Ч�ʱȽϸ�
	int getSupport(Item prefix,ClassLabel label) const;

	//���ض�ӦLabel��֧�ֶ�
	int getSupport(ClassLabel label) const;

	int getSize()const;

	//����Tid������ע��˷���Ч�ʽϵͣ��ܲ��ò���
	//���û���ҵ�������idΪ-1������
	const Transaction* getTransactionByTid(int nTid)const;

	/*!
	 * \brief
	 * �������
	 * 
	 * \returns
	 * �������
	 * 
	 */
	const TransactionSet& getTransaction() const;
	
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
	void createConditionalDB(const TrDB& parent,Item prefix,int nMinSupport,bool removeEmptyTrans = false);

	void removeItem(Item item);

	//ɾ��ͬʱ����items����
	void removeItem(const ItemSet& items);

	//�Ƿ���Դ�����ݿ�
	bool isSource()const{ return m_bSource;}

	const TrDB* getSourceTrDB()const { return m_pRawTrDB;}

	//����һ��Item���Ϻ����ǩ����ö�Ӧ��֧�ֶ�	
	int getSupport(const ItemSet& items,ClassLabel label) const;

	//����һ��Item���ϣ��������ǩ����ö�Ӧ��֧�ֶ�
	int getSupport(const ItemSet& items) const;


	TrDB(void);
	virtual ~TrDB(void);

	TrDB(const TrDB& trdb);
};


#endif