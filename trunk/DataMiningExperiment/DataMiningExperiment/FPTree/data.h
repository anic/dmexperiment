/*----------------------------------------------------------------------
File    : Data.h
Contents: data set management
Author  : Bart Goethals
Update  : 4/4/2003
----------------------------------------------------------------------*/
#pragma once
#include <stdio.h>
namespace FPTree
{
	class Transaction
	{
	public:

		Transaction(int l) : length(l) {t = new int[l];}
		Transaction(const Transaction &tr);
		~Transaction(){delete t;}

		int length;
		int *t;
	};

	class Data
	{
	public:

		Data(char *filename, int t=0);
		~Data();

		Transaction *getNext();

	private:

		Transaction *getNextAs();
		Transaction *getNextAsFlat();
		Transaction *getNextAsQuest();
		Transaction *getNextBin();

		FILE *in;
		char *fn;
		int current;
		int type;
	};
}