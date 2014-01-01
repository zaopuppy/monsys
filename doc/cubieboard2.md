
Nand分区调整
=================================================
刚刚刷好Nand的分区一般是这样的:

    linaro@zcubieboard2:/dev$ df -h
    Filesystem      Size  Used Avail Use% Mounted on
    /dev/root       2.0G 1010M  916M  53% /
    devtmpfs        405M  4.0K  405M   1% /dev
    none            4.0K     0  4.0K   0% /sys/fs/cgroup
    none             82M  192K   81M   1% /run
    none            5.0M     0  5.0M   0% /run/lock
    none            406M     0  406M   0% /run/shm
    none            100M     0  100M   0% /run/user
    linaro@zcubieboard2:/dev$ sudo nand-part -f a20 /dev/nand
    check partition table copy 0: mbr: version 0x00000200, magic softw411
    OK
    check partition table copy 1: mbr: version 0x00000200, magic softw411
    BAD!
    check partition table copy 2: mbr: version 0x00000200, magic softw411
    BAD!
    check partition table copy 3: mbr: version 0x00000200, magic softw411
    BAD!
    mbr: version 0x00000200, magic softw411
    3 partitions
    partition  1: class =         DISK, name =   bootloader, partition start =    32768, partition size =   131072 user_type=0
    partition  2: class =         DISK, name =       rootfs, partition start =   163840, partition size =  4194304 user_type=0
    partition  3: class =         DISK, name =        UDISK, partition start =  4358144, partition size =  3375104 user_type=0
    linaro@zcubieboard2:/dev$ sudo fdisk -l

    Disk /dev/nand: 4085 MB, 4085252096 bytes
    255 heads, 63 sectors/track, 496 cylinders, total 7979008 sectors
    Units = sectors of 1 * 512 = 512 bytes
    Sector size (logical/physical): 512 bytes / 512 bytes
    I/O size (minimum/optimal): 512 bytes / 512 bytes
    Disk identifier: 0x00000000

    Disk /dev/nand doesn't contain a valid partition table

    Disk /dev/nanda: 67 MB, 67108864 bytes
    255 heads, 63 sectors/track, 8 cylinders, total 131072 sectors
    Units = sectors of 1 * 512 = 512 bytes
    Sector size (logical/physical): 512 bytes / 512 bytes
    I/O size (minimum/optimal): 512 bytes / 512 bytes
    Disk identifier: 0x00000000

         Device Boot      Start         End      Blocks   Id  System

    Disk /dev/nandb: 2147 MB, 2147483648 bytes
    255 heads, 63 sectors/track, 261 cylinders, total 4194304 sectors
    Units = sectors of 1 * 512 = 512 bytes
    Sector size (logical/physical): 512 bytes / 512 bytes
    I/O size (minimum/optimal): 512 bytes / 512 bytes
    Disk identifier: 0x00000000

    Disk /dev/nandb doesn't contain a valid partition table

    Disk /dev/nandc: 1853 MB, 1853882368 bytes
    255 heads, 63 sectors/track, 225 cylinders, total 3620864 sectors
    Units = sectors of 1 * 512 = 512 bytes
    Sector size (logical/physical): 512 bytes / 512 bytes
    I/O size (minimum/optimal): 512 bytes / 512 bytes
    Disk identifier: 0xffffffff

    Disk /dev/nandc doesn't contain a valid partition table

可以看到, nand上已经有三个分区了, 分别是: bootloader(32768, 131072), rootfs(163840, 4194304), UDISK(4358144, 3375104)

由于系统已经装好了, 分区是不能随便调整了, 不过基本可以判断实际在使用的只有bootloader和rootfs, UDISK是没有用的, 可以干掉, 全部给rootfs

nand的分区格式和普通的不一样, 不能使用fdisk, 需要使用nand-part

    linaro@zcubieboard2:/dev$ nand-part 
    usage: nand-part [-f a10|a20] nand-device
           nand-part nand-device 'name2 len2 [usertype2]' ['name3 len3 [usertype3]'] ...
           nand-part [-f a10|a20] nand-device start1 'name1 len1 [usertype1]' ['name2 len2 [usertype2]'] ...

直接跟nand设备文件, 将打印出当前的分区情况(见前面的输出)
直接跟'name len [usertyp]'将在指定分区的起始块调整分区
如果指定分区之前增加一个参数start, 将从start开始重新划分分区

好吧, 虽然看起来很灵活, 实际上发现各种坑爹:

    linaro@zcubieboard2:~$ sudo nand-part -f a20 /dev/nand 'rootfs 7569408'
    if using -f, must set info for first partition
    linaro@zcubieboard2:~$ sudo nand-part /dev/nand 'rootfs 7569408'
    ...
    Partition 1 starting offset must be at least 128

只能采用重划所有分区的方式了:

    sudo nand-part -f a20 /dev/nand 32768 'bootloader 131072' 'rootfs 7569408'

注意执行命令必须有root权限

顺便说一句, 这里只是调整分区结构, 并不会重新擦掉原来的数据, 所以, 只要起始块不变, 且调整后的分区比调整前的分区大, 数据是不会丢失的(https://groups.google.com/forum/#!topic/cubieboard/VYSSavnhxvE)

重启, 然后调整文件系统分区大小

    linaro@zcubieboard2:~$ sudo resize2fs /dev/nandb
    resize2fs 1.42.5 (29-Jul-2012)
    Filesystem at /dev/nandb is mounted on /; on-line resizing required
    old_desc_blocks = 1, new_desc_blocks = 1
    The filesystem on /dev/nandb is now 976896 blocks long.

    linaro@zcubieboard2:~$ df -h
    Filesystem      Size  Used Avail Use% Mounted on
    /dev/root       3.7G 1011M  2.6G  29% /
    devtmpfs        405M  4.0K  405M   1% /dev
    none            4.0K     0  4.0K   0% /sys/fs/cgroup
    none             82M  184K   81M   1% /run
    none            5.0M     0  5.0M   0% /run/lock
    none            406M     0  406M   0% /run/shm
    none            100M     0  100M   0% /run/user

开搞!

wrote at 2013-11-19 22:27




