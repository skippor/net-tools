git clone https://github.com/luigirizzo/netmap.git
http://info.iet.unipi.it/~luigi/netmap/

https://juejin.cn/post/7125483979792187399
https://zhuanlan.zhihu.com/p/607854526
https://abcdxyzk.github.io/blog/2020/05/21/netmap/

apt install linux-source
cd /usr/src
tar -xvjf linux-source-5.15.0.tar.bz2

./configure --drivers=e1000,veth.c --kernel-sources=/usr/src/linux-source-5.15.0
make all

# find . -name "*.ko"
./veth.ko
./e1000/e1000.ko
./netmap.ko


# rmmod veth
# insmod ./netmap.ko
# insmod ./veth.ko

# ip l a veth1 type veth peer name veth2

# ./build-apps/pkt-gen/pkt-gen -i veth1 -f tx
351.893693 main_thread [2683] 26.279 Mpps (26.308 Mpkts 12.614 Gbps in 1001078 usec) 511.51 avg_batch 99999 min_space
352.894782 main_thread [2683] 24.250 Mpps (24.276 Mpkts 11.640 Gbps in 1001090 usec) 511.51 avg_batch 99999 min_space

# ./build-apps/pkt-gen/pkt-gen -i veth2 -f rx
351.859454 main_thread [2683] 26.142 Mpps (26.170 Mpkts 12.548 Gbps in 1001068 usec) 511.51 avg_batch 1 min_space
352.860528 main_thread [2683] 24.394 Mpps (24.420 Mpkts 11.709 Gbps in 1001073 usec) 511.51 avg_batch 1 min_space


# rmmod e1000
# insmod ./netmap.ko
# insmod ./e1000/e1000.ko

# send about 500 million packets of 60 bytes each.
# wait 5s before starting, so the link can go up
pkt-gen -i eth0 -f tx -n 500111222 -l 60 -w 5
# you should see about 14.88 Mpps

# act as a receiver
pkt-gen -i eth0 -f rx 




# ./configure --drivers=veth.c,e1000 --kernel-sources=/usr/src/linux-source-5.15.0
**********************************  NOTE   **********************************
*** Running some preliminary tests to customize the build environment.
*****************************************************************************
**********************************  ERROR  **********************************
*** The '/mnt/dtt/workspace/netmap/LINUX' directory appears to contain leftovers from a previous
*** compilation. This may break compilation in the current directory.
*** Please run 'make distclean' in '/mnt/dtt/workspace/netmap/LINUX', or start from a clean
*** repository if that fails.
***
***
***     Current configuration values:
***
*** kernel directory            /lib/modules/5.15.0-58-generic/build
***                             [/usr/src/linux-headers-5.15.0-58-generic]
*** kernel sources              /usr/src/linux-source-5.15.0
*** linux version               50f4a  [5.15.74]
*** module file                 netmap.ko
*** 
*** subsystems                  null ptnetmap generic monitor pipe vale
*** apps                        dedup vale-ctl nmreplay tlem lb bridge pkt-gen
*** native drivers              e1000 veth.c
*** 
*** Contents of the drivers.mak file:
*** e1000@conf := CONFIG_E1000
*** e1000@src := cp -Rp /usr/src/linux-source-5.15.0/drivers/net/ethernet/intel/e1000 e1000
*** e1000@patch := patches/vanilla--e1000--20620--99999
*** veth.c@conf := CONFIG_VETH
*** veth.c@src := cp -Rp /usr/src/linux-source-5.15.0/drivers/net/veth.c veth.c
*** veth.c@patch := patches/vanilla--veth.c--41400--99999
*****************************************************************************