// DataMiningExperiment.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Transaction.h"
#include "TrDB.h"

int _tmain(int argc, _TCHAR* argv[])
{

	TrDB trdb;
	trdb.createFromFile("mushroom.dat",10);
	
	return 0;
}

