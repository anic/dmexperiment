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
	 * 执行算法
	 * 
	 * \param trdb
	 * 原始的数据库
	 * 
	 * \param nSupport
	 * 用户定义的支持度
	 * 
	 * \returns
	 * 算法是否执行成功，是则返回true;否则false
	 * 
	 */
	virtual bool execute(const TrDB& trdb,int nSupport) = 0;

	
	/*!
	 * \brief
	 * 获得算法返回的结果
	 * 
	 * \returns
	 * 算法结果
	 */
	const Result& getResult()const {return m_result;}
};
