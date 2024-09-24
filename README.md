# AutoConnect
长春理工大学自动连接校园网Shell脚本

使用前请自行填写账号密码，不同区的登陆地址可能不一样，自己看着改别忘记给权限

```chmod +x AutoConnect.sh```

Ubuntu CentOS等系统下添加至开机自启

```sudo vim /etc/systemd/system/AutoConnect.service ```

添加以下内容
```
[Unit]
Description=autonetworkconnect
[Service]
ExecStart=路径例如 /home/ubuntu/desktop/AutoConnect/AutoConnect.sh
[Install]  
WantedBy=multi-user.target
```

执行
```
sudo systemctl daemon-reload
sudo systemctl enable AutoConnect.service  
sudo systemctl start AutoConnect.service
```
对于Openwrt
```
vi /etc/rc.local
```
添加一行脚本路径,例如
```
XXXXXXXXXXXXXXXXXXXX
/A123/AutoConnect.sh
exit 0
```
