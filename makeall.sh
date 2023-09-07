rm inc_list.config
DIR=$(pwd)/core
(find $DIR \(   -path "*demos" \
                -o -path "*examples" \
                -o -path "*docs" \
                -o -path "*.github" \
                -o -path "*build" \) -prune -o \
                -type d | awk '{printf "INC += %s\n",$1}' ) >> inc_list.config
make