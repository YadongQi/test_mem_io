
./gen-file ./t1

./copy-file ./t1 ./t2 &
pid1=$!
./copy-file ./t1 ./t3 &
pid2=$!
./copy-file ./t1 ./t4 &
pid3=$!

echo "BG pid: $pid1 $pid2 $pid3"

wait $pid1
wait $pid2
wait $pid3

sleep 1

sha256sum ./t1 ./t2 ./t3 ./t4
