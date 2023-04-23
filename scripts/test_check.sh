
./gen-file ./t1

./check-file ./t1 &
pid1=$!
./check-file ./t1 &
pid2=$!
./check-file ./t1 &
pid3=$!
./check-file ./t1 &
pid4=$!

echo "BG pid: $pid1 $pid2 $pid3 $pid4"

wait $pid1
wait $pid2
wait $pid3
wait $pid4

