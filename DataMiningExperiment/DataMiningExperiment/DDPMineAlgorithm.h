#pragma once

#include "DMAlgorithm.h"

/*!
 * \brief
 * ʵ��DDPMine�㷨
 */
class DDPMineAlgorithm :
	public DMAlgorithm
{
private:
	ItemSet prefix;
	void DDPMine(TrDB& trdb, int min_sup);
    void branch_and_bound(const TrDB& trdb, int min_sup, const std::set<Item>& a);
	double computeIG(const TrDB& trdb, ItemSet_Support& iset);
	void update_tree(TrDB& trdb, const ItemSet& iset);
	bool getElement(const TrDB& trdb, ItemSet &current);
	double computeIGup(const TrDB& trdb, ItemSet_Support& iset);
	void computeTa(const TrDB &trdb, ItemSet &iset);
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

