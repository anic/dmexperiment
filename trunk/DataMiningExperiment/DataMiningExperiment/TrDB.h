#ifndef _TRDB_H
#define _TRDB_H
#pragma once

#include <vector>
#include <string>
#include <map>
#include "Transaction.h"


typedef std::vector<Transaction> TransactionSet;
typedef std::vector<int> ItemList; //��¼TransactionSet�е�Id��
typedef std::map<Item,ItemList> ItemMap;

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
	 * ��ı�
	 */
	ItemMap m_itemTable;

public:
	
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