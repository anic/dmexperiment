@echo off
rem DIRΪ���Լ����ڵ�·��������ڵ�ǰĿ¼������Ϊ��;
set DIR= .\mushroom\

rem ��ʽҪ��ΪUKchess_train_10_1.dat
set TRAIN=mushroom_train
set TEST=mushroom_test
set NUM=10

rem ��С֧�ֶ�
set MIN_SUP=50

rem type - 1����harmony��������ddpmine
set TYPE=1

set OUTPUT_DIR=%DIR%%TYPE%\

md %OUTPUT_DIR%
rem for /L %%i in (1,1,6) DO (SVMTest_single.bat %DIR%%TRAIN% %DIR%%TEST% %NUM% %MIN_SUP% %%i %OUTPUT_DIR% %TYPE%)

rem SVMTest_single.bat %DIR%%TRAIN% %DIR%%TEST% %NUM% %MIN_SUP% 9 %OUTPUT_DIR% %TYPE%

SVMTest_single.bat %DIR%%TRAIN% %DIR%%TEST% %NUM% %MIN_SUP% 10 %OUTPUT_DIR% %TYPE%

pause

