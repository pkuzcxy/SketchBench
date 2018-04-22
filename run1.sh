python3 utils/data_gen.py 12285667 763206 0.0 4 test1.dat
python3 utils/data_gen.py 12285667 763206 0.3 4 test2.dat
python3 utils/data_gen.py 12285667 763206 0.6 4 test3.dat
python3 utils/data_gen.py 12285667 763206 0.9 4 test4.dat

g++ -std=c++11 task/freq.cpp

./a.out test1.dat > res1.txt
./a.out test2.dat > res2.txt
./a.out test3.dat > res3.txt
./a.out test4.dat > res4.txt

python3 utils/res_analyzer.py res1.txt res1.pickle
python3 utils/res_analyzer.py res2.txt res2.pickle
python3 utils/res_analyzer.py res3.txt res3.pickle
python3 utils/res_analyzer.py res4.txt res4.pickle
