#!/bin/bash
#run picodb with reaffile as argument
#take instructions from instructions.txt
#and put the output to output.txt
./picodb readfile < instructions.txt > output.txt
#take number1 and number2 to check if characters were the same
#for read and fread
number1=$(grep -o '[0-9]*' stdout1.txt)
number2=$(grep -o '[0-9]*' stdout2.txt)
if [ "$number1" -eq "$number2" ]
then
	echo "Total characters read are the same!!"
else
	echo "Total characters read are not the same!!"
fi
#read output.txt line by line
#find and print total system calls made
flag=0
while read line           
do  
	if [ "$flag" -eq "1" ]
	then
		flag=2
		echo "This is read."
		echo "$line"
	elif [  "$flag" -eq "3"  ]
	then
		echo "This is fread."
		echo "$line"
		flag=4
	fi
    if [ "$line" == "This execution is over." ]
    then
		if [ "$flag" -eq "0" ]
		then
			flag=1
		else
			flag=3
		fi
	fi
done <output.txt
