@echo off
rem "只要修改TEST后面的文件名，即可测试对应的数据"

set TEST="UKchess_test_10_1.dat"
@echo =====================学习结果=====================
svm_multiclass_learn -c 0.01 rule.txt model.txt
del result_static.txt

svm_multiclass_classify %TEST% model.txt result.txt >> result_static.txt
@echo =====================分类结果=====================
type result_static.txt
@echo ==================================================
@echo 上述结果保存在result_static.txt 和 result.txt==
@echo ==================================================
pause