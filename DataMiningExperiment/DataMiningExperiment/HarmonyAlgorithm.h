#pragma once

#include "DMAlgorithm.h"


/*!
 * \brief
 * 实现Harmony算法
 */
class HarmonyAlgorithm:
	public DMAlgorithm
{
public:
	HarmonyAlgorithm(void);
	~HarmonyAlgorithm(void);

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
	bool execute(const TrDB& trdb,int nSupport);
};
