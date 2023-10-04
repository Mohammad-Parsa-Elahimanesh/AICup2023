sudo echo "get permission to start: "
PZ='./'${0:0:-13}
Kernel=$PZ"../Kernel-web-server-version"
DefaultMAP=$PZ"maps/map1.json"
MapName=`echo $DefaultMAP | rev | cut -d "/" -f 1 | rev`

O='./game'
N='./compile/main'
Client2=$PZ"../v7.7" && A1=$N # Red
Client3=$PZ"../cpp-client" && A2=$N # Green
Client1=$PZ"../v8.0" && A3=$N # Blue

LogToWinner=$PZ'LTW.py'
rm result.txt
touch result.txt

(cd $Client1 && make)
(cd $Client2 && make)
(cd $Client3 && make)
    

for _ in {1..10}
do
    python3 $Kernel/run.py -m $MapName &
    sleep 1
    (cd $Client1 && $A1) &
    sleep 0.3
    (cd $Client2 && $A2) &
    sleep 0.3
    (cd $Client3 && $A3)
    LOG=`find $Kernel/log -type f -exec ls -t1 {} + | head -1`
    LOG=${LOG%.json}
    python3 $LogToWinner <<< $LOG >> result.txt
    sleep 1
    sudo kill -9 $(ps aux | grep './compile/main' | awk '{print $2}')
	free -h && sudo sysctl -w vm.drop_caches=3 && sudo sync && echo 3 | sudo tee /proc/sys/vm/drop_caches && free -h
done

#


