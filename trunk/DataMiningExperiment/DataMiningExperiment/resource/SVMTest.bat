@echo off
rem "ֻҪ�޸�TEST������ļ��������ɲ��Զ�Ӧ������"

set TEST="UKchess_test_10_1.dat"
@echo =====================ѧϰ���=====================
svm_multiclass_learn -c 0.01 rule.txt model.txt
del result_static.txt

svm_multiclass_classify %TEST% model.txt result.txt >> result_static.txt
@echo =====================������=====================
type result_static.txt
@echo ==================================================
@echo �������������result_static.txt �� result.txt==
@echo ==================================================
pause