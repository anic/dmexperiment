#include "StdAfx.h"
#include "Transaction.h"

Transaction::Transaction(int nId,ClassLabel label)
{
	id = nId;
	this->label = label;
}
Transaction::Transaction(int nId)
{
	id = nId;
	label = 0;
}
Transaction::Transaction(void)
{
	id = 0;
	label = 0;
}