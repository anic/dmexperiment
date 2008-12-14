#ifndef _TRDB_H
#define _TRDB_H
#pragma once

#include <vector>
#include <string>
#include "Transaction.h"


typedef std::vector<Transaction> TransactionSet;

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