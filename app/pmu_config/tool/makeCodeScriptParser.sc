    source  ../out/pmu_config.bin  S1
    create  ../out/pmu_config_temp.bin     T2
    map S1[0x00000000,0x000100] to T2[0x00000000]   //0x0000_0000 -> 0x0000_0000,0x0005_8000