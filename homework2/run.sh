#! /bin/bash
root=`dirname $0`
cd $root
if [ ! -d ./out ]
then mkdir ./out
fi

cd $root/texts
filelist=`ls`
cd ..

if [ ${1}x = testx ]
then echo "<错误> 请不要用test命名你的源代码"
exit
fi

if [ -f ./$1.cpp ]
	then g++ ./$1.cpp -o ./$1
	else 
		if [ -f ./$1.c ]
		then g++ ./$1.c -o ./$1
		else echo "<错误> 没有找到可以编译的文件"
		exit
		fi
fi

if [ $? != 0 ]
then echo "<错误> 编译错误"
exit
fi
chmod +x ./test
for file in $filelist
do
	echo "==> 测试文件：$file"
	(time cat ./dictionary.txt ./texts/$file | ./test 1> ./keys/$file) >& ./time1.tmp
	(time cat ./dictionary.txt ./texts/$file | ./$1 1> ./out/$file) >& ./time2.tmp
	diff ./out/$file ./keys/$file
	temp1=(`md5sum ./out/$file`)
	temp2=(`md5sum ./keys/$file`)
	if [ ${temp1[0]} != ${temp2[0]} ]
	then 
		echo "<错误> 结果不匹配"
		exit
	else 
		echo "结果匹配"
		time1=(`cat ./time1.tmp`)
		time2=(`cat ./time2.tmp`)
		echo "例程运行时间  ${time1[3]}; 你的运行时间 ${time2[3]}"
		echo
	fi
done
rm ./time1.tmp ./time2.tmp
