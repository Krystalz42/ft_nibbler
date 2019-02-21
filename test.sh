#!/bin/bash
export id=()
index=0
pid=()
returnSucessProcess=0

pidOfScript="$(echo ${$})"

function trap_with_arg() {
    func="$1" ; shift
    for sig ; do
        trap "${func} ${sig}" "$sig"
    done
}
function func_trap() {
    echo Trapped: $1
    index=0
    if [[ $1 = SIGUSR1 ]]
        then
            while test ${index} != ${numOfClient}
            do
                kill -n 30 ${pid[$index]}
                echo "kill -n 30 ${pid[$index]} $index"
                index=$(($index + 1))
            done
    elif [[ $1 = SIGUSR2 ]]
        then
            returnSucessProcess=$((returnSucessProcess + 1))
    fi
}
function in_id {
  for e in ${id[*]}
  do
    if [[ "$e" == "$1" ]]
    then
        echo "$e == $1"
      return 1
    fi
  done
  return 0
}

function func_trap_SIGUSR1() {
    echo "Trapped SIGUSR1"
    i=0
    while test ${i} != ${numOfClient}
    do
        kill -n 30 ${pid[$i]}
        echo "kill -n 30 ${pid[$i]} $i"
        i=$(($i + 1))
    done
}
function func_trap_SIGUSR2() {
    echo "Trapped SIGUSR2"    
    returnSucessProcess=$((returnSucessProcess + 1))
}

trap func_trap_SIGUSR1 SIGUSR1
trap func_trap_SIGUSR2 SIGUSR2

#trap "func_trap SIGUSR2" SIGUSR2
#trap "func_trap SIGUSR1" SIGUSR1
#trap_with_arg func_trap SIGUSR1

fileLog="$(basename ${1})/$(basename ${1})"

echo "pidOfScript : ${pidOfScript}"
echo "fileToTest: ${1}"
echo "fileLog: ${fileLog}"


echo "Count all id ..."
while read ligne ; do
    checkId="$(echo ${ligne} | cut -d' ' -f1)"
    if in_id ${checkId}
    then
        id+=("$(echo ${checkId})")
    fi
done < "tests/${fileLog}.ut"
echo "Where has $numOfClient Id's"
echo
echo

numOfClient=${#id[@]}

echo "Run nibbler ..."
while test ${index} != ${numOfClient}
    do
    ${4} -t --id ${id[$index]} --fileInput ${2} --fileLog ${2} --pidTestProcess ${pidOfScript} &> "logTests/${2}${id[$index]}.output" & pid[$index]=$! > /tmp/log_pid.out
    echo ${id[$index]}
    index=$(($index + 1))
done
echo
echo

sleep 2

echo "Start test ..."
index=0
while test ${index} != ${numOfClient}
    do
    kill -n 30 ${pid[$index]}
    echo "kill -n 30 ${pid[$index]}"
    index=$(($index + 1))
done
echo
echo

echo "Print Pid of all process :"
index=0
while test ${index} != ${numOfClient}
    do
    echo "${pid[$index]}"
    index=$(($index + 1))
done
echo
echo
#while read line ; do
#    if [ $line = "kill" ]
#        then
#            for p in ${pid[*]}; do
#                kill $p > /dev/null;
#            done
#        exit
#    fi
#    returnSucessProcess
#done


echo "Waiting process exit ..."
for p in ${pid[*]}; do
    while kill -0 ${p} > /dev/null ; do
    sleep 0.1
    done
done

if [ $returnSucessProcess != $numOfClient ]
    then
        echo "EXIT_FAILURE $returnSucessProcess $numOfClient"
        exit 1
fi

index=0
while test ${index} != ${numOfClient}
    do

    diff="$(diff tests/${fileLog}${id[$index]}.log logTests/${2}${id[$index]}.log)"

    echo "diff tests/${fileLog}${id[$index]}.log logTests/${2}${id[$index]}.log"
    if [ "$diff" != "" ]
        then
            echo "EXIT_DIFF $returnSucessProcess $numOfClient"
            exit 2
    fi

    index=$(($index + 1))
done
echo "EXIT_SUCESS $returnSucessProcess $numOfClient"
exit 0