
function check_gen() {
    gen_num=$1
    ./check-file ./t1
    ./gen-file ./t$gen_num
}

./gen-file ./t1

check_gen 2 &
pid1=$!
check_gen 3 &
pid2=$!
check_gen 4 &
pid3=$!
check_gen 5 &
pid4=$!

echo "BG pid: $pid1 $pid2 $pid3 $pid4"

wait $pid1
wait $pid2
wait $pid3
wait $pid4

sha256sum ./t2 ./t3 ./t4 ./t5

