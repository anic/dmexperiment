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
		//����һ�������񡰵�ͷ���С�

		set<Item>::iterator head;
		int added=0;

		for(int depth=0; depth < t->length; depth++) {
			head = header.find(Item(t->t[depth], 0));
			if(head == header.end()) { //û���ҵ������һ��
				head = header.insert(Item(t->t[depth], 0)).first; //��ͷ���м���
				added++;
			}
			head->Increment(times); //������ͷ���е�֧�ֶȣ��ҵ���û���ҵ����ǣ�
		}
		return added;
	}

	int FPtree::processTransaction(Transaction *t, int times)
	{
		//����һ��������

		set<Item>::iterator it, head;
		set<Item>* items = root;
		Item_ *current = 0;
		int added=0;

		for(int depth=0; depth < t->length; depth++) {
			head = header.find(Item(t->t[depth], 0));		//��ͷ������
			if(head != header.end()) {						//����У�Ҳ�������û�б�minsupɾ��prune��
				it = items->find(Item(t->t[depth], 0));		//��ʼ��root��ʼ�ҵ�һ����

				if(it == items->end()) {								//û���ҵ���
					it = items->insert(Item(t->t[depth], current)).first;	//���һ���µ�
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
			int *comb = new int[header.size()];		//��ӵ�ǰ׺
			int cl=0;
			for(set<Item>::iterator it=header.begin(); it != header.end(); ++it) {
				current[depth-1] = it->getId();			//��Ϊ��֦���������һ�������
				/*
				current = 1,5,6
				header = {4,7,8}
				current = 1,5,6,4;
				current = 1,5,6,7
				current = 1,5,6,8 //��Ϊ��ǰ������Ѿ��ǵ�֦��
				*/

				print(current,depth,comb,cl,it->getSupport());	//�����ǰ�Ľ�����ļ���˵���ҵ�һ������Ĺ���
				comb[cl++] = it->getId();
				added += factor;
				factor *= 2;
			}
			delete [] comb;
		}
		else { //not singlepath ���ǵ�֦
			for(set<Item>::iterator it=header.begin(); it != header.end(); ++it) {
				Item_ *i;
				current[depth-1] = it->getId(); 

				//����ͷ����ÿһ��Ԫ�أ�����һ���µ�FPTree
				FPtree *cfpt = new FPtree();
				
				//������С֧�ֶȺ����
				cfpt->setMinsup(minsup); 
				cfpt->setOutput(out);

				int *tmp = new int[header.size()];
				for(i = it->getNext(); i; i = i->nodelink) {
					int l=0;
					for(Item_ *p=i->parent; p; p = p->parent) tmp[l++] = p->id; //��������

					//�������ĸ���
					Transaction *t = new Transaction(l);
					for(int j=0; j<l; j++) t->t[j] = tmp[l-j-1]; //������ֵ

					cfpt->processItems(t,i->supp); //����������µ�tree��
					delete t;
				}
				cfpt->Prune(); //��֦

				for(i = it->getNext(); i; i = i->nodelink) {
					int l=0;
					for(Item_ *p=i->parent; p; p = p->parent) tmp[l++] = p->id;
					Transaction *t = new Transaction(l);
					for(int j=0; j<l; j++) t->t[j] = tmp[l-j-1];
					cfpt->processTransaction(t,i->supp);
					delete t;
				}
				delete [] tmp;
				print(current,depth,0,0,it->getSupport()); //�����ǰ���

				/*
				current = 1,5,6
				header = {4,7,10,8,9}, 
				4___7_8
				  |_10_8_9

			    1,5,6,4
				1,5,6,8,4,7,10(��Ϊ��ͼ��,�õ�һ��8����4,7Ҳ��ͬ����֧�ֶ�,����prune)
				
				*/

				added ++;
				added += cfpt->grow(current,depth+1); //�ݹ�grow���������
				delete cfpt;
			}
		}
		return added;
	}

	int FPtree::Prune()
	{
		int left=0;

		//��С��֧�ֶȵ�ͷ����ɾ��
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

	//itemset: ��
	//il ��ĸ��� itemlength
	//comb ǰ׺
	//cl ǰ׺����
	//support ֧�ֶ�
	//�����û���ù�...
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