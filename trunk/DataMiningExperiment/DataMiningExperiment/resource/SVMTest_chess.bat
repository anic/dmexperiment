@echo off
rem DIRΪ���Լ����ڵ�·��������ڵ�ǰĿ¼������Ϊ��;
set DIR= .\chess\

rem ��ʽҪ��ΪUKchess_train_10_1.dat
set TRAIN=UKchess_train
set TEST=UKchess_test
set NUM=10

rem ��С֧�ֶ�
set MIN_SUP=50

rem type - 1����harmony��������ddpmine
set TYPE=1

set OUTPUT_DIR=%DIR%%TYPE%\

md %OUTPUT_DIR%
for /L %%i in (1,1,%NUM%) DO (SVMTest_single.bat %DIR%%TRAIN% %DIR%%TEST% %NUM% %MIN_SUP% %%i %OUTPUT_DIR% %TYPE%)
pause

