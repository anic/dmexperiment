#pragma once
#include <vector>
#include "common.h"
#include "TrDB.h"

typedef std::vector<Rule> Result;

class DMAlgorithm
{
protected:
	Result m_result;
public:
	
	/*!
	 * \brief
	 * ִ���㷨
	 * 
	 * \param trdb
	 * ԭʼ�����ݿ�
	 * 
	 * \param nSupport
	 * �û������֧�ֶ�
	 * 
	 * \returns
	 * �㷨�Ƿ�ִ�гɹ������򷵻�true;����false
	 * 
	 */
	virtual bool execute(const TrDB& trdb,int nSupport) = 0;

	
	/*!
	 * \brief
	 * ����㷨���صĽ��
	 * 
	 * \returns
	 * �㷨���
	 */
	const Result& getResult()const {return m_result;}
};
