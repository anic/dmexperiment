#pragma once

#include "DMAlgorithm.h"

/*!
 * \brief
 * ʵ��DDPMine�㷨
 */
class DDPMineAlgorithm :
	public DMAlgorithm
{
public:
	DDPMineAlgorithm(void);
	~DDPMineAlgorithm(void);

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

