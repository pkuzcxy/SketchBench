
g++ -std=c++11 task/test_accuracy\&throughput.cpp -O3

./a.out test1.dat 
./a.out test2.dat 
./a.out test3.dat 
./a.out test4.dat 

python3 utils/res_analyzer.py res1.txt res1.pickle
python3 utils/res_analyzer.py res2.txt res2.pickle
python3 utils/res_analyzer.py res3.txt res3.pickle
python3 utils/res_analyzer.py res4.txt res4.pickle

python3 utils/res_grapher.py res1.pickle res2.pickle res3.pickle res4.pickle
g++ -std=c++11 task/test_accuracy\&throughput.cpp -O3

./a.out test1.dat
./a.out test2.dat
./a.out test3.dat
./a.out test4.dat

