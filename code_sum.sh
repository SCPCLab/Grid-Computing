#!/bin/bash
#name: code_num.sh
#

if [[ $# -ne 1 ]]; then  # not input any parameter
    #statements
    echo "usage is $0 basepath"
    exit
fi
path=$1     #parameter is file path
sum=0
declare -A statarray;   #statarray is a array

while read line; do
    #statements
    code_num=`awk '{print NR}' "$line" |tail -n1`   #-d, 表示用,作为分割符，-f1表示提取第一个属性作为ftype
    ftype=`file -b "$line" | cut -d, -f1 `
    if [[ $code_num -gt 0 ]]; then
        #statements
        let statarray["$ftype"]=statarray["$ftype"]+"$code_num"; #统计每种类型的数量
    fi
                    
done< <(find $path -type f -print )         #读取当前目录下的文件名，读到line中

echo ========= File types codes num ===========
for ftype in "${!statarray[@]}";            #返回数组的索引列表
do
    declare -i sum=statarray["$ftype"]+$sum
    echo $ftype : ${statarray["$ftype"]}
done
echo "total code num : $sum"
