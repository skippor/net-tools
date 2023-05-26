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
