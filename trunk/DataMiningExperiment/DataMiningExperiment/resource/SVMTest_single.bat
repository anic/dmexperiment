@echo off
rem ����Ҫ�޸�����ļ�
rem ����ڵ�ǰĿ¼������Ϊ��
rem ��ʽҪ��ΪUKchess_train_10_1.dat
set TRAIN=%1
set TEST=%2
set NUM=%3
set MIN_SUP=%4
set i=%5
set OUTPUT_DIR=%6
set TYPE=%7


rem ֻҪ�޸�TEST������ļ��������ɲ��Զ�Ӧ������
rem dataset minsup k output type label
rem dataset - ���ݼ�
rem minsup - ��С���֧�ֶ�
rem k - Ϊÿ��ʵ������ھ�Ĺ�����Ŀ
rem output - ����Ĺ����ļ�
rem type - 1����harmony��������ddpmine
rem label - 1�������ǩ; �������0

@echo ����ִ���㷨...
DataMiningExperiment.exe %TRAIN%_%NUM%_%i%.dat %MIN_SUP% 1 %OUTPUT_DIR%rule%i%.txt %TYPE% 1

@echo ����ѧϰ...
svm_multiclass_learn -c 0.01 %OUTPUT_DIR%rule%i%.txt %OUTPUT_DIR%model%i%.txt
del %OUTPUT_DIR%result_static%i%.txt
svm_multiclass_classify %TEST%_%NUM%_%i%.dat %OUTPUT_DIR%model%i%.txt %OUTPUT_DIR%result%i%.txt >> %OUTPUT_DIR%result_static%i%.txt
@echo ���з���...
type %OUTPUT_DIR%result_static%i%.txt
@echo ================================================================================
@echo �������������%OUTPUT_DIR%result_static%i%.txt �� %OUTPUT_DIR%result%i%.txt ��
@echo ================================================================================

