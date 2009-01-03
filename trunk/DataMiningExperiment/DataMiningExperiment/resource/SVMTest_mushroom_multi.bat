@echo off
rem DIR为测试集所在的路径；如果在当前目录，设置为空;
set DIR= .\mushroom\

rem 格式要求为UKchess_train_10_1.dat
set TRAIN=mushroom_train
set TEST=mushroom_test
set NUM=10

rem 最小支持度
set MIN_SUP=50

rem type - 1是用harmony；其他用ddpmine
set TYPE=1

set OUTPUT_DIR=%DIR%%TYPE%\

md %OUTPUT_DIR%
rem for /L %%i in (1,1,6) DO (SVMTest_single.bat %DIR%%TRAIN% %DIR%%TEST% %NUM% %MIN_SUP% %%i %OUTPUT_DIR% %TYPE%)

rem SVMTest_single.bat %DIR%%TRAIN% %DIR%%TEST% %NUM% %MIN_SUP% 9 %OUTPUT_DIR% %TYPE%

SVMTest_single.bat %DIR%%TRAIN% %DIR%%TEST% %NUM% %MIN_SUP% 10 %OUTPUT_DIR% %TYPE%

pause

