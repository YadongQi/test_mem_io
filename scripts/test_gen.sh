
./gen-file ./t1 &
pid1=$!
./gen-file ./t2 &
pid2=$!
./gen-file ./t3 &
pid3=$!
./gen-file ./t4 &
pid4=$!

echo "BG pid: $pid1 $pid2 $pid3 $pid4"

wait $pid1
wait $pid2
wait $pid3
wait $pid4

sleep 1

sha256sum ./t1 ./t2 ./t3 ./t4
