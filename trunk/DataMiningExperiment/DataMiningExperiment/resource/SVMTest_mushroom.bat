@echo off
rem DIRΪ���Լ����ڵ�·��������ڵ�ǰĿ¼������Ϊ��;

set TRAIN=mushroom_train.dat
set TEST=mushroom_test.dat
set MIN_SUP=50
set NUM=10
set TYPE=1

@echo ����ִ���㷨...
DataMiningExperiment.exe %TRAIN% %MIN_SUP% 1 rule.txt %TYPE% 1

@echo ����ѧϰ...
svm_multiclass_learn -c 0.01 rule.txt model.txt
del result_static.txt
svm_multiclass_classify %TEST% model.txt result.txt >> result_static.txt
@echo ���з���...
type result_static.txt
@echo ================================================================================
@echo �������������result_static.txt �� result.txt ��
@echo ================================================================================

