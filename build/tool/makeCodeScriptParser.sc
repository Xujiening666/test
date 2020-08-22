    source  ../out/sl.bin  S1
    create  ../out/sl_temp.bin     T2
    map S1[0x00000000,0x00054000] to T2[0x00000000]   //0x0000_0000 -> 0x0000_0000,0x0005_4000