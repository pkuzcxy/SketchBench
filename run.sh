# 转换数据格式
python3 utils/converter.py dat/GSA.txt test.dat
# 分析数据集特征，画出表格
python3 utils/data_analyzer.py 4 test.dat
# 编译、运行任务
g++ -std=c++11 task/freq.cpp
./a.out test.dat > res.txt
# 分析结果
python3 utils/res_analyzer.py res.txt
# 清理环境
rm a.out
rm test.dat
rm res.txt