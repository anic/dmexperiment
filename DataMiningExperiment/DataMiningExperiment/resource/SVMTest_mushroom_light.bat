@echo off
rem DIR为测试集所在的路径；如果在当前目录，设置为空;

set TRAIN=mushroom.dat
set TEST=mushroom_test.dat
set MIN_SUP=50
set NUM=10
set TYPE=1
rem 3表示1变成-1，2变成1
@echo 正在执行算法...
DataMiningExperiment.exe %TRAIN% %MIN_SUP% 1 rule.txt %TYPE% 3

@echo 进行学习...
svm_learn rule.txt
svm_classify %TEST% svm_model
pause
