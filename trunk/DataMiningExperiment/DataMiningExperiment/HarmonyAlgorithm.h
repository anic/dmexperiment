#pragma once

#include "DMAlgorithm.h"


/*!
 * \brief
 * ʵ��Harmony�㷨
 */
class HarmonyAlgorithm:
	public DMAlgorithm
{
public:
	HarmonyAlgorithm(void);
	~HarmonyAlgorithm(void);

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
	bool execute(const TrDB& trdb,int nSupport);
};
