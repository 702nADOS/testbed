#
xfce4-terminal -x sudo minicom
#Start QEMUSAVM
xfce4-terminal --default-working-directory=/home/ga68cap/lil4/qemusavm-build -x make run/sa_drv
#Start BeagleBoneBlack
xfce4-terminal -x ssh root@10.0.1.209  /home/root/poluluServer/servocontroller
#Start Speeddreams
xfce4-terminal -x /home/ga68cap/speed-dreams/build/games/speed-dreams-2 

