rm inc_list.config
DIR=$(pwd)/core
echo $DIR
#使用绝对路径时,find也会输出绝对路径
(find $DIR -type d | awk '{printf "INC += %s\n",$1}' ) >> inc_list.config
make -j6