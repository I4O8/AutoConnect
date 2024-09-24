#!/bin/bash

#填入帐号密码
USERNAME=""
PASSWORD=""

#不同校区登陆ip可能不一样，这里只试了南区
LOGIN_IP="172.16.30.98"

IP_ADDRESS=$(ifconfig | grep -Eo 'inet (addr:)?([0-9]{1,3}\.){3}[0-9]{1,3}' | grep -v '127.0.0.1' | awk '{print $2; exit}' | sed 's/addr://')

URL1="http://${LOGIN_IP}:801/eportal/?c=ACSetting&a=Login&protocol=http:&hostname=${LOGIN_IP}&iTermType=1&wlanuserip=${IP_ADDRESS}&wlanacip=null&wlanacname=null&mac=00-00-00-00-00-00&ip=${IP_ADDRESS}&enAdvert=0&queryACIP=0&jsVersion=2.4.3&loginMethod=1"
GET="DDDDD=${USERNAME}&upass=${PASSWORD}&R1=0&R2=0&R3=0&R6=0&para=00&0MKKey=123456&buttonClicked=&redirect_url=&err_flag=&username=&password=&user=&cmd=&Login=&v6ip="

LOG=$(curl -X POST -i -L -s -d "$GET" $URL1)
URL2=$(echo "$LOG" | grep -i "^Location:" | awk '{print $2}')
ACIP="null"
ACNAME="null"
if [[ $URL2 =~ wlanacip=([0-9.]+) ]]; then
	ACIP="${BASH_REMATCH[1]}"
fi

if [[ $URL2 =~ wlanacname=([A-Za-z0-9-]+) ]]; then
	ACNAME="${BASH_REMATCH[1]}"
	L="${ACNAME: -1}"
	
	if [ "$L" == "A" ]; then
		ACNAME="${ACNAME::-1}B"
	elif [ "$L" == "B" ]; then
		ACNAME="${ACNAME::-1}A"
	fi
fi
URL3="http://${LOGIN_IP}:801/eportal/?c=ACSetting&a=Login&protocol=http%3A&hostname=${LOGIN_IP}&iTermType=1&wlanuserip=${IP_ADDRESS}&wlanacip=${ACIP}&wlanacname=${ACNAME}&mac=00-00-00-00-00-00&ip=${IP_ADDRESS}&enAdvert=0&queryACIP=0&jsVersion=2.4.3&loginMethod=1"
sleep 2
curl -X POST -d "$GET" $URL3
