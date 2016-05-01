#!/bin/sh

PORT=10999

for OPT in $*
do
    case $OPT in
        '-p' )
            FLAG_PORT=1
            PORT=$2
            shift 2
            ;;
    esac
    shift $(( $# > 0 ? 1 : 0 ))
done

while true;
do
    TEMPERATURE=` od -A n -t u4 -N 1 /dev/urandom | sed 's/[^0-9]//g'`
    HUMANITY=` od -A n -t u4 -N 1 /dev/urandom | sed 's/[^0-9]//g'`
    echo "{ \"temperature\": $TEMPERATURE, \"humanity\": $HUMANITY }" | nc -u -w 3 localhost $PORT
done

