@echo off
rem 不需要修改这个文件
rem 如果在当前目录，设置为空
rem 格式要求为UKchess_train_10_1.dat
set TRAIN=%1
set TEST=%2
set NUM=%3
set MIN_SUP=%4
set i=%5
set OUTPUT_DIR=%6
set TYPE=%7


rem 只要修改TEST后面的文件名，即可测试对应的数据
rem dataset minsup k output type label
rem dataset - 数据集
rem minsup - 最小相对支持度
rem k - 为每个实例最多挖掘的规则数目
rem output - 输出的规则文件
rem type - 1是用harmony；其他用ddpmine
rem label - 1是输出标签; 否则输出0

@echo 正在执行算法...
DataMiningExperiment.exe %TRAIN%_%NUM%_%i%.dat %MIN_SUP% 1 %OUTPUT_DIR%rule%i%.txt %TYPE% 1

@echo 进行学习...
svm_multiclass_learn -c 0.01 %OUTPUT_DIR%rule%i%.txt %OUTPUT_DIR%model%i%.txt
del %OUTPUT_DIR%result_static%i%.txt
svm_multiclass_classify %TEST%_%NUM%_%i%.dat %OUTPUT_DIR%model%i%.txt %OUTPUT_DIR%result%i%.txt >> %OUTPUT_DIR%result_static%i%.txt
@echo 进行分类...
type %OUTPUT_DIR%result_static%i%.txt
@echo ================================================================================
@echo 上述结果保存在%OUTPUT_DIR%result_static%i%.txt 和 %OUTPUT_DIR%result%i%.txt 中
@echo ================================================================================

