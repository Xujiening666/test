/*
*****************************************************************************
**

**  File        : LinkerScript.ld
**
**  Abstract    : Linker script for SLXXXX Device with
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : Smartlink Technology SLXXXX
**
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
**  (c)Copyright mName.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted.
**
*****************************************************************************
*/
/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   _estack
 */
/* Entry Point */
ENTRY(Reset_Handler)


/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x0000;    /* required amount of heap  */
_Min_Stack_Size = 0x0000;   /* required amount of stack */

BASE_LADDR        = 0x2000;

VECTOR_LADDR      = 0x00000000;
VECTOR_ADDR       = 0x00800000; /* keep it */
VECTOR_SIZE       = 0x100;

COM_BT_LADDR      = VECTOR_SIZE;
COM_BT_ADDR       = 0x00804000; /* keep it */
COM_BT_SIZE       = 16K;

COM_SYS_LADDR     = COM_BT_LADDR + COM_BT_SIZE;
COM_SYS_ADDR      = COM_BT_ADDR + COM_BT_SIZE;
COM_SYS_SIZE      = 41K;

APP_DATA_LADDR    = COM_SYS_LADDR + COM_SYS_SIZE;
APP_DATA_ADDR     = COM_SYS_ADDR + COM_SYS_SIZE;
APP_DATA_SIZE     = 2K;

BSS_ADDR          = APP_DATA_ADDR + APP_DATA_SIZE;
BSS_SIZE          = 115K;

STACK_ADDR        = BSS_ADDR + BSS_SIZE;
STACK_SIZE        = 2K;

SRAM1_LADDR       = APP_DATA_LADDR + APP_DATA_SIZE;
SRAM1_ADDR        = STACK_ADDR + STACK_SIZE;
SRAM1_SIZE        = 0x00836000 - SRAM1_ADDR;

SRAM2_LADDR       = SRAM1_LADDR + SRAM1_SIZE;
SRAM2_ADDR        = 0x40036100;
SRAM2_SIZE        = 0x1EFC; /* ~8K */

/* bank must be 8k align */
BANK_ALIGN        = 0x2000;
BANK_LADDR        = (( SRAM2_LADDR + SRAM2_SIZE + BANK_ALIGN ) / BANK_ALIGN) * BANK_ALIGN;
BANK_ADDR         =  0xC00000 + BANK_LADDR + BASE_LADDR;
BANK_SIZE         = 280K;

TONE_SIZE         = 108K;
PARAM_ADDR        = BANK_LADDR + BANK_SIZE + TONE_SIZE + BASE_LADDR;

/* Specify the memory areas */
MEMORY
{
    VECTOR(rxw):        ORIGIN = VECTOR_ADDR,    LENGTH = VECTOR_SIZE
    COM_BT(rxw):        ORIGIN = COM_BT_ADDR,    LENGTH = COM_BT_SIZE
    COM_SYS(rxw):       ORIGIN = COM_SYS_ADDR,   LENGTH = COM_SYS_SIZE
    DATA_SYS(rxw):      ORIGIN = APP_DATA_ADDR,  LENGTH = APP_DATA_SIZE
    BSS_SYS(rxw):       ORIGIN = BSS_ADDR,       LENGTH = BSS_SIZE
    STACK(rxw):         ORIGIN = STACK_ADDR,     LENGTH = STACK_SIZE
    SRAM1(rxw):         ORIGIN = SRAM1_ADDR,     LENGTH = SRAM1_SIZE
    SRAM2(rxw):         ORIGIN = SRAM2_ADDR,     LENGTH = SRAM2_SIZE
    BANK(rx):           ORIGIN = BANK_ADDR,      LENGTH = BANK_SIZE
}

/* Define output sections */
SECTIONS
{

    .vector_sec : AT( "VECTOR_LADDR" )
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
    } >VECTOR


    .com_bt_sec : AT( "COM_BT_LADDR" )
    {
        *(.bth_common_ram_seg)
        *(.bth_connect_seg)
        *(.bth_basic_seg)
        *(.btc_common_ram_seg)
        *(.bt_lcsco_seg)
        *(.bt_ld_seg)
        *(.bt_com_seg)
        *(.btc_uart_seg)
        *(.blec_ea_seg)
        *(.blec_lld_seg)
        *(.blec_lld_util_seg)
        *(.blec_lld_evt_seg)
        *(.blec_lld_data_seg)
        *(.blec_llc_util_datatrans_seg)
        *(.blec_common_ram_seg)
        *(.btc_hciacl_seg)
    } >COM_BT


    .com_sys_sec : AT( "COM_SYS_LADDR" )
    {
        . = ALIGN(4);
        KEEP(*(.text.Reset_Handler))
        KEEP(*(.text.Exception_Handler))
        *(.startup_seg)
        *(.main_seg)
        *(.eid_seg)
        *(.sys_com_seg)
        *(.isr_seg)
        *(.r_twi_seg)
        *(.gpio_seg)
        *(.gpio_rodata_seg)
        *hal_gpio.o(.rodata.CSWTCH*)
        *hal_gpio_lib.o(.rodata.CSWTCH*)
        *(.event_seg)
        *(.timer0_seg)
        *(.com_seg)
        *(.com_rodata_seg)
        *(.key_seg)
        *(.key_rodata_seg)
        *(.kadc_seg)
        *(.irrx_seg)
        *(.irrx_rodata_seg)
        *(.disp_seg)
        *(.disp_rodata_seg)
        *(.ledc_seg)
        *(.ledc_rodata_seg)

        *(.audio_effects_seg)

        *(.audiorecorder_sram_seg)
        *(.bitstream_store_sram_seg)
        *(.audioplayer_sram_seg)
        *(.audiotrack_sram_seg)
        *(.karaoke_sram_seg)
        *(.hwasrc_sram_seg)
        *(.fade_sram_seg)
        *(.fade_rodata_sram_seg)
        *(.audio_mix_sram_seg)
        *(.audio_clksync_sram_seg)
        *(.fatfs_stream_sram_seg)
        *(.tonefs_stream_sram_seg)
        *(.volume_control_sram_seg)
        *(.tone_mixer_sram_seg)
        *(.tone_mixer_rodata_sram_seg)
        *(.sbc_auxtrack_sram_seg)
        *(.sbc_auxtrack_rodata_sram_seg)
        *(.mp3_auxtrack_sram_seg)
        *(.auxtrack_mixer_sram_seg)
        *(.auxtrack_mixer_rodata_sram_seg)
        *(.aout_sram_seg)
        *(.dac_aout_sram_seg)
        *(.karaoke_aout_sram_seg)
        *(.adc_ain_sram_seg)
        *(.karaoke_ain_sram_seg)
        *(.recorder_ain_sram_seg)

        *(.info_seg)
        *(.dual_cpu_seg)
        *(.usb_seg)
        *(.mode_com_seg)
        *(.sleep_sram_seg)
        *(.sleep1_sram_seg)
        *(.btc_sleep_sram_seg)
        *(.btc_sleep1_sram_seg)
        *(.pmu_sram_seg)
        *(.ccu_seg)
        *(.timer_seg)
        *(.i2s_sram_seg)
        *(.auss_sram_seg)
        *(.pcm_sram_seg)
        *(.ring_buffer_sram_seg)
        *(.spi_seg)
        *(.pwm_seg)
        *(.twi_seg)
        *(.uart_seg)
        *(.update_seg)
        *(.rtc_sram_seg)

        *sp_data.o(.text*)
        *sp_data.o(.rodata*)
        *sp_fft.o(.text*)
        *sp_fft.o(.rodata*)
        *sp_math.o(.text*)
        *sp_math.o(.rodata*)
        *spcommon.o(.text*)
        *spcommon.o(.rodata*)
        *hal_power_lib.o(.rodata*)
        *bt_host_hal_uart.o(.rodata*)
        *pmu.o(.rodata*)
        *(.pmu_rodata_seg)
        *sltip.o(.rodata*)
        *hal_clk_lib.o(.rodata*)
        *hal_clk_lib.o(.rodata*)

        *uart.o(.text*)
        *uart.o(.rodata*)

        *(.device_sram_seg)
        *(.btc_access_ram_seg)
        *(.bth_uart_seg)
        *bt_host_rfcomm.o(.text*)

        *(.sd_seg)
        *(.sd_isr_seg)

        *(.xfs_sram_seg)
        *(.xfs_diskio_sram_seg)

        *(.usbaudio_sram_seg)
        *(.lineinaudio_sram_seg)
        *(.btaudio_sram_seg)
        *(.sco_audio_sram_seg)
        *(.a2dp_play_sram_seg)

        *(.nhs_rodata_seg)
        *(.fs_data_seg)
        *(.freertos_sram_seg)

        *(.sys_sram_seg)
        *(.app_sram_seg)
        *(.fcc_sram_seg)
    } >COM_SYS


    .app_data_sec : AT( "APP_DATA_LADDR" )
    {
        . = ALIGN(4);
        _sdata = .;        /* create a global symbol at data start */
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */

        . = ALIGN(4);
        _edata = .;        /* define a global symbol at data end */

        /* used by the startup to initialize data */
        _sidata = _sdata;
    } >DATA_SYS


    .stack_sec :
    {
        . = ALIGN(4);
        *(.app_stack)
         . = ALIGN(4);
        __app_stack_end  = .;
    } >STACK


    .app_bss_start :
    {
        . = ALIGN(4);
        _bss = .;
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start__ = _sbss;
    } >BSS_SYS


    /* Uninitialized data section */
    . = ALIGN(4);
    .app_bss_sec :
    {
        /* This is used by the startup in order to initialize the .bss secion */
        *(.driver_bss)
        *(.bss)
        *(.bss*)
    } >BSS_SYS


    .user_heap :
    {
        . = ALIGN(4);
        _heap = .;
        *(.os_heap)
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        _eheap = .;
        . = ALIGN(4);
    } >BSS_SYS


    .app_bss_end :
    {
        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
        __bss_end__ = _ebss;
    } >BSS_SYS

    .audio_heap :
    {
        . = ALIGN(4);
        *(.audio_heap)
        . = ALIGN(4);
    } >BSS_SYS

    .sram1_sec "SRAM1_ADDR" : AT( "SRAM1_LADDR" )
    {
        *(.auss_anc_sram_seg)
        *reservoir.o(.text*)
        *encoder.o(.text*)
        *quantize.o(.text*)
        *bitstream.o(.rodata*)
        *encoder.o(.rodata*)
        //*lame.o(.rodata*)
        *newmdct.o(.rodata*)
        //*presets.o(.rodata*)
        *quantize.o(.rodata*)
        *quantize_pvt.o(.rodata*)
        *reservoir.o(.rodata*)
        *takehiro.o(.rodata*)
        //*util.o(.rodata*)

        *(.bt_lctask_reg)
        *(.btaudio_seg)

        *(.fdnrev_seg)
        *(.formant_seg)
        *(.spec_sram_seg)
        *(.postproc_tsps_sram_seg)
        *(.tsps_proc_seg)
        *(.fc_data_seg)
        *(.fc_proc_seg)
        *(.autotune_proc_seg)
        *(.sdpro_seg)
        *(.sdpro_rodata_seg)
        *(.spec_rodata_seg)
        *(.decorr_rodata_seg)
        *(.decorr_proc_seg)
        *(.fs_proc_seg)
        *(.nhs_seg)
        *(.ducker_proc_seg)
        *(.echo_proc_seg)
        *(.dump_seg)
        *(.cache_seg)
    } >SRAM1

    .sram2_sec "SRAM2_ADDR" : AT( "SRAM2_LADDR" )
    {
        . = ALIGN(4);
        *(.btc_hci_rodata_seg)
        *(.btc_hcitask_rodata_seg)
        *(.btc_hcieif_rodata_seg)
        *(.bt_hcirodata_seg)
        *(.bt_rodata_seg)

        *(.blec_rodata_seg)
        *llc_llcp.o(.rodata*)
        *llm_task.o(.rodata*)
        *llm.o(.rodata*)
        *llc_task.o(.rodata*)

        *hci_rom.o(.rodata*)
        *hci_uart.o(.rodata*)

        *ld_init.o(.rodata*)
        *bt_host_reconnect.o(.rodata*)
        *sltbt.o(.rodata*)

        *bt_host_avdtp.o(.rodata*)
        *bt_host_reconnect.o(.rodata*)
        *bt_host_hs_hf.o(.rodata*)

        *hcic.o(.rodata*)
        *rf.o(.rodata*)

        *hci_host.o(.rodata*)
        *hci_cmd.o(.rodata*)
        *(.rf_rodata_seg)
        *(.rf_bss_seg)

        *(.bth_sdp_const_seg)
    } >SRAM2

    .bank_sec : AT( "BANK_LADDR" )
    {
        . = ALIGN(4);
        . = ALIGN("BANK_ADDR" + 0x2000*0);
        *(.bank0_seg)
        *(.mode_bt_seg)
        *(.mode_bt_const_seg)
        *(.bth_api_seg)
        *(.bt_appdata_seg*)
        *(.bth_app_seg)
        *(.btc_sleep_seg)
        *(.btc_hcicom_seg)
         *(.rf_datadft_seg)

        . = ALIGN("BANK_ADDR" +0x2000*1);
        *(.bank1_seg)
        *(.btc_init_seg)
        *(.bth_hci_seg)
        *(.blec_sec_conn_seg)

        . = ALIGN("BANK_ADDR" +0x2000*2);
        *(.bank2_seg)
        *(.bth_avdtp_seg)
        *(.bth_avctp_seg)
        *(.bth_a2dpapi_seg)
        *(.bth_init_seg)
        *(.bth_a2dp_seg)
        *(.btc_tmp_seg)
        *(.btc_init_rodata_seg)

        . = ALIGN("BANK_ADDR" +0x2000*3);
        *(.bank3_seg)
        *(.bth_sdp_const_seg)
        *(.bth_sdp_seg)
        *(.blec_llc_task_llcp_seg)
        *(.blec_hcicom_seg)
        *(.bleh_stack_seg)

        . = ALIGN("BANK_ADDR" +0x2000*4);
        *(.bank4_seg)
        *(.bth_atcmd_seg)
        *(.bth_spp_seg)
        *(.bth_reconnect_seg)
        *(.bth_hfp_seg)
        *(.bth_reconnect_seg)
        *(.bth_hfpconnect_seg)
        *(.blec_llc_task_hci_seg)
        *(.blec_llc_task_llc_seg)

        . = ALIGN("BANK_ADDR" +0x2000*5);
        *(.bank5_seg)
        *(.bth_rfcomm_seg)
        *(.bth_l2cap_seg)
        *(.bth_rfcomm_seg)

        . = ALIGN("BANK_ADDR" +0x2000*6);
        *(.bank6_seg)
        *(.btc_hci_seg)
        *llm_task.o(.text*)

        . = ALIGN("BANK_ADDR" +0x2000*7);
        *(.bank7_seg)
        *(.sys_init_seg)
        *(.gpio_init_seg)
        *(.sd_init_seg)
        *(.usb_init_seg)
        *(.usb_const_data)
        *(.i2s_flash_seg)
        *hal_i2s.o(.rodata.CSWTCH*)
        *(.freertos_flash_seg)

        *(.btaudio_flash_seg)
        *(.a2dp_play_flash_seg)
        *(.sco_audio_flash_seg)
        *(.uart_init_seg)
        *(.pmu_init_seg)
        *(.twi_init_seg)

        . = ALIGN("BANK_ADDR" +0x2000*8);
        *(.bank8_seg)
        *(.auss_flash_seg)
        *(.audio_general_seg)
        *(.plc_init_seg)
        *(.volume_control_flash_seg)
        *(.ain_flash_seg)
        *(.adc_ain_flash_seg)
        *(.karaoke_ain_flash_seg)
        *(.recorder_ain_flash_seg)
        *(.aout_flash_seg)
        *(.aout_rodata_flash_seg)
        *(.dac_aout_flash_seg)
        *(.karaoke_aout_flash_seg)

        *(.audio_init_seg)
        *(.info_init_seg)

        . = ALIGN("BANK_ADDR" +0x2000*9);
        *(.bank9_seg)
        *(.default_int_seg)
        *vector_gcc.o(.rodata*)
        *(.audioplayer_flash_seg)
        *(.audiotrack_flash_seg)
        *(.karaoke_flash_seg)
        *(.fade_flash_seg)

        *(.pmu_seg)
        *(.tsps_seg)
        *(.drc_seg)

        . = ALIGN("BANK_ADDR" +0x2000*10);
        *(.bank10_seg)
        *(.mode_set_seg)
        *(.mode_set_const_seg)
        *(.eqsetting_load)
        *(.mode_music_seg)
        *(.player_const_seg)
        *(.player_seg)
        *(.device_seg)
        *(.autotune_init_seg)

        *(.mode_com_init)

        . = ALIGN("BANK_ADDR" +0x2000*11);
        *(.bank11_seg)
        *(.ducker_init_seg)
        *(.tsps_init_seg)
        *(.id3_meta_const_seg)
        *(.id3_meta_seg)

        . = ALIGN("BANK_ADDR" +0x2000*12);
        *(.bank12_seg)
        *(.rtc_seg)
        *(.rtc_rodata_seg)
        *(.mode_clock_seg)
        *(.mode_usbdev_seg)
        *(.mode_poweroff_seg)
        *(.vorbis_meta_seg)
        *(.rf_driver0_seg)

        . = ALIGN("BANK_ADDR" +0x2000*13);
        *(.bank13_seg)
        *(.mode_fm_seg)
        *(.fm_seg)
        *(.fm_qn8065_seg)
        *(.fm_qn8065_rodata_seg)
        *(.fm_qn8035_seg)
        *(.fm_qn8035_rodata_seg)
        *(.fm_rda5807_seg)
        *(.fm_rda5807_rodata_seg)
        *(.mode_record_seg)
        *(.mode_linein_seg)

        . = ALIGN("BANK_ADDR" +0x2000*14);
        *(.bank14_seg)
        *(.rf_driver_seg)

        . = ALIGN("BANK_ADDR" +0x2000*15);
        *(.bank15_seg)
        *(.rf_driver1_seg)
        *(.blec_llm_task_seg)
        *(.blec_llc_asses_seg)
        *(.blec_llc_task_seg)
        *(.blec_llc_seg)
        *(.blec_llc_util_seg)
        *(.blec_llm_util_seg)

        . = ALIGN("BANK_ADDR" +0x2000*16);
        *(.bank16_seg)
        *(.blec_llcp_seg)
        *(.blec_hcic_msg_seg)
        *(.blec_hcic_seg)
        *(.blec_hcic_eif_seg)
        *(.blec_hci_fc_seg)
        *(.blec_ctr_seg)
        *(.blec_ea_init_seg)
        *(.bleh_dtm_seg)

        . = ALIGN("BANK_ADDR" +0x2000*17);
        *(.bank17_seg)
        *(.bleh_sm_seg)
        *(.bleh_host_hci_seg)
        *(.bleh_host_hal_seg)
        *(.blec_lld_test_seg)

        . = ALIGN("BANK_ADDR" +0x2000*18);
        *(.bank18_seg)
        *(.bleh_device_seg)
        *(.bleh_gap_seg)
        *(.bleh_att_db_seg)
        *(.bleh_att_dispatch_seg)
        *(.bleh_att_server_seg)
        *(.bleh_app_seg)
        *(.bleh_L2cap_seg)
        *(.bleh_common_ram_seg)

        . = ALIGN("BANK_ADDR" +0x2000*19);
        *(.bank19_seg)
        *(.blec_llm_seg)
        *(.blec_llc_conn_seg)
        *(.blec_llc_util_conn_seg)
        *(.blec_llm_conn_seg)
        *(.blec_llm_util_conn_seg)
        *(.blec_llc_llcp_conn_seg)
        *(.blec_llc_task_conn_seg)
        *(.blec_ea_conn_seg)
        *(.blec_lld_init_seg)

        . = ALIGN("BANK_ADDR" +0x2000*20);
        *(.bank20_seg)
        *(.echo_init_seg)
        *helper_ptr.o(.text*)
        *helper_ptr.o(.rodata*)
        *(.fdnrev_init_seg)
        *(.tsps_init_seg)
        *dly_rev.o(.text*)
        *dly_rev.o(.rodata*)
        *(.sdpro_init_seg)
        *(.fs_init_seg)
        *(.crossfade_init_seg)
        *(.pitch_init_seg)

        . = ALIGN("BANK_ADDR" +0x2000*21);
        *(.bank21_seg)
        *(.key_init_seg)
        *(.kadc_init_seg)
        *(.key_init_rodata_seg)
        *(.ledc_init_seg)
        *(.disp_init_seg)
        *(.irrx_init_seg)
        *(.spi_init_seg)
        *(.pwm_init_seg)
        *(.trng_init_seg)
        *(.spdif_init_seg)
        *(.timer_init_seg)
        *(.eid_1_seg)
        *(.update_init)

        *(.decorr_init_seg)
        *(.nhs_init_seg)

        . = ALIGN("BANK_ADDR" +0x2000*22);
        *(.bank22_seg)
        *(.pcm_flash_seg)
        *(.mixer_flash_seg)

        *(.usbaudio_flash_seg)
        *(.lineinaudio_flash_seg)
        *(.metadata_flash_seg)
        *(.metadata_rodata_flash_seg)
        *(.hwasrc_flash_seg)
        *(.audio_clksync_flash_seg)
        *(.tone_mixer_flash_seg)
        *(.sbc_auxtrack_flash_seg)
        *(.mp3_auxtrack_flash_seg)
        *(.auxtrack_mixer_flash_seg)
        *(.xfs_flash_seg)
        *(.xfs_diskio_flash_seg)
        *(.stream_flash_seg)
        *(.fatfs_stream_flash_seg)
        *(.tonefs_stream_flash_seg)
        *(.audiorecorder_flash_seg)
        *(.bitstream_store_flash_seg)
        *(.spi_rec_flash_seg)
        *(.fcc_datadft_seg)
        *(.dio_flash_seg)
        *(.bt_flash_seg)

        *(.afx_plug_seg)

        *(.rodata)
        *(.rodata*)

        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.glue_7)         /* glue arm to thumb code */
        *(.glue_7t)        /* glue thumb to arm code */
        *(.eh_frame)
        *(.eq_seg)
        *(.fc_init_seg)

        . = ALIGN(4);
        _etext = .;        /* define a global symbols at end of code */
    } >BANK
    .ARM.extab  :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } >BANK

    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >BANK



    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    __etext = _etext;
    __init_data = _sidata;
    __data_start__ = _sdata;
    _data = _sdata;
    __data_end__ = _edata;

    .ARM.attributes 0 : { *(.ARM.attributes) }
}


into_borther_item                                   =0x0000d061;
xfs_get_fileinfo                                    =0x0000d14b;
into_dir                                            =0x0000cf3d;
check_file_name                                     =0x0000cee1;
ld_clust                                            =0x0000e14d;
p_suffix_wl                                         =0x008004cc;
xfs_dir_tree                                        =0x008011f8;
afx_cur_dir_depth                                   =0x008011fc;
f_readdir                                           =0x0000c538;
f_mkdir                                             =0x0000cb44;

sp_tan                                              =0x0005fcc8;
sp_arm_cos_f32                                      =0x0005fbd4;
sp_arm_sin_f32                                      =0x0005fc50;
shelf2low                                           =0x0005d0ea;
primes                                              =0x00065400;

read_chunk_stsd                                     =0x00043135;
read_chunk_stts                                     =0x00042af5;
read_chunk_stsz                                     =0x00042b3d;
read_chunk_stsc                                     =0x00042b8f;
read_chunk_stco                                     =0x00042d99;
qtmovie_read                                        =0x00043305;
get_mp4_metadata                                    =0x00043d65;
qtmovie_getframe                                    =0x00042bd9;
qtmovie_seek                                        =0x00042c8d;
qtmovie_cmd                                         =0x00042d3d;
qtmovie_close                                       =0x00042d59;
aac_decode_frame_int                                =0x00037699;

_init                                               =0x00000138;
_fini                                               =0x00000144;
__aeabi_drsub                                       =0x00000154;
__aeabi_dsub                                        =0x0000015c;
__subdf3                                            =0x0000015c;
__adddf3                                            =0x00000160;
__aeabi_dadd                                        =0x00000160;
__aeabi_ui2d                                        =0x000003d8;
__floatunsidf                                       =0x000003d8;
__aeabi_i2d                                         =0x000003f8;
__floatsidf                                         =0x000003f8;
__extendsfdf2                                       =0x0000041c;
__aeabi_f2d                                         =0x0000041c;
__aeabi_ul2d                                        =0x00000460;
__floatundidf                                       =0x00000460;
__floatdidf                                         =0x00000470;
__aeabi_l2d                                         =0x00000470;
__muldf3                                            =0x000004cc;
__aeabi_dmul                                        =0x000004cc;
__aeabi_ddiv                                        =0x00000720;
__divdf3                                            =0x00000720;
__fixunsdfsi                                        =0x000008f0;
__aeabi_d2uiz                                       =0x000008f0;
__aeabi_ldivmod                                     =0x00000930;
__aeabi_uldivmod                                    =0x000009d0;
__udivmoddi4                                        =0x00000a00;
__aeabi_idiv0                                       =0x00000ccc;
__aeabi_ldiv0                                       =0x00000ccc;
_mainCRTStartup                                     =0x00000d10;
_start                                              =0x00000d10;
INTC_SetVector                                      =0x00000d84;
INTC_GetVector                                      =0x00000da8;
clk_log2                                            =0x00000e64;
module_is_enable                                    =0x00000e74;
module_ccu_enable                                   =0x00000e94;
module_ccu_disable                                  =0x00000ea0;
set_corepll_clk                                     =0x00000ee6;
set_audiopll_rate                                   =0x00000f28;
get_corepll_clk                                     =0x00000f74;
get_audiopll_clk                                    =0x00000fa0;
get_audiopllx8_clk                                  =0x00000fd0;
corepll_enable                                      =0x00000ff4;
corepll_disable                                     =0x00001010;
audiopll_enable                                     =0x00001024;
audiopll_disable                                    =0x0000103c;
clk_enable                                          =0x0000104c;
clk_disable                                         =0x000011a0;
clk_set_div                                         =0x000012f4;
clk_get_div                                         =0x000013dc;
clk_set_src                                         =0x00001474;
clk_get_src                                         =0x00001610;
clk_get_rate                                        =0x00001720;
clk_set_rate                                        =0x0000186c;
module_reset                                        =0x00001928;
clk_bypass_usb_suspend_clk                          =0x00001948;
clk_reset_cpu1                                      =0x00001960;
clk_dcxo_set                                        =0x00001980;
ccu_init                                            =0x000019a8;
ccu_deinit                                          =0x000019e8;
HAL_SD_Init                                         =0x00002116;
HAL_SD_ReadyForData                                 =0x000024b4;
HAL_SD_Stop                                         =0x000024f4;
HAL_SD_Read                                         =0x0000254e;
//HAL_SD_WaitBusy                                     =0x000025e6;
HAL_SD_Write                                        =0x000025f8;
HAL_SD_CheckRWOperation                             =0x00002694;
HAL_SD_IRQHandler                                   =0x00002736;
USB_EnableGlobalInt                                 =0x00002924;
USB_DisableGlobalInt                                =0x00002938;
USB_InitFSLSPClkSel                                 =0x00003268;
RegisterIsrDmaProcessImpl							=0x00003568;
HAL_HCD_Init                                        =0x00003574;
HAL_HCD_SOF_Callback                                =0x00003664;
HAL_HCD_Connect_Callback                            =0x00003674;
HAL_HCD_Disconnect_Callback                         =0x00003684;
HAL_HCD_HC_NotifyURBChange_Callback                 =0x00003698;
HAL_HCD_GetCurrentFrame                             =0x000036f8;
USBH_CtlSendSetup                                   =0x00003f54;
USBH_CtlSendData                                    =0x00003f70;
USBH_CtlReceiveData                                 =0x00003f8a;
USBH_CtlReq                                         =0x000041d4;
USBH_SetCfg                                         =0x000044a4;
USBH_ClrFeature                                     =0x000044ec;
USBH_OpenPipe                                       =0x00004510;
USBH_ClosePipe                                      =0x00004524;
USBH_AllocPipe                                      =0x00004528;
USBH_FreePipe                                       =0x00004556;
DeInitStateMachine                                  =0x0000456c;
USBH_Init                                           =0x000045b4;
USBH_RegisterClass                                  =0x00004610;
USBH_SelectInterface                                =0x00004616;
USBH_FindInterface                                  =0x00004640;
USBH_Start                                          =0x000046bc;
USBH_Stop											=0x000046c0;
USBH_HandleEnum                                     =0x000046f8;
USBH_Process                                        =0x00004810;
USBH_LL_SetTimer                                    =0x0000498c;
USBH_MSC_BOT_REQ_Reset                              =0x000049f8;
USBH_MSC_BOT_REQ_GetMaxLUN                          =0x00004a10;
USBH_MSC_BOT_Init                                   =0x00004a2c;
USBH_MSC_DecodeCSW									=0x00004a74;
USBH_RegisterPreprocessImpl                         =0x00004f70;
USBH_RegisterDmaCallbackImpl                        =0x00004f7c;
USBH_MSC_GetLUNInfo                                 =0x00004fd8;
USBH_MSC_Read_dma                                   =0x00005078;
USBH_MSC_Write_dma                                  =0x000050bc;
USBH_MSC_SCSI_TestUnitReady                         =0x00005488;
USBH_MSC_SCSI_ReadCapacity                          =0x000054dc;
USBH_MSC_SCSI_Inquiry                               =0x00005550;
USB_disk_write                                      =0x000057b8;
USB_disk_ioctl                                      =0x00005804;
USB_disk_hdl_register                               =0x00005854;
HAL_HCD_MspInit                                     =0x00005860;
USBH_LL_Init                                        =0x00005888;
USBH_LL_GetSpeed                                    =0x000058f8;
USBH_LL_ResetPort                                   =0x000058fc;
USBH_LL_SetToggle                                   =0x00005918;
USBH_LL_GetToggle                                   =0x00005938;
USBH_Delay                                          =0x00005958;
USBD_CtlSendData                                    =0x0000595c;
USBD_CtlPrepareRx                                   =0x00005982;
USBD_GetString										=0x00005ce6;
USBD_CtlError                                       =0x00005d04;
USBD_Init                                           =0x00005d50;
USBD_DeInit                                         =0x00005d70;
USBD_RegisterClass                                  =0x00005d98;
USBD_Start                                          =0x00005d9e;
USBD_Stop                                           =0x00005da2;
USBD_COMP_EP0_RxReady                               =0x000062fc;
USBD_COMP_MSC_Init                                  =0x00006360;
USBD_COMP_MSC_Setup                                 =0x0000644c;
USBD_COMP_MSC_DataIn                                =0x000064fc;
USBD_COMP_Init                                      =0x000065a0;
USBD_COMP_MSC_DeInit                                =0x0000666c;
USBD_COMP_MSC_RegisterStorage                       =0x00006790;
USBD_COMP_AUDIO_RegisterInterface                   =0x00006996;
Register_HID_Callback                               =0x000069d0;
USBD_COMP_REG_DESC                                  =0x000069dc;
USBD_LL_OpenEP                                      =0x00007224;
USBD_LL_CloseEP                                     =0x0000722e;
USBD_LL_FlushEP                                     =0x00007232;
USBD_LL_Transmit                                    =0x0000726c;
USBD_LL_PrepareReceive                              =0x00007270;
COMP_STORAGE_IsReady                                =0x00007280;
COMP_STORAGE_GetMaxLun                              =0x00007287;   /* thumbObject */
USB_WritePacket                                     =0x0000728c;
USB_ReadPacket                                      =0x000072b4;
USBH_BulkSendData                                   =0x00007718;
USBH_BulkReceiveData                                =0x00007736;
USBH_MSC_Read                                       =0x000079a0;
USBH_MSC_SCSI_RequestSense                          =0x00007a10;
USB_disk_status                                     =0x00007b34;
USB_disk_read                                       =0x00007b50;
USBH_LL_GetLastXferSize                             =0x00007b90;
USBH_LL_GetURBState                                 =0x00007ba8;
USB_ReadInterrupts                                  =0x00007bc4;
USB_GetMode                                         =0x00007bd4;
USB_HC_ReadInterrupt                                =0x00007be4;
USB_HC_Halt                                         =0x00007c04;
HAL_HCD_IRQHandler                                  =0x000080be;
HAL_NORF_CLK_Change                                 =0x00008564;
write_irq_set_region_addr                           =0x00008580;
write_irq_enable_region_irq                         =0x000085b6;
write_irq_is_region_pending                         =0x000085d8;
write_irq_get_addr                                  =0x00008606;
write_irq_get_pc                                    =0x00008618;
write_irq_get_sr                                    =0x0000862a;
nor_irq_process                                     =0x00008745;   /* thumbObject */
nor_init                                            =0x0000877c;
nor_read                                            =0x00008884;
nor_write                                           =0x0000897c;
nor_erase                                           =0x000089b0;
nor_crc_enable                                      =0x000089e4;
nor_crc_disable                                     =0x000089fc;
nor_workmode_change                                 =0x00008a3c;
xTimerCreateTimerTask                               =0x00008c08;
xTimerCreate                                        =0x00008c4c;
xTimerGenericCommand                                =0x00008c86;
pcTimerGetTimerName                                 =0x00008ea0;
xTimerIsTimerActive                                 =0x00008ea4;
pvTimerGetTimerID                                   =0x00008ebc;
xTimerPendFunctionCallFromISR                       =0x00008ec0;
xTimerPendFunctionCall                              =0x00008ee8;
xTaskGenericCreate                                  =0x00008f10;
vTaskDelete                                         =0x000090c4;
vTaskStartScheduler                                 =0x00009170;
vTaskDelayUntil                                     =0x000091d8;
xEventGroupCreate                                   =0x00009248;
xEventGroupWaitBits                                 =0x00009262;
xEventGroupClearBits                                =0x00009318;
xEventGroupGetBitsFromISR                           =0x00009332;
xEventGroupSetBits                                  =0x0000934e;
xEventGroupSync                                     =0x000093aa;
vEventGroupDelete                                   =0x00009434;
vEventGroupSetBitsCallback                          =0x0000945a;
vEventGroupClearBitsCallback                        =0x0000945e;
vListInitialise                                     =0x00009464;
xQueueGenericReset                                  =0x0000947c;
xQueueGenericCreate                                 =0x000094e8;
xQueueCreateCountingSemaphore                       =0x00009522;
xQueueCreateMutex                                   =0x00009534;
xQueueAltGenericSend                                =0x0000957a;
xQueueAltGenericReceive                             =0x00009634;
xQueueGiveFromISR                                   =0x0000971c;
xQueueReceiveFromISR                                =0x00009784;
xQueuePeekFromISR                                   =0x000097e0;
uxQueueMessagesWaiting                              =0x00009810;
uxQueueSpacesAvailable                              =0x00009822;
uxQueueMessagesWaitingFromISR                       =0x00009838;
vQueueDelete                                        =0x0000983c;
xQueueIsQueueEmptyFromISR                           =0x00009840;
xQueueIsQueueFullFromISR                            =0x0000984a;
xQueueCreateSet                                     =0x00009856;
xQueueAddToSet                                      =0x0000985e;
xQueueRemoveFromSet                                 =0x00009880;
xQueueSelectFromSet                                 =0x000098a0;
xQueueSelectFromSetFromISR                          =0x000098b8;
inHandlerMode                                       =0x000098d0;
osMessageCreate                                     =0x000098dc;
osMessagePut                                        =0x000098e6;
osMessageGet                                        =0x0000994c;
osQueueSend                                         =0x000099e0;
osQueueReceive                                      =0x00009a3c;
osSemaphoreGive                                     =0x00009a94;
osSemaphoreTake                                     =0x00009ad4;
osTaskNotify                                        =0x00009b38;
vAssertCalled                                       =0x00009b74;
vApplicationIdleHook                                =0x00009b88;
vApplicationTickHook                                =0x00009b9c;
vApplicationMallocFailedHook                        =0x00009bb0;
vApplicationStackOverflowHook                       =0x00009bc4;
vPreSleepHook                                       =0x00009bd8;
vPostSleepHook                                      =0x00009bec;
pxPortInitialiseStack                               =0x00009c2c;
vPortEndScheduler                                   =0x00009c50;
vPortSuppressTicksAndSleep                          =0x00009c52;
vPortEnableVFP                                      =0x00009dd0;
vPortSetSystickClkHz                                =0x00009e20;
prvHeapConfig                                       =0x00009efc;
pvPortMalloc                                        =0x00009f24;
vPortFree                                           =0x0000a010;
xPortGetFreeHeapSize                                =0x0000a05c;
xPortGetMinimumEverFreeHeapSize                     =0x0000a068;
vPortInitialiseBlocks                               =0x0000a074;
eTaskGetState                                       =0x0000a11c;
uxTaskPriorityGet                                   =0x0000a17c;
uxTaskPriorityGetFromISR                            =0x0000a198;
vTaskPrioritySet                                    =0x0000a1c0;
vTaskResume                                         =0x0000a284;
xTaskResumeFromISR                                  =0x0000a30c;
vTaskEndScheduler                                   =0x0000a39c;
vTaskSuspendAll                                     =0x0000a3bc;
xTaskGetTickCount                                   =0x0000a3cc;
xTaskGetTickCountFromISR                            =0x0000a3d8;
uxTaskGetNumberOfTasks                              =0x0000a3e4;
pcTaskGetTaskName                                   =0x0000a3f0;
xTaskGetIdleTaskHandle                              =0x0000a400;
vTaskStepTick                                       =0x0000a40c;
xTaskIncrementTick                                  =0x0000a41c;
xTaskResumeAll                                      =0x0000a538;
vTaskDelay                                          =0x0000a60c;
prvIdleTask                                         =0x0000a66c;
vTaskSwitchContext                                  =0x0000a6ec;
vTaskSuspend                                        =0x0000a77c;
vTaskPlaceOnEventList                               =0x0000a82c;
vTaskPlaceOnUnorderedEventList                      =0x0000a884;
vTaskPlaceOnEventListRestricted                     =0x0000a8e8;
xTaskRemoveFromEventList                            =0x0000a92c;
xTaskRemoveFromUnorderedEventList                   =0x0000a9a0;
vTaskSetTimeOutState                                =0x0000a9fc;
xTaskCheckForTimeOut                                =0x0000aa14;
vTaskMissedYield                                    =0x0000aa60;
eTaskConfirmSleepModeStatus                         =0x0000aa6c;
xTaskGetCurrentTaskHandle                           =0x0000aa8c;
xTaskGetSchedulerState                              =0x0000aa98;
vTaskPriorityInherit                                =0x0000aab8;
xTaskPriorityDisinherit                             =0x0000ab48;
uxTaskResetEventItemValue                           =0x0000abb8;
pvTaskIncrementMutexHeldCount                       =0x0000abd0;
ulTaskNotifyTake                                    =0x0000abe8;
xTaskNotifyWait                                     =0x0000ac8c;
xTaskNotify                                         =0x0000ad58;
xTaskNotifyFromISR                                  =0x0000adf4;
vTaskNotifyGiveFromISR                              =0x0000aeac;
puTaskStackAddr                                     =0x0000af40;
pcTaskName                                          =0x0000af4c;
vListInitialiseItem                                 =0x0000af98;
vListInsertEnd                                      =0x0000af9e;
vListInsert                                         =0x0000afb6;
uxListRemove                                        =0x0000afe4;
xQueueGenericSend                                   =0x0000b150;
xQueueGenericSendFromISR                            =0x0000b25c;
xQueueGenericReceive                                =0x0000b2d2;
vQueueWaitForMessageRestricted                      =0x0000b3f4;
vPortSVCHandler                                     =0x0000b42d;   /* thumbObject */
vPortEnterCritical                                  =0x0000b44c;
vPortExitCritical                                   =0x0000b46c;
xPortPendSVHandler                                  =0x0000b481;   /* thumbObject */
xPortSysTickHandler                                 =0x0000b4e1;   /* thumbObject */
disk_initialize                                     =0x0000b510;
disk_write                                          =0x0000b538;
disk_ioctl                                          =0x0000b56c;
disk_config                                         =0x0000b59c;
f_mount                                             =0x0000b98c;
f_read                                              =0x0000ba14;
f_write                                             =0x0000bbc8;
f_sync                                              =0x0000bdbe;
f_close                                             =0x0000bf12;
f_lseek                                             =0x0000bf44;
f_closedir                                          =0x0000c2fc;
f_open                                              =0x0000c638;
f_opendir                                           =0x0000c960;
f_setcp                                             =0x0000ceba;
disk_status                                         =0x0000d4f4;
disk_read                                           =0x0000d514;
unlock_fs                                           =0x0000d671;   /* thumbObject */
inc_lock                                            =0x0000d709;   /* thumbObject */
move_window                                         =0x0000d839;   /* thumbObject */
clst2sect                                           =0x0000d905;   /* thumbObject */
get_fat                                             =0x0000d91b;   /* thumbObject */
clmt_clust                                          =0x0000df25;   /* thumbObject */
dir_sdi                                             =0x0000dfa7;   /* thumbObject */
dir_next                                            =0x0000e03b;   /* thumbObject */
dir_read                                            =0x0000e4bb;   /* thumbObject */
validate                                            =0x0000e6bd;   /* thumbObject */
dir_find                                            =0x0000e9eb;   /* thumbObject */
HAL_DMA_Init                                        =0x0000f094;
HAL_DMA_DeInit                                      =0x0000f0bc;
HAL_DMA_Start_IT                                    =0x0000f0e8;
HAL_DMA_Abort                                       =0x0000f144;
HAL_DMA_IRQHandler                                  =0x0000f15c;
timestamp_init                                      =0x0000f1b8;
us_to_timestamp                                     =0x0000f1d8;
ms_to_timestamp                                     =0x0000f1dc;
timestamp_to_us                                     =0x0000f1e6;
timestamp_to_ms                                     =0x0000f1e8;
get_timestamp                                       =0x0000f1f6;
set_timestamp                                       =0x0000f22c;
cdelay                                              =0x0000f244;
udelay                                              =0x0000f252;
mdelay                                              =0x0000f26e;
msleep                                              =0x0000f290;
HAL_IWDG_Init                                       =0x0000f2b4;
HAL_IWDG_DeInit                                     =0x0000f2e0;
HAL_IWDG_Refresh                                    =0x0000f2ec;
HAL_TRNG_Init                                       =0x0000f300;
HAL_TRNG_DeInit                                     =0x0000f304;
HAL_TRNG_GetRandomNumber                            =0x0000f308;
lc_send_lmp                                         =0x0000ffe0;
lc_send_pdu_not_acc                                 =0x0001019c;
lc_send_pdu_not_acc_ext4                            =0x000101dc;
lc_send_pdu_encaps_payl                             =0x00010420;
lc_send_pdu_io_cap_res_entiy                        =0x00010869;   /* thumbObject */
lc_lmp_acc_handler                                  =0x00010b0c;
lc_lmp_detach_handler                               =0x00010f99;   /* thumbObject */
lc_lmp_io_cap_req_handler                           =0x0001123c;
lc_acl_data_req_handler                             =0x00017589;   /* thumbObject */
lc_lmp_acc_handler_entity                           =0x000183bf;   /* thumbObject */
lc_lmp_sniff_req_handler_entity                     =0x00018b5d;   /* thumbObject */
lc_chg_pkt_type_cont                                =0x000190d8;
lc_common_nb_of_pkt_comp_evt_send                   =0x0001922a;
lc_con_cmp_evt_send                                 =0x0001924e;
lc_free_tx_acl_buf                                  =0x000194c8;
lc_local_switch                                     =0x000196fc;
lc_mst_qos_done                                     =0x000197fc;
lc_enter_hold_mode                                  =0x00019c60;
lc_start_entity                                     =0x00019d39;   /* thumbObject */
lc_start_lmp_to                                     =0x00019e10;
lc_enter_sniff_mode                                 =0x0001a00c;
lc_start_enc                                        =0x0001a594;
lc_start_enc_key_size                               =0x0001a710;
lc_start_key_exch                                   =0x0001a778;
lc_stop_enc                                         =0x0001a808;
lc_restart_enc_cont_entity                          =0x0001a9cd;   /* thumbObject */
lc_semi_key_cmp                                     =0x0001abe4;
lc_rem_sniff                                        =0x0001b9b8;
lc_switch_cmp                                       =0x0001be34;
lc_enc_cmp                                          =0x0001c020;
lc_epr_cmp                                          =0x0001c278;
lc_sp_fail                                          =0x0001c458;
lc_calc_link_key                                    =0x0001c4c4;
lc_dhkey                                            =0x0001c628;
lc_resp_pair_entity                                 =0x0001ca8d;   /* thumbObject */
lc_scatt_init                                       =0x0001cc2c;
lc_util_rsw_en                                      =0x0001ce4c;
lc_util_set_loc_trans_coll                          =0x0001ce84;
ld_ack_received                                     =0x0001d0bc;
ld_ack_received_entity                              =0x0001d0cd;   /* thumbObject */
ld_next_acl_tx                                      =0x0001d2f4;
ld_next_acl_tx_entity                               =0x0001d305;   /* thumbObject */
ld_rx_sync                                          =0x0001d55c;
ld_rx_sync_entity                                   =0x0001d569;   /* thumbObject */
ld_data_rx                                          =0x0001d774;
ld_data_rx_entity                                   =0x0001d791;   /* thumbObject */
ld_rx_clock_entity                                  =0x0001d90d;   /* thumbObject */
ld_rx_isr                                           =0x0001e648;
ld_clock_isr                                        =0x0001e664;
ld_acl_data_tx                                      =0x0001f34c;
ld_acl_flow_check                                   =0x0001f650;
co_list_init                                        =0x0001f660;
co_list_push_back                                   =0x0001f668;
co_list_push_front                                  =0x0001f67c;
co_list_pop_front                                   =0x0001f688;
co_list_extract                                     =0x0001f698;
co_list_extract_after                               =0x0001f6ce;
co_list_find                                        =0x0001f6ea;
co_list_merge                                       =0x0001f6fc;
co_list_insert_after                                =0x0001f738;
co_list_size                                        =0x0001f75a;
co_alloc_buf                                        =0x0001f76c;
co_alloc_buf_entity                                 =0x0001f779;   /* thumbObject */
co_free_buf                                         =0x0001f79a;
co_free_buf_entity                                  =0x0001f7a5;   /* thumbObject */
co_buf_init                                         =0x0001f7e6;
co_alloc_acl_buf_tx                                 =0x0001f874;
co_alloc_acl_buf_rx                                 =0x0001f888;
co_alloc_acl_buf_rx_entity                          =0x0001f895;   /* thumbObject */
co_free_acl_buf                                     =0x0001f8a8;
co_slot_to_duration                                 =0x0001f8e8;
co_bdaddr_compare                                   =0x0001f8f2;
co_conhdl_init                                      =0x0001f912;
co_conhdl_bdaddr_to_tid                             =0x0001f998;
co_conhdl_type_get                                  =0x0001fa0c;
co_conhdl_to_tid                                    =0x0001fa1c;
ke_task_schedule                                    =0x0001fa85;   /* thumbObject */
ke_task_create                                      =0x0001fb4c;
ke_state_set                                        =0x0001fbdc;
ke_state_get                                        =0x0001fc60;
ke_msg_alloc                                        =0x0001fc9c;
ke_msg_alloc_entity                                 =0x0001fcb1;   /* thumbObject */
ke_msg_send                                         =0x0001fce0;
ke_msg_send_entity                                  =0x0001fced;   /* thumbObject */
ke_msg_send_basic                                   =0x0001fd10;
ke_msg_forward_entity                               =0x0001fd2d;   /* thumbObject */
ke_msg_free                                         =0x0001fd4e;
ke_flush                                            =0x0001fd60;
ke_sleep_check                                      =0x0001fda8;
ke_timer_hw_set_entity                              =0x0001fe09;   /* thumbObject */
ke_timer_schedule                                   =0x0001fe3d;   /* thumbObject */
ke_timer_set                                        =0x0001fe9c;
ke_timer_set_entity                                 =0x0001fea9;   /* thumbObject */
ke_timer_clear                                      =0x0001ff4c;
ke_timer_clear_entity                               =0x0001ff59;   /* thumbObject */
ke_timer_active                                     =0x0001ffbc;
ke_timer_active_entity                              =0x0001ffc9;   /* thumbObject */
ke_event_init                                       =0x00020054;
ke_event_schedule                                   =0x00020064;
ke_event_set                                        =0x00020070;
ke_event_callback_set                               =0x0002007c;
ke_event_clear                                      =0x00020094;
lm_util_get_offset_clkn                             =0x00020174;
lm_util_get_offset_clke                             =0x0002018a;
lm_util_add_event                                   =0x000201a0;
lm_util_delete_event                                =0x00020400;
lm_util_init_events                                 =0x00020438;
LM_UpdateAclPacketType                              =0x0002047c;
LM_UpdateAclEdrPacketType                           =0x000204c4;
LM_CreateUnitKey                                    =0x00020714;
LM_GetFeature                                       =0x00020a0c;
LM_CheckTransactionID                               =0x00020ae0;
lm_lmp_rx_ind_handler_entity                        =0x00020c29;   /* thumbObject */
lm_sp_get_pairing_mode                              =0x00020e1c;
lm_get_encap_pdu_data                               =0x00020e3c;
lm_get_nonce                                        =0x0002101c;
lm_init_sp                                          =0x00021094;
lm_sp_32bits_to_array                               =0x000211d8;
lm_get_oob_local_commit                             =0x000211f0;
lm_oob_f1                                           =0x0002139c;
lm_g1                                               =0x000213b2;
lm_dhkey_calc_init                                  =0x00021808;
lm_sp_dhkey_get_buffer_idx                          =0x000219b8;
lm_initlowpow                                       =0x00021ba8;
lm_common_cmd_complete_send                         =0x000220dc;
lc_lmp_sco_lk_rem_req_handler                       =0x00022335;   /* thumbObject */
lc_lmp_esco_lk_rem_req_handler                      =0x00022301;   /* thumbObject */
lc_sco_start_link_entity                            =0x0002297d;   /* thumbObject */
lc_sco_modify_link_entity                           =0x00022ab1;   /* thumbObject */
lc_sco_stop_link_entity                             =0x00022b6d;   /* thumbObject */
lc_sco_init                                         =0x00022b72;
lc_sco_reset                                        =0x00022b8c;
lc_sco_peer_accept                                  =0x000233d0;
lc_sco_peer_request_disc                            =0x00023524;
lc_sco_peer_accept_disc                             =0x000235ec;
ld_access_init_cs_access_entity                     =0x00023de5;   /* thumbObject */
ld_access_f_entity                                  =0x00025659;   /* thumbObject */
ld_access_rx_access_entity                          =0x000256c1;   /* thumbObject */
ld_access_setup_complete                            =0x00025724;
ld_util_lmp_tx_entity                               =0x000258bd;   /* thumbObject */
ld_util_disable_power_cntl                          =0x000259d8;
ld_util_get_nb_acl                                  =0x00025a44;
ld_util_get_min_poll_interval                       =0x00025aa8;
ld_util_poll_int_update                             =0x00025b54;
lm_get_sync_offset_entity                           =0x000266c1;   /* thumbObject */
lm_get_allowed_intv_bitmask_entity                  =0x0002679d;   /* thumbObject */
lm_get_sync_param_hl_req_entity                     =0x00026de9;   /* thumbObject */
lm_get_sync_param_entity                            =0x00027db1;   /* thumbObject */
lm_init_sync                                        =0x00027efc;
ld_afh_init                                         =0x00028738;
ld_afh_hssi_switch_instant_entity                   =0x00028ff5;   /* thumbObject */
ld_afh_implicit_disabling                           =0x000291cc;
ld_afh_implicit_enabling                            =0x000291d0;
ld_lowpow_unsniff                                   =0x000294f0;
ld_init_link                                        =0x000295f8;
ld_init_link_entity                                 =0x00029605;   /* thumbObject */
ld_init_CQDDR                                       =0x00029940;
ld_init_CQDDR_entity                                =0x0002994d;   /* thumbObject */
ld_compute_tx_type_entity                           =0x000299b5;   /* thumbObject */
ld_cqddr_new                                        =0x00029a70;
ld_cqddr_new_entity                                 =0x00029a7d;   /* thumbObject */
ld_lowpowit_compute_sync_window                     =0x00029d14;
ld_lowpowit_compute_sync_window_entity              =0x00029d29;   /* thumbObject */
ld_lowpowit_manage_entity                           =0x0002a04d;   /* thumbObject */
ld_pbf_enhanced_flush_cs_tx                         =0x0002a8ac;
ld_util_read_features                               =0x0002ab8c;
ld_util_alloc_lt_addr                               =0x0002ab9c;
ld_util_free_lt_addr                                =0x0002abc4;
ld_util_set_bd_addr                                 =0x0002abe0;
ld_util_set_poll_interval                           =0x0002aed4;
ld_util_get_master_clock                            =0x0002af78;
ld_util_get_cs_clock_offset                         =0x0002afa8;
ld_util_dec_power_requested                         =0x0002b0fc;
ld_util_inc_power_requested                         =0x0002b144;
ld_util_test_mode_cntl_entity                       =0x0002b2c1;   /* thumbObject */
ld_util_set_sp_feature_bit                          =0x0002b77c;
ld_util_check_rsw_act                               =0x0002b7bc;
ld_switch_req                                       =0x0002b834;
ld_switch_start_entity                              =0x0002b91d;   /* thumbObject */
ld_acl_init_fifo                                    =0x0002c3a0;
ld_acl_flow_control                                 =0x0002c43c;
ld_aclit_disc_link                                  =0x0002c4d0;
ld_aclit_start_acl_flush_timer                      =0x0002cb14;
ld_aclit_set_next_acl_entity                        =0x0002cb91;   /* thumbObject */
GLOBAL_INT_DISABLE                                  =0x0002cf68;
GLOBAL_INT_RESTORE                                  =0x0002cf74;
co_rand_word                                        =0x0002cf80;
ke_malloc                                           =0x0002cf8c;
ke_free                                             =0x0002cf98;
ld_sco_init                                         =0x0002d0b4;
ld_sco_configure_con_entity                         =0x0002d269;   /* thumbObject */
ld_sco_manage_entity                                =0x0002d889;   /* thumbObject */
lld_util_get_bd_address                             =0x000316c8;
lld_util_set_bd_address                             =0x000316f0;
lld_util_ral_force_rpa_renew                        =0x00031734;
lld_util_freq2chnl                                  =0x00031778;
lld_util_get_local_offset                           =0x00031798;
lld_util_get_peer_offset                            =0x000317ae;
lld_util_connection_param_set                       =0x000317c6;
lld_util_dle_set_cs_fields                          =0x0003180a;
lld_util_anchor_point_move                          =0x0003182c;
lld_util_flush_list                                 =0x0003191c;
lld_data_rx_check                                   =0x00031966;
lld_data_rx_flush                                   =0x000319b4;
lld_data_tx_check                                   =0x000319e0;
lld_data_tx_loop                                    =0x00031a5c;
lld_data_tx_push                                    =0x00031a94;
lld_data_tx_prog                                    =0x00031ae4;
lld_data_tx_flush                                   =0x00031b80;
lld_adv_start                                       =0x00031bb0;
lld_adv_stop                                        =0x00031d68;
lld_scan_start                                      =0x00031d94;
lld_scan_stop                                       =0x00031f18;
lld_con_start                                       =0x00031f44;
lld_move_to_master                                  =0x00032450;
lld_con_update_req                                  =0x00032488;
lld_con_update_after_param_req                      =0x00032510;
lld_con_param_rsp                                   =0x00032760;
lld_con_param_req                                   =0x00032870;
lld_con_stop                                        =0x00032948;
lld_get_mode                                        =0x00032972;
lld_move_to_slave                                   =0x0003299c;
lld_ch_map_ind                                      =0x00032ae8;
lld_con_update_ind                                  =0x00032b0a;
lld_crypt_isr                                       =0x00032b20;
lld_ral_renew_req                                   =0x00032b26;
em_buf_rx_free                                      =0x00032b40;
em_buf_rx_buff_addr_get                             =0x00032b58;
em_buf_tx_buff_addr_get                             =0x00032b64;
em_buf_tx_alloc                                     =0x00032b70;
em_buf_tx_free                                      =0x00032bf0;
lld_evt_prevent_stop                                =0x00032d90;
lld_evt_schedule                                    =0x00032e0c;
lld_evt_deferred_elt_pop                            =0x00032f08;
lld_evt_init_evt                                    =0x00032f70;
lld_evt_elt_delete                                  =0x00032fa0;
lld_evt_conhdl2elt                                  =0x0003318c;
lld_evt_drift_compute                               =0x000331ac;
lld_evt_elt_insert                                  =0x00033694;
lld_evt_canceled                                    =0x000336d0;
lld_evt_schedule_next                               =0x000336fc;
lld_evt_scan_create                                 =0x000337e0;
lld_evt_move_to_master                              =0x00033870;
lld_evt_update_create                               =0x00033990;
lld_evt_ch_map_update_req                           =0x00033ab8;
lld_evt_move_to_slave                               =0x00033ad0;
lld_evt_slave_update                                =0x00033d84;
lld_evt_adv_create                                  =0x00033e44;
lld_evt_deffered_elt_handler                        =0x00033eec;
lld_evt_end                                         =0x00033fce;
lld_evt_rx                                          =0x0003413c;
sp_calloc                                           =0x00034550;
sp_free                                             =0x00034560;
sp_realfft                                          =0x0005f7b0;
sp_realifft                                         =0x0005f8bc;
sp_cos_twiddle                                      =0x0006590c;
sp_sin_twiddle                                      =0x0006694c;
general_imdct                                       =0x00034f60;
general_log2                                        =0x000350c0;
general_pow2                                        =0x00035100;
general_exp                                         =0x000351a4;
general_sincos                                      =0x000352a8;
general_inc_mem                                     =0x000352c8;
general_memconfig                                   =0x000352e0;
general_malloc                                      =0x00035308;
general_calloc                                      =0x00035340;
general_malloc_max                                  =0x0003536c;
general_calloc_max                                  =0x00035394;
general_free                                        =0x000353bc;
general_malloc_stack                                =0x000353cc;
general_calloc_stack                                =0x00035404;
general_free_stack                                  =0x00035430;
general_get_avaheapsize                             =0x00035448;
aac_check_enough_bits                               =0x0003545d;   /* thumbObject */
aac_framedec                                        =0x000379e1;   /* thumbObject */
aac_cmd                                             =0x00037a2d;   /* thumbObject */
check_enough_bytes                                  =0x00037fed;   /* thumbObject */
range_decode_culfreq                                =0x00038085;   /* thumbObject */
range_decode_culshift                               =0x000381a3;   /* thumbObject */
entropy_decode_mono_3900                            =0x000382bf;   /* thumbObject */
entropy_decode_stereo_3930                          =0x000382e7;   /* thumbObject */
entropy_decode_mono_3990                            =0x00038327;   /* thumbObject */
entropy_decode_stereo_3990                          =0x0003834f;   /* thumbObject */
predictor_decode_stereo_3930                        =0x0003838f;   /* thumbObject */
predictor_decode_mono_3930                          =0x000383fb;   /* thumbObject */
predictor_decode_stereo_3950                        =0x0003844f;   /* thumbObject */
predictor_decode_mono_3950                          =0x00038705;   /* thumbObject */
ape_decode_frame                                    =0x000387e5;   /* thumbObject */
ape_decode_cmd                                      =0x00038bc5;   /* thumbObject */
flac_check_enough_bits                              =0x000391dc;
ff_vorbis_nth_root                                  =0x0003dc10;
ff_vorbis_len2vlc                                   =0x0003dc32;
ff_vorbis_ready_floor1_list                         =0x0003dd08;
vorbis_check_enough_bits                            =0x0003e01d;   /* thumbObject */
ff_init_vlc_sparse                                  =0x0003e118;
ff_free_vlc                                         =0x0003e34c;
vorbis_clear_saved                                  =0x0003e4a8;
vorbis_floor0_decode                                =0x0003e4c1;   /* thumbObject */
vorbis_floor1_decode                                =0x0003e789;   /* thumbObject */
vorbis_framedec                                     =0x0003f597;   /* thumbObject */
vorbis_cmd                                          =0x0003f9dd;   /* thumbObject */
vorbis_static_init                                  =0x0003fa22;
vorbis_static_alloc                                 =0x0003fa2c;
vorbis_static_giveback                              =0x0003fa92;
sbc_unpack_frame                                    =0x0004010d;   /* thumbObject */
msbc_unpack_frame                                   =0x000401a5;   /* thumbObject */
sbc_decode                                          =0x000401e5;   /* thumbObject */
sbc_get_stream_info                                 =0x00040320;
sbc_frame_decode                                    =0x000403b5;   /* thumbObject */
av_log2                                             =0x00040c44;
wma_check_enough_bits                               =0x00040c51;   /* thumbObject */
wma_window_output                                   =0x00041195;   /* thumbObject */
wma_decode_block                                    =0x00041e4b;   /* thumbObject */
wma_decode_phdr_decode                              =0x00041fc1;   /* thumbObject */
seek_to_next_block                                  =0x00042121;   /* thumbObject */
wmadec_frame_decode                                 =0x000421cd;   /* thumbObject */
wmadec_cmd                                          =0x00042363;   /* thumbObject */
cvsd_decoder_open                                   =0x00048b7c;
cvsd_decoder_close                                  =0x00048bac;
cvsd_decoder_proc                                   =0x00048bb4;
cvsd_encoder_open                                   =0x00048cc8;
cvsd_encoder_close                                  =0x00048cec;
cvsd_encoder_proc                                   =0x00048cf4;
sbc_encode_close                                    =0x0004913a;
sbc_encode_open                                     =0x0004915c;
sbc_encode_proc                                     =0x000498b8;
wavenc_close                                        =0x00049c82;
wavenc_open                                         =0x00049ca4;
wavenc_proc                                         =0x00049d50;
wavenc_cmd                                          =0x00049e06;
cos_twiddle                                         =0x0004b000;
decibel_table                                       =0x0004b034;
pow_table                                           =0x0004b438;
sin_twiddle                                         =0x0004c040;
vlc_scalefactors                                    =0x0004c080;
vlc_spectral                                        =0x0004c090;
ff_mpeg4audio_channels                              =0x0004c7ab;
ff_mpeg4audio_sample_rates                          =0x0004c7b4;
vlc_scalefactors_table                              =0x0004de3c;
vlc_spectral0                                       =0x0004e124;
vlc_spectral1                                       =0x0004e2e4;
vlc_spectral10                                      =0x0004e48c;
vlc_spectral2                                       =0x0004eacc;
vlc_spectral3                                       =0x0004ecac;
vlc_spectral4                                       =0x0004ee8c;
vlc_spectral5                                       =0x0004f04c;
vlc_spectral6                                       =0x0004f20c;
vlc_spectral7                                       =0x0004f384;
vlc_spectral8                                       =0x0004f4ec;
vlc_spectral9                                       =0x0004f8a4;
ape_filter_orders                                   =0x0004fc5c;
ff_flac_blocksize_table                             =0x0004fd4c;
ff_flac_sample_rate_table                           =0x0004fd8c;
flac_decoder_plugin                                 =0x0004fdcc;
sample_size_table                                   =0x0004fddc;
avpriv_mpa_bitrate_tab                              =0x00050044;
avpriv_mpa_freq_tab                                 =0x000500f8;
band_index_long                                     =0x000500fe;
band_size_long                                      =0x0005029c;
band_size_short                                     =0x00050362;
csa_table                                           =0x000503d8;
ff_mdct_win_float                                   =0x00050458;
ff_mpa_alloc_tables                                 =0x000508d8;
ff_mpa_quant_bits                                   =0x000508ec;
ff_mpa_quant_steps                                  =0x00050930;
ff_mpa_sblimit_table                                =0x00050974;
huff_quad_vlc_depth                                 =0x00051208;
huff_quad_vlc_tables                                =0x0005120a;
huff_vlc_table_depth                                =0x000512ca;
huff_vlc_tables                                     =0x000512da;
is_table                                            =0x0005338c;
is_table_lsf                                        =0x0005340c;
lsf_nsf_table                                       =0x0005350c;
mp3_exp2table                                       =0x00053554;
mp3_pow43tab                                        =0x00053564;
mpa_huff_data                                       =0x000535a4;
mpa_pretab                                          =0x000535e4;
mul_12_tab                                          =0x00053610;
mul_2_tab                                           =0x0005361c;
slen_table                                          =0x00053628;
mp3_decoder_plugin                                  =0x00053730;
pcm_decoder_plugin                                  =0x00053e6c;
exp_vlc                                             =0x0005b894;
exponent_band_22050                                 =0x0005bb24;
exponent_band_32000                                 =0x0005bb6f;
exponent_band_44100                                 =0x0005bbba;
hgain_vlc                                           =0x0005bc08;
lsp_codebook                                        =0x0005bd18;
pow_10_to_yover16                                   =0x0005bf98;
vlc_rom_table                                       =0x0005c19c;
wma_critical_freqs                                  =0x0005c1b4;
wma_pow_table                                       =0x0005c1e8;
aac_parser_plugin                                   =0x0005c43c;
aacm4a_parser_plugin                                =0x0005c490;
ape_parser_plugin                                   =0x0005c4dc;
flac_parser_plugin                                  =0x0005c4fc;
mp3_parser_plugin                                   =0x0005c510;
ogg_parser_plugin                                   =0x0005c550;
wav_parser_plugin                                   =0x0005cad8;
wma_parser_plugin                                   =0x0005cba0;
h                                                   =0x0005cbe0;
biquad_setcoeffs                                    =0x0005cec1;   /* thumbObject */
shelf2high                                          =0x0005d191;   /* thumbObject */
filter_design                                       =0x0005d241;   /* thumbObject */
biquad_init                                         =0x0005d331;   /* thumbObject */
biquad_proc                                         =0x0005d33b;   /* thumbObject */
lm_default_priv_key                                 =0x0005e698;
lm_default_pub_key                                  =0x0005e6f8;
co_default_bdaddr                                   =0x0005edd4;
co_null_bdaddr                                      =0x0005edda;
co_sca2ppm                                          =0x0005ede0;
sp_log2                                             =0x0005f9f1;   /* thumbObject */
sp_pow2                                             =0x0005fa31;   /* thumbObject */
aenc_update_rse_asm                                 =0x0005fce5;   /* thumbObject */
aenc_update_cpsdxe_asm                              =0x0005fe07;   /* thumbObject */
aenc_postfilt_applygain_asm                         =0x0005fed1;   /* thumbObject */
aenc_update_background_asm                          =0x00060049;   /* thumbObject */
avq_caculate_maskthd_asm                            =0x00060195;   /* thumbObject */
aenc_update_rpsd_asm                                =0x000604f9;   /* thumbObject */
aenc_foreground_filter_asm                          =0x0006079d;   /* thumbObject */
aenc_background_filter_asm                          =0x0006087d;   /* thumbObject */
aenc_calculate_gain_asm                             =0x0006095d;   /* thumbObject */
agc_proc                                            =0x00060e05;   /* thumbObject */
aenc_init_variable_states                           =0x00060fe0;
aenc_preemp_notch_tx                                =0x0006157d;   /* thumbObject */
aenc_preemp_rx                                      =0x00061649;   /* thumbObject */
aenc_stepsize_control                               =0x000616f9;   /* thumbObject */
aenc_adjust_prop                                    =0x0006177d;   /* thumbObject */
aenc_update_pxx                                     =0x00061841;   /* thumbObject */
aenc_update_respsd                                  =0x00061961;   /* thumbObject */
aenc_update_NpsdSlow                                =0x000619f9;   /* thumbObject */
aenc_update_dtthd                                   =0x00061ad5;   /* thumbObject */
aenc_update_Npsd                                    =0x00061e7d;   /* thumbObject */
aenc_smooth_gain                                    =0x00061f59;   /* thumbObject */
aenc_deemp_tx                                       =0x000620b9;   /* thumbObject */
avq_update_refpsd                                   =0x00062131;   /* thumbObject */
avq_apply_gain                                      =0x00062261;   /* thumbObject */
avq_deemp_rxout                                     =0x0006238d;   /* thumbObject */
aenc_smooth_initNpsd                                =0x00062409;   /* thumbObject */
aenc_postfilter_wola                                =0x00062441;   /* thumbObject */
aenc_convert_txout                                  =0x0006249d;   /* thumbObject */
avq_convert_rxout                                   =0x000624dd;   /* thumbObject */
aenc_update_idx                                     =0x00062521;   /* thumbObject */
aenc_ref_tfpsd                                      =0x00062575;   /* thumbObject */
aenc_get_residual_fg                                =0x00062605;   /* thumbObject */
aenc_update_resbg_dtstatistics                      =0x00062641;   /* thumbObject */
aenc_transfer_dt_logic                              =0x00062731;   /* thumbObject */
aenc_get_res_final                                  =0x0006287d;   /* thumbObject */
aenc_update_stepsize_logic                          =0x000628e9;   /* thumbObject */
aenc_initial_stepsize_update                        =0x00062a9d;   /* thumbObject */
aenc_postfilter                                     =0x00062b29;   /* thumbObject */
aenc_blockproc                                      =0x00062c78;
aenc_cmd                                            =0x0006307c;
agc_open                                            =0x00063145;   /* thumbObject */
agc_close                                           =0x00063159;   /* thumbObject */
sltkplc_close                                       =0x000632b8;
sltkplc_open                                        =0x00063374;
sltkplc_conceal                                     =0x000634fa;
sltkplc_addhist                                     =0x00063686;
drc_close                                           =0x00063a50;
drc_cmd                                             =0x00063e54;
drc_init                                            =0x00063ecc;
drc_process                                         =0x00064318;
eq_close                                            =0x00064602;
eq_open                                             =0x0006460a;
eq_cmd                                              =0x0006462a;
eq_process                                          =0x00064a66;
Cerlog                                              =0x00066980;
S                                                   =0x000669c8;
bark                                                =0x00066f38;
bark_fbin                                           =0x00066f7a;
bark_lenrecip                                       =0x00066fac;
bidx                                                =0x00066ff4;
hwindow                                             =0x00067008;
wd1                                                 =0x00067208;
delay_nop                                           =0x00067444;
boot_usb_init                                       =0x00067462;
USBD_COMP_Desc_Init                                 =0x00068144;
HAL_UART_Init                                       =0x00068378;
HAL_UART_DeInit                                     =0x00068420;
HAL_UART_Transmit                                   =0x00068424;
HAL_UART_Receive                                    =0x00068436;
HAL_UART_EnableIT                                   =0x0006844c;
serial_debug_io_init                                =0x000684b4;
xprintf                                             =0x000684c0;
printf_array                                        =0x000686bc;
HAL_UART_IRQHandler                                 =0x0006870c;
get_pin_group_and_id                                =0x0006873c;
HAL_GPIO_Set_Func                                   =0x0006879e;
HAL_GPIO_To_Irq                                     =0x00068838;
HAL_GPIO_Irq_Enable                                 =0x0006887c;
HAL_GPIO_Irq_Disable                                =0x00068894;
HAL_GPIO_Set_Debounce                               =0x000688ae;
HAL_GPIO_Get_Pending                                =0x000688b4;
gpio_config                                         =0x00068924;
gpio_cfg_func                                       =0x0006895c;
gpio_get_value                                      =0x00068962;
gpio_set_value                                      =0x00068974;
pin_config                                          =0x000689ae;
pin_get_value                                       =0x000689f0;
pin_set_value_rom                                   =0x00068a12;
pin_irq_clk_config                                  =0x00068a30;
pin_irq_config                                      =0x00068a46;
pin_get_irq_pending                                 =0x00068a76;
pin_clear_irq_pending                               =0x00068a9a;
pin_get_wakeup_pending                              =0x00068ab6;
pin_clear_wakeup_pending                            =0x00068ad4;
pin_set_wakeup                                      =0x00068af0;
pin_get_func                                        =0x00068b0a;
strcpy                                              =0x00068b64;
strlen                                              =0x00068bd4;
strcmp                                              =0x00068be4;
memcpy                                              =0x00068c5c;
memset                                              =0x00068cc0;
memmove                                             =0x00068d10;
memcmp                                              =0x00068d46;
libc_malloc                                         =0x00068d62;
libc_free                                           =0x00068d66;
libc_calloc                                         =0x00068d6a;
qsort                                               =0x00068d82;
atoi                                                =0x00068e10;
CpuClkHz                                            =0x0080016c;
SystickClkHz                                        =0x00800170;
TickRateHz                                          =0x00800174;
uxCriticalNesting                                   =0x00800178;
mini_stack_size                                     =0x0080017c;
timer_queue_length                                  =0x00800180;
timer_task_priority                                 =0x00800184;
g_pHhcd                                             =0x00800190;
g_pHpcd												=0x00800198;
USBH_msc                                            =0x008001e4;
g_pUsbHidCallback                                   =0x0080020c;
HID_Buffer                                          =0x00800210;
USBD_COMP_HID                                       =0x0080022c;
USBD_COMP_MSC                                       =0x00800258;
USBD_COMP_AUDIO                                     =0x00800284;
USBD_COMP                                           =0x008002b0;
USBD_COMP_MSC_CfgFSDesc                             =0x008002dc;
COMP_HID_MOUSE_ReportDesc                           =0x008002fc;
USBD_COMP_HID_Desc                                  =0x00800320;
USBD_COMP_DeviceQualifierDesc                       =0x0080032c;
USBD_COMP_CfgDesc                                   =0x00800338;
USBD_COMP_DISK_fops									=0x00800484;
COMP_Desc                                           =0x0080042c;
COMP_STORAGE_Inquirydata                            =0x008004a8;
TASK_DESC_LC                                        =0x00800514;
TASK_DESC_LD                                        =0x00800524;
TASK_DESC_LM                                        =0x0080057c;
hhcd                                                =0x00800900;
pUSBD_COMP_CfgDesc                                  =0x00800ba4;
USBD_COMP_HID_DATA                                  =0x00800bbc;
USBD_COMP_MSC_DATA                                  =0x00800bcc;
pCOMP_HID_MOUSE_ReportDesc                          =0x00800e38;
pUSBD_COMP_HID_Desc                                 =0x00800e3c;
USBD_COMP_AUDIO_DATA                                =0x00800e40;
pUSBD_COMP_DISK_fops                                =0x00800e88;
pUSBD_COMP_AUDIO_fops                               =0x00800e8c;
pMMCDiskStatus                                      =0x00801108;
pUSBDiskStatus                                      =0x0080110c;
pMMCDiskInit                                        =0x00801110;
pUSBDiskInit                                        =0x00801114;
pMMCDiskRead                                        =0x00801118;
pMMCReadFailCallback                                =0x0080111c;
pUSBDiskRead                                        =0x00801120;
pUSBReadFailCallback                                =0x00801124;
pMMCDiskWrite                                       =0x00801128;
pUSBDiskWrite                                       =0x0080112c;
pMMCDiskIoctl                                       =0x00801130;
pUSBDiskIoctl                                       =0x00801134;
uNorfHdl                                            =0x00801200;
g_memctx_tmp                                        =0x00801290;
g_memctx_setup                                      =0x0080129c;
pxCurrentTCB                                        =0x00801304;
xIdleTaskHandle                                     =0x008014e0;
xSchedulerRunning                                   =0x008014fc;
xTickCount                                          =0x00801528;
SystickClkSrc                                       =0x00801550;
ulStoppedTimerCompensation                          =0x00801554;
ulTimerCountsForOneTick                             =0x00801558;
xMaximumPossibleSuppressedTicks                     =0x0080155c;
pTskUserFunc                                        =0x00801560;
lc_state                                            =0x00801564;
lc_env                                              =0x00801568;
pfunc_lc_con_cmp                                    =0x00801d88;
lc_pfunc                                            =0x00801d8c;
pfunc_lc_sco_modify_link                            =0x00801db8;
pfunc_lc_sco_stop_link                              =0x00801dbc;
lc_scatt_env                                        =0x00801dc4;
ld_afh_env                                          =0x00801dd4;
ld_lowpow_env                                       =0x0080223c;
ld_pfunc                                            =0x008022d0;
NORMAL_WIN_SIZE                                     =0x00802330;
BT_BCAST_PRIO_DEF                                   =0x00802332;
BT_BEACON_PRIO_DEF                                  =0x00802333;
BT_ACL_PRIO_INC                                     =0x00802334;
BT_CONNECT_PRIO_DEF                                 =0x00802335;
BT_SCO_PRIO_DEF                                     =0x00802336;
BT_SCO_PRIO_INC                                     =0x00802337;
ld_env                                              =0x00802338;
ld_edr_enabled                                      =0x0080245c;
ld_radio_offset                                     =0x0080245d;
BT_NONCONN_PRIO_INC                                 =0x0080245e;
BT_MCONPEND_PRIO_DEF                                =0x0080245f;
BT_SCONPEN_PRIO_DEF                                 =0x00802460;
BT_MSSWITCH_PRIO_DEF                                =0x00802461;
BT_LOWPOW_PRIO_DEF                                  =0x00802462;
BT_POLLINT_PRIO_DEF                                 =0x00802463;
BT_PAGE_PRIO_DEF                                    =0x00802464;
BT_PAGESCAN_PRIO_DEF                                =0x00802465;
BT_INQ_PRIO_DEF                                     =0x00802466;
BT_INQSCAN_PRIO_DEF                                 =0x00802467;
BT_ACLTX_PRIO_DEF                                   =0x00802468;
BT_LMPTX_PRIO_DEF                                   =0x00802469;
BT_ARB_MARGIN                                       =0x0080246a;
SWITCH_WIN_SIZE                                     =0x0080246c;
INQPAGE_WIN_SIZE                                    =0x0080246e;
ld_access_env                                       =0x00802470;
ld_access_pfunc                                     =0x008024cc;
ld_eir_env                                          =0x008024ec;
ld_lowpowit_env                                     =0x008024f4;
LD_LOWPOW_SNIFF_EVT_END_WAIT_SYNC                   =0x00802500;
ld_bch                                              =0x00802501;
ld_pbf_acl_cs_non_flushable                         =0x00802509;
ld_util_env                                         =0x00802514;
ld_switch_env                                       =0x0080253c;
ld_inqtxpwr_cs                                      =0x008025ac;
ld_acl_env                                          =0x008025b0;
ld_sco_env                                          =0x00802820;
ld_sco_pfunc                                        =0x00802880;
ld_aclit_env                                        =0x0080288c;
ld_ssr_env                                          =0x0080295c;
lm_util_env                                         =0x008029fc;
pfunc_lm_lmp_rx_ind_handler                         =0x00802a50;
lm_sp_debug_mode                                    =0x00802a54;
lm_pub_key                                          =0x00802a58;
lm_sp_dhkey_data                                    =0x00802bd8;
lm_sp_mode                                          =0x00802ea0;
lm_priv_key                                         =0x00802ea4;
lm_oob_r                                            =0x00802f04;
lm_oob_c                                            =0x00802f14;
lm_state                                            =0x00802f28;
lm_env                                              =0x00802fa4;
lm_sync_conf                                        =0x008031c4;
lm_nb_sync_active                                   =0x0080324c;
pfunc_lm_get_sync_offset                            =0x00803250;
pfunc_lm_get_allowed_intv_bitmask                   =0x00803254;
lm_sync_nego                                        =0x00803258;
pfunc_lm_get_sync_param_hl_req                      =0x0080325c;
lm_nego_max_cnt                                     =0x00803260;
lm_nego_cnt                                         =0x00803261;
lm_nego_cntl                                        =0x00803262;
lm_nego_pkt_used                                    =0x00803264;
pfunc_lm_get_sync_param                             =0x00803268;
hci_pfunc                                           =0x0080326c;
sltip_rf                                            =0x0080328c;
SLTBT_SW_VERSION_SUB_BUILD                          =0x008032c4;
SLTBT_SW_VERSION_BUILD                              =0x008032c5;
SLTBT_SW_VERSION_MINOR                              =0x008032c6;
SLTBT_SW_VERSION_MAJOR                              =0x008032c7;
SLT_COMP_ID                                         =0x008032c8;
pfunc_global_int_diable                             =0x008032cc;
pfunc_global_int_restore                            =0x008032d0;
pfunc_co_rand_word                                  =0x008032d4;
pfunc_co_alloc_buf                                  =0x0080377c;
pfunc_co_free_buf                                   =0x00803780;
co_buf_env                                          =0x00803784;
pfunc_co_alloc_acl_buf_rx                           =0x008037d0;
ke_task_env                                         =0x008037f0;
pfunc_ke_msg_alloc                                  =0x0080383c;
pfunc_ke_msg_send                                   =0x00803840;
pfunc_ke_msg_forward                                =0x00803844;
ke_env                                              =0x00803848;
pfunc_ke_malloc                                     =0x00803860;
pfunc_ke_free                                       =0x00803864;
pfunc_ke_timer_hw_set                               =0x00803868;
pfunc_ke_timer_set                                  =0x0080386c;
pfunc_ke_timer_clear                                =0x00803870;
pfunc_ke_timer_active                               =0x00803874;
ke_event_env                                        =0x00803878;
pfunc_ke_event_schedule                             =0x00803894;
pfunc_ke_event_set                                  =0x00803898;
BLE_ADV_PRIO_INC                                    =0x0080389c;
BLE_ADV_PRIO_DEF                                    =0x0080389d;
BLE_SCAN_PRIO_INC                                   =0x0080389e;
BLE_SCAN_PRIO_DEF                                   =0x0080389f;
BLE_SCONNECT_PRIO_INC                               =0x008038b6;
BLE_SCONNECT_PRIO_DEF                               =0x008038b7;
ble_default_bdaddr                                  =0x008038b8;
BLE_MCONNECT_PRIO_INC                               =0x008038be;
BLE_MCONNECT_PRIO_DEF                               =0x008038bf;
BLE_INIT_PRIO_INC                                   =0x008038c0;
BLE_INIT_PRIO_DEF                                   =0x008038c1;
BLE_ARB_MARGIN                                      =0x008038c2;
lld_evt_env                                         =0x008038c4;
em_buf_env                                          =0x008038e0;
llm_le_env                                          =0x00803ba4;
pfunc_evt_restart                                   =0x00803c5c;
pfunc_evt_schedule                                  =0x00803c60;
pfunc_ea_interval_insert                            =0x00803c64;
pfunc_ea_interval_create                            =0x00803c68;
pfunc_ea_elt_insert                                 =0x00803c6c;
pfunc_ea_interval_remove                            =0x00803c70;
pfunc_ea_elt_remove                                 =0x00803c74;
pfunc_ea_time_get_halfslot_rounded                  =0x00803c78;
pfunc_ea_elt_create                                 =0x00803c7c;
pfunc_ea_interval_duration_req                      =0x00803c80;
pfunc_ea_offset_req                                 =0x00803c84;
pfunc_llc_con_update_ind                            =0x00803c88;
pfunc_llc_map_update_ind                            =0x00803c8c;
pfunc_llc_lsto_con_update                           =0x00803c90;
pfunc_llm_util_adv_data_update                      =0x00803c94;
pfunc_llc_common_nb_of_pkt_comp_evt_send            =0x00803c98;
pfunc_llm_util_check_map_validity                   =0x00803c9c;
pfunc_llm_util_apply_bd_addr                        =0x00803ca0;
pfunc_adv_pack                                      =0x00803ca4;
pfunc_llm_util_aa_gen                               =0x00803ca8;
pfunc_llm_util_bd_addr_in_ral                       =0x00803cac;
llc_env                                             =0x00803cb0;
timBeforeSleep                                      =0x00800164;
