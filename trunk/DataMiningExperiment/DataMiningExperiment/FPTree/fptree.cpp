/*----------------------------------------------------------------------
File    : fptree.cpp
Contents: fpgrowth algorithm for finding frequent sets
Author  : Bart Goethals
Update  : 08/04/2003 - single prefix path bug fixed (Thanks to Xiaonan Wang)
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <set>
using namespace std;
#include "data.h"
#include "item.h"
#include "fptree.h"
namespace fptree
{
	int *FPtree::remap = 0;
	set<Element> *FPtree::relist = 0;
	int FPtree::len_remap = 0;

	FPtree::FPtree()
	{
		root = new set<Item>;
		nodes = 0;
		singlepath=true;
	}

	FPtree::~FPtree()
	{
		set<Item>::iterator it;

		for(it = root->begin(); it != root->end(); it++)
			it->removeChildren();
		delete root;
	}

	
	int FPtree::processItems(Transaction *t, int times)
	{
		//插入一个新事务“到头表中”

		set<Item>::iterator head;
		int added=0;

		for(int depth=0; depth < t->length; depth++) {
			head = header.find(Item(t->t[depth], 0));
			if(head == header.end()) { //没有找到，添加一个
				head = header.insert(Item(t->t[depth], 0)).first; //在头表中加入
				added++;
			}
			head->Increment(times); //增加在头表中的支持度（找到和没有找到都是）
		}
		return added;
	}

	int FPtree::processTransaction(Transaction *t, int times)
	{
		//插入一个新事务

		set<Item>::iterator it, head;
		set<Item>* items = root;
		Item_ *current = 0;
		int added=0;

		for(int depth=0; depth < t->length; depth++) {
			head = header.find(Item(t->t[depth], 0));		//在头表中找
			if(head != header.end()) {						//如果有，也就是如果没有被minsup删除prune掉
				it = items->find(Item(t->t[depth], 0));		//开始从root开始找第一个项

				if(it == items->end()) {								//没有找到项
					it = items->insert(Item(t->t[depth], current)).first;	//添加一个新的
					it->setNext(head->getNext());
					head->setNext(it->getItem());
					nodes++;
					added++;
					if(singlepath && (items->size()>1)) singlepath=false; 		
				}

				it->Increment(times);
				current = it->getItem();
				items = it->makeChildren();
			}
		}

		return added;
	}

	int FPtree::grow(int *current, int depth)
	{
		int added=0, factor=1;
		if(header.size() == 0) return 0;

		if(singlepath) { 
			int *comb = new int[header.size()];		//添加的前缀
			int cl=0;
			for(set<Item>::iterator it=header.begin(); it != header.end(); ++it) {
				current[depth-1] = it->getId();			//因为单枝，所以最后一定是这个
				/*
				current = 1,5,6
				header = {4,7,8}
				current = 1,5,6,4;
				current = 1,5,6,7
				current = 1,5,6,8 //因为当前这棵树已经是单枝了
				*/

				print(current,depth,comb,cl,it->getSupport());	//输出当前的结果到文件，说明找到一个满足的规则
				comb[cl++] = it->getId();
				added += factor;
				factor *= 2;
			}
			delete [] comb;
		}
		else { //not singlepath 不是单枝
			for(set<Item>::iterator it=header.begin(); it != header.end(); ++it) {
				Item_ *i;
				current[depth-1] = it->getId(); 

				//对于头表中每一个元素，构造一个新的FPTree
				FPtree *cfpt = new FPtree();
				
				//设置最小支持度和输出
				cfpt->setMinsup(minsup); 
				cfpt->setOutput(out);

				int *tmp = new int[header.size()];
				for(i = it->getNext(); i; i = i->nodelink) {
					int l=0;
					for(Item_ *p=i->parent; p; p = p->parent) tmp[l++] = p->id; //遍历父亲

					//获得事务的个数
					Transaction *t = new Transaction(l);
					for(int j=0; j<l; j++) t->t[j] = tmp[l-j-1]; //将事务赋值

					cfpt->processItems(t,i->supp); //将事务插入新的tree中
					delete t;
				}
				cfpt->Prune(); //剪枝

				for(i = it->getNext(); i; i = i->nodelink) {
					int l=0;
					for(Item_ *p=i->parent; p; p = p->parent) tmp[l++] = p->id;
					Transaction *t = new Transaction(l);
					for(int j=0; j<l; j++) t->t[j] = tmp[l-j-1];
					cfpt->processTransaction(t,i->supp);
					delete t;
				}
				delete [] tmp;
				print(current,depth,0,0,it->getSupport()); //输出当前结果

				/*
				current = 1,5,6
				header = {4,7,10,8,9}, 
				4___7_8
				  |_10_8_9

			    1,5,6,4
				1,5,6,8,4,7,10(因为在图中,拿到一个8，则4,7也有同样的支持度,经过prune)
				
				*/

				added ++;
				added += cfpt->grow(current,depth+1); //递归grow，看看添加
				delete cfpt;
			}
		}
		return added;
	}

	int FPtree::Prune()
	{
		int left=0;

		//将小于支持度的头表项删除
		for(set<Item>::iterator it = header.begin();it != header.end(); ) {
			if(it->getSupport() < minsup) {
				/*set<Item>::iterator tmp = it++;
				header.erase(tmp);*/

				//modify 
				header.erase(it++);
			}
			else {
				++left;
				++it;
			}
		}
		return left;
	}

	void FPtree::ReOrder()
	{
		set<Item>::iterator itI;
		multiset<Element>::iterator itE;
		multiset<Element> list;

		for(itI = header.begin(); itI != header.end(); itI++)
			list.insert(Element(itI->getSupport(), itI->getId()));

		len_remap = list.size()+1; //new added
		remap = new int[len_remap];
		

		if(relist!=NULL)
			delete relist;
		relist = new set<Element>;
		header.clear();
		int i=1;
		for(itE=list.begin(); itE!=list.end(); itE++) {
			if(itE->support >= minsup) {
				remap[i] = itE->id;
				relist->insert(Element(itE->id,i));
				Item a(i,0);
				itI = header.insert(a).first;
				itI->Increment(itE->support);
				i++;
			}
		}
	}

	//itemset: 项
	//il 项的个数 itemlength
	//comb 前缀
	//cl 前缀个数
	//support 支持度
	//后面的没有用过...
	void FPtree::print(int *itemset, int il, int *comb, int cl, int support, int spos, int depth, int *current)
	{
		if(current==0) {
			//if(out) {
			//	set<int> outset;
			//	for(int j=0; j<il; j++) outset.insert(remap[itemset[j]]); 
			//	for(set<int>::iterator k=outset.begin(); k!=outset.end(); k++) fprintf(out, "%d ", *k);
			//	fprintf(out, "(%d)\n", support);
			//	if(cl) {
			//		current = new int[cl];
			//		print(itemset,il,comb,cl,support,0,1,current);
			//		delete [] current;
			//	}
			//}

			
				set<int> outset;
				for(int j=0; j<il; j++) outset.insert(remap[itemset[j]]); 
				for(set<int>::iterator k=outset.begin(); k!=outset.end(); k++) printf("%d ", *k);
				printf("(%d)\n", support);
				if(cl) {
					current = new int[cl];
					print(itemset,il,comb,cl,support,0,1,current);
					delete [] current;
				}
			
			
		}
		else {
			int loper = spos;
			spos = cl;
			/*while(--spos >= loper) {
				set<int> outset;
				current[depth-1] = comb[spos];
				for(int i=0; i<depth; i++) outset.insert(remap[current[i]]); 
				for(int j=0; j<il; j++) outset.insert(remap[itemset[j]]); 
				for(set<int>::iterator k=outset.begin(); k!=outset.end(); k++) fprintf(out, "%d ", *k);
				fprintf(out, "(%d)\n", support);
				print(itemset, il, comb, cl, support, spos+1, depth+1, current);*/

			while(--spos >= loper) {
				set<int> outset;
				current[depth-1] = comb[spos];
				for(int i=0; i<depth; i++) outset.insert(remap[current[i]]); 
				for(int j=0; j<il; j++) outset.insert(remap[itemset[j]]); 
				for(set<int>::iterator k=outset.begin(); k!=outset.end(); k++) printf( "%d ", *k);
				printf("(%d)\n", support);
				print(itemset, il, comb, cl, support, spos+1, depth+1, current);
			}
		}
	}
}