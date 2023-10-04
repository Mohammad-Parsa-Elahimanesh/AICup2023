sudo echo "get permission to start: "

PZ='./'${0:0:-13}
Kernel=$PZ"../Kernel-web-server-version"
DefaultMAP=$PZ"maps/map6.json"
MapName=`echo $DefaultMAP | rev | cut -d "/" -f 1 | rev`

O='./game'
N='./compile/main'
Client1=$PZ"../v9.9" && A1=$N # Red
Client2=$PZ"../cpp-client" && A2=$N # Green
Client3=$PZ"../v9.9" && A3=$N # Blue

if [[ $# -eq 0 ]]; then
	(cd $Client1 && make)
	(cd $Client2 && make)
	(cd $Client3 && make)
	#sudo kill -9 $(sudo lsof -t -i:12345)
	python3 $Kernel/run.py -m $MapName &
	sleep 1
	(cd $Client1 && $A1) &
	sleep 1
	(cd $Client2 && $A2) &
	sleep 1
	(cd $Client3 && $A3)
fi

if [[ $# -eq 0 ]]; then
	MAP=$DefaultMAP
else
	MAP=$1	
fi

MAP=${MAP%.json}
MapToData=$PZ'MTD.py'
python3 $MapToData <<< $MAP > $MAP.txt

FlatGraphDrawer=$PZ'DPG'
FlatGraphDrawerCPP=$FlatGraphDrawer'.cpp'
g++ $FlatGraphDrawerCPP -o $FlatGraphDrawer && \
$FlatGraphDrawer < $MAP.txt > $MAP-data.txt && \
rm $FlatGraphDrawer $MAP.txt

if [[ $# -eq 2 ]]; then
	LOG=$2
else
	LOG=`find $Kernel/log -type f -exec ls -t1 {} + | head -1`
fi
LOG=${LOG%.json}

LogToData=$PZ'LTD.py' && \
python3 $LogToData <<< $LOG > $LOG.txt

if [[ $# -eq 0 ]]; then
	echo $Client1 $Client2 $Client3 >> $LOG.txt
fi
echo "Red Green Blue" >> $LOG.txt # extra line ...

input='input_data.txt'
rm $input 2> /dev/null

touch $input && \
cat $MAP-data.txt >> $input && \
cat $LOG.txt >> $input && \
rm $MAP-data.txt $LOG.txt

sudo kill -9 $(ps aux | grep './compile/main' | awk '{print $2}')
free -h && sudo sysctl -w vm.drop_caches=3 && sudo sync && echo 3 | sudo tee /proc/sys/vm/drop_caches && free -h 

javac $PZ"Visualizer/src/"*.java && \
java -cp $PZ"Visualizer/src" Main < $input && \
rm $input $PZ"Visualizer/src/"*.class && \
rm -r $PZ"Visualizer/out/" 2> /dev/null

free -h && sudo sysctl -w vm.drop_caches=3 && sudo sync && echo 3 | sudo tee /proc/sys/vm/drop_caches && free -h 

#
