@echo off
rem DIR为测试集所在的路径；如果在当前目录，设置为空;

set TRAIN=mushroom_train.dat
set TEST=mushroom_test.dat
set MIN_SUP=50
set NUM=10
set TYPE=1

@echo 正在执行算法...
DataMiningExperiment.exe %TRAIN% %MIN_SUP% 1 rule.txt %TYPE% 1

@echo 进行学习...
svm_multiclass_learn -c 0.01 rule.txt model.txt
del result_static.txt
svm_multiclass_classify %TEST% model.txt result.txt >> result_static.txt
@echo 进行分类...
type result_static.txt
@echo ================================================================================
@echo 上述结果保存在result_static.txt 和 result.txt 中
@echo ================================================================================

