@echo off
rem DIRΪ���Լ����ڵ�·��������ڵ�ǰĿ¼������Ϊ��;

set TRAIN=mushroom.dat
set TEST=mushroom_test.dat
set MIN_SUP=50
set NUM=10
set TYPE=1
rem 3��ʾ1���-1��2���1
@echo ����ִ���㷨...
DataMiningExperiment.exe %TRAIN% %MIN_SUP% 1 rule.txt %TYPE% 3

@echo ����ѧϰ...
svm_learn rule.txt
svm_classify %TEST% svm_model
pause
