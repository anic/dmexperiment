#pragma once

#include "DMAlgorithm.h"

/*!
 * \brief
 * 实现DDPMine算法
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

