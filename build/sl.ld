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

BASE_LADDR          = 0x2000 + 0x14000; /* 8K loader + 80K ota */

VECTOR_LADDR        = 0x00000000;
VECTOR_ADDR         = 0x00800000;   /* keep it */
VECTOR_SIZE         = 0x100;

UPDATE_LADDR        = VECTOR_LADDR + VECTOR_SIZE;
UPDATE_ADDR         = 0x00804000;               /* keep it */
UPDATE_SIZE         = 0x40;

SYSTEM_LADDR        = UPDATE_LADDR + UPDATE_SIZE;
SYSTEM_ADDR         = UPDATE_ADDR + UPDATE_SIZE;
SYSTEM_SIZE         = 8K - UPDATE_SIZE;

SRAM0_LADDR         = SYSTEM_LADDR + SYSTEM_SIZE;
SRAM0_ADDR          = SYSTEM_ADDR + SYSTEM_SIZE;
SRAM0_SIZE          = 20K;                      /* 大小可配置，最小20K */

DATA_LADDR          = SRAM0_LADDR + SRAM0_SIZE;
DATA_ADDR           = SRAM0_ADDR + SRAM0_SIZE;
DATA_SIZE           = 2K;
LOAD_SIZE           = SRAM0_SIZE + DATA_SIZE - 20K;

BSS_ADDR            = DATA_ADDR + DATA_SIZE;
BSS_SIZE            = 147K;                     /* 大小可配置 */

STACK_ADDR          = BSS_ADDR + BSS_SIZE;
STACK_SIZE          = 2K;

SRAM1_LADDR         = DATA_LADDR + DATA_SIZE;
SRAM1_ADDR          = STACK_ADDR + STACK_SIZE;
SRAM1_SIZE          = 0x00836000 - SRAM1_ADDR;

SRAM2_LADDR         = SRAM1_LADDR + SRAM1_SIZE;
SRAM2_ADDR          = 0x40036100;
SRAM2_SIZE          = 0x1EFC; /* ~8K */

/* bank must be 8k align */
BANK_ALIGN          = 0x2000;
BANK_LADDR          = (( SRAM2_LADDR + SRAM2_SIZE + BANK_ALIGN ) / BANK_ALIGN) * BANK_ALIGN;
BANK_ADDR           =  0xC00000 + BANK_LADDR + BASE_LADDR;
BANK_SIZE           = 232K;     /* 29bank*8K */

TONE_SIZE           = 64K;
PARAM_ADDR          = BANK_LADDR + BANK_SIZE + TONE_SIZE + BASE_LADDR;

/* Specify the memory areas */
MEMORY
{
    VECTOR(rxw):    ORIGIN = VECTOR_ADDR,   LENGTH = VECTOR_SIZE
    SYSTEM(rxw):    ORIGIN = SYSTEM_ADDR,   LENGTH = SYSTEM_SIZE
    SRAM0(rxw):     ORIGIN = SRAM0_ADDR,    LENGTH = SRAM0_SIZE
    DATA(rxw):      ORIGIN = DATA_ADDR,     LENGTH = DATA_SIZE
    BSS(rxw):       ORIGIN = BSS_ADDR,      LENGTH = BSS_SIZE
    STACK(rxw):     ORIGIN = STACK_ADDR,    LENGTH = STACK_SIZE
    SRAM1(rxw):     ORIGIN = SRAM1_ADDR,    LENGTH = SRAM1_SIZE
    SRAM2(rxw):     ORIGIN = SRAM2_ADDR,    LENGTH = SRAM2_SIZE
    BANK(rx):       ORIGIN = BANK_ADDR,     LENGTH = BANK_SIZE
}

/* Define output sections */
SECTIONS
{
    .vector_sec : AT( "VECTOR_LADDR" )
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
    } >VECTOR

    .system_sec : AT( "SYSTEM_LADDR" )
    {
        . = ALIGN(4);
        *(.norflash_sram_seg)
        *(.sys_com_seg)
        *(.r_twi_seg)
        *(.gpio_seg)
        *(.gpio_rodata_seg)
        *hal_gpio.o(.rodata.CSWTCH*)
        *hal_gpio_lib.o(.rodata.CSWTCH*)
        *(.event_seg)
        *(.dual_cpu_seg)
        *(.pmu_sram_seg)
        *(.sys_sram_seg)
        *(.clk_sram_seg)

        *(.fade_sram_seg)
        *(.fade_rodata_sram_seg)
        *(.audio_mix_sram_seg)
        *(.pcm_sram_seg)
        *(.ring_buffer_sram_seg)
        *(.a2dp_play_sram_seg)
        *(.dma_seg)
        *(.dac_aout_sram_seg)
        *(.adc_ain_sram_seg)
        *(.ain_sram_seg)
        *(.aout_sram_seg)
        *(.volume_control_sram_seg)
    } >SYSTEM


    .sram0_sec : AT( "SRAM0_LADDR" )
    {
        . = ALIGN(4);
        /* 系统启动相关段（请勿更改）*/
        KEEP(*(.text.Reset_Handler))
        KEEP(*(.text.Exception_Handler))
        KEEP(*(.startup_seg))
        KEEP(*(.eid_seg))

        /* bt相关段 */
        *(.bth_common_ram_seg)
        *(.bth_connect_seg)
        *(.bth_basic_seg)
        *(.btc_common_ram_seg)
        *(.bt_ld_seg)
        *(.bt_com_seg)
        *(.btc_uart_seg)
        *(.btc_hciacl_seg)
        *(.btc_sleep1_sram_seg)
        *(.btc_bt_sleep_sram_seg)
        *(.btc_sleep_sram_seg)
        *(.bt_c2h_flow_cntl_seg)
        *bt_host_hal_uart.o(.rodata*)
        *sltip.o(.rodata*)
        *(.btc_access_ram_seg)
        *(.bth_uart_seg)
        *bt_host_rfcomm.o(.text*)
        *(.fcc_sram_seg)

        /* audio相关段 */
        *(.tone_mixer_sram_seg)
        *(.tone_mixer_rodata_sram_seg)
        *(.hwasrc_sram_seg)
        *(.karaoke_aout_sram_seg)
        *(.karaoke_ain_sram_seg)
        *(.karaoke_sram_seg)
        *(.audiotrack_sram_seg)
        *(.normal_audiotrack_sram_seg)
        *(.audio_sink_sram_seg)
        *(.audio_track_rodata_sram_seg)


        /* 其他段 */
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
        *(.info_seg)
        *(.usb_seg)
        *(.usbh_seg)
        *(.usbd_seg)
        *(.usbd_audio_seg)
        *(.usbd_audio_eq_seg)
        *(.usbd_msc_seg)
        *(.usbd_msc_eq_seg)
       
        *(.i2s_sram_seg)
        *(.auss_sram_seg)
        *(.spi_seg)
        *(.pwm_seg)
        *(.uart_sram_seg)
        *(.uart0_sram_seg)
        *(.uart1_sram_seg)
        *(.uart2_sram_seg)
        *(.uart_dma_sram_seg)
        *uart.o(.text*)
        *uart.o(.rodata*)
        *(.rtc_base_sram_seg)
        *(.rtc_clock_sram_seg)
        *(.rtc_alarm_sram_seg)
        *(.sd_sram_seg)
        *(.sd_isr_seg)
        *(.freertos_sram_seg)
        *(.app_sram_seg)
        *(.motor_sram_seg)
        
        *(.charge_sram_seg)
        *charge.o(.rodata*)
        *hal_power_lib.o(.rodata*)
        *(.xfs_diskio_must_in_sram_seg)
        *(.update_nor)
        *update.o(.rodata*)
        *update_api.o(.rodata*)
		
        *(.spcommon_xfilter_seg)
        *(.ble_app_sram_seg)
		  *(.mp2_enc_seg)
        *(.mp2_rodata_seg)
		*(.audiorecorder_sram_seg)
    } >SRAM0


    .data_sec : AT( "DATA_LADDR" )
    {
        . = ALIGN(4);
        _sdata = .;        /* create a global symbol at data start */
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */

        . = ALIGN(4);
        _edata = .;        /* define a global symbol at data end */

        /* used by the startup to initialize data */
        _sidata = _sdata;
    } >DATA


    .stack_sec :
    {
        . = ALIGN(4);
        *(.app_stack)
         . = ALIGN(4);
        __app_stack_end  = .;
    } >STACK


    .bss_start :
    {
        . = ALIGN(4);
        _bss = .;
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start__ = _sbss;
    } >BSS


    /* Uninitialized data section */
    . = ALIGN(4);
    .app_bss_sec :
    {
        /* This is used by the startup in order to initialize the .bss secion */
        *(.driver_bss)
        *(.bss)
        *(.bss*)
    } >BSS


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
    } >BSS


    .app_bss_end :
    {
        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
        __bss_end__ = _ebss;
    } >BSS

    .audio_heap :
    {
        . = ALIGN(4);
        *(.audio_heap)
        . = ALIGN(4);
    } >BSS

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
        *(.*audio_src_sram_seg)
        *(.bt_lctask_seg)
        *(.fdnrev_rodata_seg)
        *(.fdnrev_seg)
        *(.formant_seg)
        *(.spec_sram_seg)
        *(.postproc_tsps_sram_seg)
        *(.tsps_proc_seg)
        *(.eq_ram_proc_seg)
        *(.tsps_rodata_seg)
        *(.fc_data_seg)
        *(.fc_proc_seg)
        *(.autotune_proc_seg)
        *(.sdpro_seg)
        *(.sdpro_rodata_seg)

        *(.spec_rodata_seg)
        *(.spec_proc_seg)
        *(.decorr_rodata_seg)
        *(.decorr_proc_seg)
        *(.decorr_proc_int_seg)
        *(.dr_proc_seg)
        *(.dr_rodata_seg)
        *(.drc_proc_seg)
        *(.fs_proc_seg)
        *(.nhs_seg)
        *(.nhs_rodata_seg)
        *(.fs_data_seg)
        *(.ducker_proc_seg)
        *(.ducker_rodata_seg)
        *(.echo_proc_seg)
        *(.echo_rodata_seg)
        *(.eq2_rodata_seg)
        *(.eq2_proc_seg)
        *(._tbvc_proc_seg)
        *(._tbvc_rodata_seg)
        *(.semi_fs_proc_seg)
        *(.dump_seg)
        *(.crossfilt_seg)
        *(.cache_seg)
        . = ALIGN(4);
        *(.cpu1_overflow_check_seg)
        *(.cpu1_stack_seg)
        *(.cache_rodata_seg)
        *(.helptr_proc_seg)
        *(.sp_iir_seg)
        *(.crossfade_proc_seg)

        *(.ews_service_sram_seg)
        *(.ews_sram_seg)
        *(.bth_ews_seg)
        *(.ews_con_sram_seg)

        *(.blec_ea_seg)
        *(.blec_common_ram_seg)
        *(.blec_lld_seg)
        *(.blec_lld_util_seg)
        *(.blec_lld_evt_seg)
        *(.blec_lld_data_seg)
        *(.blec_llc_util_datatrans_seg)
        *(.sco_audio_sram_seg)
	    *(.admfd_proc_seg)
	    *(.admomini_proc_seg)
        *(.adm_admfd_proc_seg)
        *(.adm_aenc_proc_seg)
        *(.aenc_proc_ram)
        *(.recorder_ain_sram_seg)
        *(.fatfs_stream_sram_seg)
        *(.bitstream_store_sram_seg)
        *(.audioplayer_sram_seg)
        *(.xfs_sram_seg)
        *(.xfs_diskio_sram_seg)
	*(.trng_sram_seg)
	*(.timer_seg)
        *(.timer_sram_seg)
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

        *hci_host.o(.rodata*)
        *hci_cmd.o(.rodata*)
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
        *(.btc_sleep1_sram_seg)
         *(.rf_datadft_seg)
        *fcc_test.o(.rodata*)
        *(.ble_app_seg)
        *(.ble_app_const_seg)
        *(.bt_c2h_flow_cntl_init_seg)

        *(.bt_lctask1_seg)
        *(.bt_lm_seg)
        *(.bt_lmtask_seg)
        *(.bt_ld_seg)
        *(.bt_lc_seg)
		
		*(.bth_rfcomm_seg)
        *(.bth_l2cap_seg)
        *(.bth_rfcomm_seg)
        *(.rf_rodata_seg)
        *(.blec_em_seg)
		
        . = ALIGN("BANK_ADDR" +0x2000*1);
        *(.bank1_seg)
        *(.btc_init_seg)
        *(.bth_hci_seg)
        *(.btc_uart_flash_seg)
        *(.bt_lcsco_seg)
        *(.btc_buf_flash_seg)
        *(.bt_ldsco_seg)

		*(.btc_hci_seg)
        *llm_task.o(.text*)
        *(.bth_init_seg)

        *(.update_init)
        *(.update_seg)
        *(.mode_diskupdate_seg)
		*(.sys_init_seg)
        *(.sys_flash_seg)
        *(.gpio_init_seg)
        *(.sd_init_seg)
        *(.usb_init_seg)
        *(.usbh_init_seg)
        *(.usbd_init_seg)
        *(.usbd_comp_init_seg)
		*(.usbd_comp_stereo_init_seg)
        *(.usbd_comp_dump_init_seg)
        *(.usbd_audio_init_seg)
		*.(usbd_audio_stereo_init_seg)
        *(.usbd_mic_init_seg)
		*(.usbd_mic_stereo_init_seg)
        *(.usbd_msc_init_seg)
        *(.usbd_msc_card_init_seg)
        *(.usbd_msc_eq_init_seg)
        *(.usb_const_data)
        *(.freertos_flash_seg)

        *(.btaudio_flash_seg)
        *(.a2dp_play_flash_seg)
        *(.sco_audio_flash_seg)
	
		. = ALIGN("BANK_ADDR" +0x2000*2);
        *(.bank2_seg)
        *(.bth_avdtp_seg)
        *(.bth_avctp_seg)
        *(.bth_a2dpapi_seg)
        //*(.bth_init_seg)
        *(.bth_a2dp_seg)
        *(.btc_tmp_seg)
        *(.btc_init_rodata_seg)
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
		
        . = ALIGN("BANK_ADDR" +0x2000*3);
        *(.bank3_seg)
        *(.bth_sdp_const_seg)
        *(.bth_sdp_seg)
		
		*(.audioplayer_flash_seg)
        *(.audiotrack_flash_seg)
        *(.normal_audiotrack_flash_seg)
        *(.fade_flash_seg)
        *(.drc_seg)
		
        . = ALIGN("BANK_ADDR" +0x2000*4);
        *(.bank4_seg)
        *(.bth_atcmd_seg)
        *(.bth_spp_seg)
        *(.bth_reconnect_seg)
        *(.bth_hfp_seg)
        *(.bth_reconnect_seg)
        *(.bth_hfpconnect_seg)
        *(.blec_llc_task_llc_seg)
        *(.bth_rfcomm_seg1)
		
		*(.pmu_seg)
        *(.pmu_bat_flash_seg)
        *(.eqsetting_load)
        *(.mode_music_seg)
        *(.player_const_seg)

        . = ALIGN("BANK_ADDR" +0x2000*5);
        *(.bank5_seg)
		*(.ducker_init_seg)
        *(.dr_init_seg)
        *(.spec_init_seg)
        *(.id3_meta_const_seg)
        *(.id3_meta_seg)
        *(._tbvc_init_seg)
        *(.helptr_init_seg)
        *(.eq2_init_seg)
				
        *(.player_seg)
        *(.device_seg)
		*(.mode_com_init)
		
        . = ALIGN("BANK_ADDR" +0x2000*6);
        *(.bank6_seg)
        *(.rtc_base_seg)
        *(.rtc_clock_seg)
        *(.rtc_alarm_seg)
        *(.rtc_api_seg)
        *(.rtc_mem_seg)
        *(.rtc_rodata_seg)
        *(.mode_clock_seg)
        *(.mode_usbdev_seg)
        *(.mode_usbdev_audio_seg)
        *(.mode_poweroff_seg)
        *(.vorbis_meta_seg)
        *(.rf_driver0_seg)
        *rf.o(.rodata*)

        . = ALIGN("BANK_ADDR" +0x2000*7);
        *(.bank7_seg)
        *(.twi_init_seg)
        *(.mode_fm_seg)
        *(.fm_seg)
        *(.fm_qn8065_seg)
        *(.fm_qn8065_rodata_seg)
        *(.fm_qn8035_seg)
        *(.fm_qn8035_rodata_seg)
        *(.fm_rda5807_seg)
        *(.fm_rda5807_rodata_seg)
        *(.record_flash_seg)
        *(.mode_record_seg)
        *(.mode_linein_seg)
        *(.mode_spdif_seg)
        *(.mode_charge_seg)

        . = ALIGN("BANK_ADDR" +0x2000*8);
        *(.bank8_seg)
        *(.rf_driver_seg)

        . = ALIGN("BANK_ADDR" +0x2000*9);
        *(.bank9_seg)
        //*(.rf_driver1_seg)
        *(.blec_llm_task_seg)
        *(.blec_llc_asses_seg)
        *(.blec_llc_task_seg)
        *(.blec_llc_seg)
        *(.blec_llc_util_seg)
        *(.blec_llm_util_seg)
        *(.blec_llc_task_llcp_seg)
        *(.blec_hcicom_seg)
        //*(.bleh_stack_seg)

        . = ALIGN("BANK_ADDR" +0x2000*10);
        *(.bank10_seg)
        *(.blec_llcp_seg)
        *(.blec_hcic_msg_seg)
        *(.blec_hcic_seg)
        *(.blec_hcic_eif_seg)
        *(.blec_hci_fc_seg)
        *(.blec_ctr_seg)
        *(.blec_ea_init_seg)
        *(.bleh_dtm_seg)

        . = ALIGN("BANK_ADDR" +0x2000*11);
        *(.bank11_seg)
        *(.bleh_sm_seg)
        *(.bleh_host_hci_seg)
        *(.bleh_host_hal_seg)
        *(.blec_lld_test_seg)

        . = ALIGN("BANK_ADDR" +0x2000*12);
        *(.bank12_seg)
        *(.bleh_device_seg)
        *(.bleh_gap_seg)
        *(.bleh_att_db_seg)
        *(.bleh_att_dispatch_seg)
        *(.bleh_att_server_seg)
        *(.bleh_app_seg)
        *(.bleh_L2cap_seg)
        *(.bleh_common_ram_seg)

        . = ALIGN("BANK_ADDR" +0x2000*13);
        *(.bank13_seg)
        *(.blec_llm_seg)
        *(.blec_llc_conn_seg)
        *(.blec_llc_util_conn_seg)
        *(.blec_llm_conn_seg)
        *(.blec_llm_util_conn_seg)
        *(.blec_llc_llcp_conn_seg)
        *(.blec_llc_task_conn_seg)
        *(.blec_ea_conn_seg)
        *(.blec_lld_init_seg)

        . = ALIGN("BANK_ADDR" +0x2000*14);
        *(.bank14_seg)
        *(.echo_init_seg)
        *(.fdnrev_init_seg)
        *(.sdpro_init_seg)
        *(.spcommon_xfilter_init_seg)
        *(.mp2_init_seg)
        *(.fs_init_seg)
        *(.tsps_init_seg)
        *(.crossfade_init_seg)
        *(.autotune_init_seg)
        *(.nhs_opt_seg)
        *(.pitch_init_seg)
        *(.bth_rfcomm_pbap)
        *(.bth_rfcomm_cononect_seg)
        *(.blec_llc_task_hci_seg)
        *(.sco_audio_flash_seg_ex)

		*(.upload_eq_seg)
        *(.basic_afx_api_seg)
        *(.eqbuf_controller_seg)
		*(.ble_app_flash_seg)
		
        . = ALIGN("BANK_ADDR" +0x2000*15);
        *(.bank15_seg)
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
        *(.eid_flash_seg)

        *(.uart_init_seg)
        *(.uart0_init_seg)
        *(.uart1_init_seg)
        *(.uart2_init_seg)
        *(.uart_dma_init_seg)
        *(.motor_flash_seg)

        *(.decorr_init_seg)
        *(.nhs_init_seg)
        *(.eq_ram_init_seg)

        . = ALIGN("BANK_ADDR" +0x2000*16);
        *(.bank16_seg)
        *(.pcm_flash_seg)
        *(.mixer_flash_seg)
        *(.i2s_flash_seg)
        *hal_i2s.o(.rodata.CSWTCH*)

         . = ALIGN("BANK_ADDR" +0x2000*17);
        *(.bank17_seg)
        //*(.sco_audio_sram_seg)
        *(.btaudio_sram_seg)
        *(.audio_clksync_sram_seg)
        *(.sbc_auxtrack_sram_seg)
        *(.sbc_auxtrack_rodata_sram_seg)
        *(.mp3_auxtrack_sram_seg)
        *(.auxtrack_mixer_sram_seg)
        *(.auxtrack_mixer_rodata_sram_seg)
		*(.ble_flash_seg)
		*(.tables_rodata_seg)
		
		*(.audio_effects_seg)
		*(.usbaudio_sram_seg)
		*(.mp3_parser_sram_seg)
        *(.mp3_parser_sram_rodata_seg)
         . = ALIGN("BANK_ADDR" +0x2000*18);
        *(.bank18_seg)
        *(.afx_api_test_func_seg)
        *(.high_lvl_api_echo_seg)
        *(.high_lvl_api_revb_seg)
        *(.high_lvl_api_tsps_fc_seg)
        *(.high_lvl_api_eq_seg)
        *(.high_lvl_api_eq5_seg)
        *(.high_lvl_api_vss_seg)
        *(.high_lvl_drc_seg)
        *(.high_lvl_api_atune_seg)
        *(.audio_afx_async_setting_seg)
        *(.audio_afx_setting_seg)
        *(.audio_afx_eq_setting_seg)
        *(.audio_afx_drc_setting_seg)
        *(.audio_afx_echo_setting_seg)
        *(.audio_afx_revb_setting_seg)
        *(.audio_afx_tsps_setting_seg)
        *(.audio_afx_vss_setting_seg)
        *(.audio_afx_dr_setting_seg)
        *spcommon.o(.text*)
        *spcommon.o(.rodata*)

        . = ALIGN("BANK_ADDR" +0x2000*19);
        *(.bank19_seg)
        *(.audio_clksync_flash_seg)
        *(.lineinaudio_flash_seg)
        *(.metadata_flash_seg)
        *(.usbaudio_flash_seg)
        *(.metadata_rodata_flash_seg)
        *(.hwasrc_flash_seg)
        *(.tone_mixer_flash_seg)
        *(.sbc_auxtrack_flash_seg)
        *(.mp3_auxtrack_flash_seg)
        *(.auxtrack_mixer_flash_seg)
        *(.xfs_flash_seg)
        *(.xfs_flash_sd_seg)
        *(.xfs_flash_usb_seg)
        *(.xfs_diskio_flash_seg)
        *(.xfs_diskio_flash_usb_seg)
        *(.xfs_diskio_flash_sd_seg)

        *(.karaoke_flash_seg)
        *(.stream_flash_seg)
        *(.fatfs_stream_flash_seg)
        *(.tonefs_stream_flash_seg)
        *(.audiorecorder_flash_seg)
        *(.bitstream_store_flash_seg)
        *(.spi_rec_flash_seg)
        *(.fcc_datadft_seg)
        *(.dio_flash_seg)
        *(.audio_heap_flash_seg)
        *(.sd_flash_seg)
        *(.*audio_src_flash_seg)
        *(.*audio_sink_flash_seg)

        //*(.recorder_ain_sram_seg)
        //*(.fatfs_stream_sram_seg)
        *(.tonefs_stream_sram_seg)
        //*(.audiorecorder_sram_seg)
        //*(.bitstream_store_sram_seg)
        //*(.audioplayer_sram_seg)
        //*(.xfs_sram_seg)
        //*(.xfs_diskio_sram_seg)
        *(.lineinaudio_sram_seg)

        *(.mode_set_seg)
        *(.mode_set_const_seg)
        *(.mode_com_seg)

        *(.twi_seg)
        *(.sd_seg)
        *pmu.o(.rodata*)
        *(.pmu_rodata_seg)
        *(.pmu_sleep_seg)
        *(.pmu_bat_sram_seg)
        *(.pmu_rodata_seg)
        *(.pmu_init_seg)

        *(.clk_flash_seg)
        *hal_clk_lib.o(.rodata*)
        *hal_clk_lib.o(.rodata*)

        *(.spdif_seg)
        *(.audiopll_seg)

        *(.sleep_com_sram_seg)
        *(.sleep1_sram_seg)
        *(.sleep_btble_com_sram_seg)
        *(.sleep_btble_sram_seg)
        *(.sleep_bt_sram_seg)
        *(.sleep_sram_seg)

        *(.device_sram_seg)
        *(.bt_flash_seg)
        *(.default_int_seg)
        *vector_gcc.o(.rodata*)

        *(.afx_plug_seg)

        *(.rodata)
        *(.rodata*)
        *(.admfd_init_seg)

        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.glue_7)         /* glue arm to thumb code */
        *(.glue_7t)        /* glue thumb to arm code */
        *(.eh_frame)
        *(.eq_seg)
        *(.fc_init_seg)

        *(.bleh_api_seg)
        *(.bleh_stack_seg)
        *(.blec_sec_conn_seg)
        *(.blec_hci_seg)
        *(.rf_driver1_seg)

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

chk_lock                                            =0x0000d647;
enq_lock                                            =0x0000d6ad;
dir_remove                                          =0x0000e559;
dec_lock					    =0x0000d759;

id3_skip                                            =0x0004886b;
avpriv_mpegaudio_decode_header                      =0x0003b699;
mp3_parse_vbr_tags                                  =0x00045389;
mp3_vbr_xing_seek                                   =0x00044acd;
mp3_parser_getframe                                 =0x00044bb5;
mp3_parser_seek                                     =0x00044bdb;
mp3_parser_cmd                                      =0x00044d41;
mp3_parser_close                                    =0x00044d5f;
aac_decode_frame_int                                =0x00037659;

_init                                               =0x00000138;
_fini                                               =0x00000144;
__aeabi_drsub                                       =0x00000154;
__aeabi_dsub                                        =0x0000015c;
__subdf3                                            =0x0000015c;
__adddf3                                            =0x00000160;
__aeabi_dadd                                        =0x00000160;
__floatunsidf                                       =0x000003d8;
__aeabi_ui2d                                        =0x000003d8;
__floatsidf                                         =0x000003f8;
__aeabi_i2d                                         =0x000003f8;
__aeabi_f2d                                         =0x0000041c;
__extendsfdf2                                       =0x0000041c;
__aeabi_ul2d                                        =0x00000460;
__floatundidf                                       =0x00000460;
__floatdidf                                         =0x00000470;
__aeabi_l2d                                         =0x00000470;
__muldf3                                            =0x000004cc;
__aeabi_dmul                                        =0x000004cc;
__aeabi_ddiv                                        =0x00000720;
__divdf3                                            =0x00000720;
__aeabi_d2uiz                                       =0x000008f0;
__fixunsdfsi                                        =0x000008f0;
__aeabi_ldivmod                                     =0x00000930;
__aeabi_uldivmod                                    =0x000009d0;
__udivmoddi4                                        =0x00000a00;
__aeabi_ldiv0                                       =0x00000ccc;
__aeabi_idiv0                                       =0x00000ccc;
_start                                              =0x00000d10;
_mainCRTStartup                                     =0x00000d10;
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
ccu_init                                            =0x000019a8;
ccu_deinit                                          =0x000019e8;
HAL_SD_Reset                                        =0x00001a14;
HAL_SD_Reg_Init                                     =0x00001a26;
HAL_SD_MspInit                                      =0x00001ac0;
HAL_SD_Get_CardInfo                                 =0x00001aea;
HAL_SD_CmdConfig                                    =0x00001d5c;
HAL_SD_DataConfig                                   =0x00001d70;
HAL_SD_SendCmd                                      =0x00001d7e;
HAL_SD_GetRsp                                       =0x00001e2c;
HAL_SD_GetCmdRsp                                    =0x00001e36;
SD_CmdError                                         =0x00001e40;
SD_CmdResp7Error                                    =0x00001e66;
SD_Select_Deselect                                  =0x00001eb6;
SD_CmdResp3Error                                    =0x00001ece;
SD_PowerON                                          =0x00001efc;
SD_CmdResp2Error                                    =0x00001fd4;
SD_CmdResp6Error                                    =0x00002010;
SD_Initialize_Cards                                 =0x00002084;
SD_CmdResp1Error                                    =0x000027b4;
general_malloc_ex                                   =0x0080126c;
general_calloc_ex                                   =0x00801270;
general_malloc_max_ex                               =0x00801274;
general_calloc_max_ex                               =0x00801278;
general_free_ex                                     =0x0080127c;
general_malloc_stack_ex                             =0x00801280;
general_calloc_stack_ex                             =0x00801284;
general_free_stack_ex                               =0x00801288;
general_get_avaheapsize_ex                          =0x0080128c;
HAL_SD_Init                                         =0x00002116;
HAL_SD_DataInit                                     =0x00002254;
HAL_SD_DmaDescInit                                  =0x00002296;
HAL_SD_DmaInit                                      =0x000022ac;
HAL_SD_DmaKick                                      =0x00002324;
HAL_SD_ReadRestart                                  =0x00002354;
HAL_SD_ReadDo                                       =0x000023b0;
HAL_SD_ReadyForData                                 =0x000024b4;
HAL_SD_Stop                                         =0x000024f4;
HAL_SD_Read                                         =0x0000254e;
HAL_SD_Write                                        =0x000025f8;
HAL_SD_CheckRWOperation                             =0x00002694;
HAL_SD_IRQHandler                                   =0x00002736;
USB_EnableGlobalInt                                 =0x00002924;
USB_DisableGlobalInt                                =0x00002938;
USB_InitFSLSPClkSel                                 =0x0000324c;
RegisterIsrDmaProcessImpl                           =0x00003544;
HAL_HCD_Init                                        =0x00003550;
HAL_HCD_SOF_Callback                                =0x00003634;
HAL_HCD_Connect_Callback                            =0x00003644;
HAL_HCD_Disconnect_Callback                         =0x00003654;
HAL_HCD_HC_NotifyURBChange_Callback                 =0x00003668;
HAL_HCD_GetCurrentFrame                             =0x000036c8;
HAL_PCD_IRQHandler                                  =0x00003868;
USBH_CtlSendSetup                                   =0x00003f24;
USBH_CtlSendData                                    =0x00003f40;
USBH_CtlReceiveData                                 =0x00003f5a;
USBH_CtlReq                                         =0x000041a4;
USBH_SetCfg                                         =0x00004474;
USBH_ClrFeature                                     =0x000044bc;
USBH_OpenPipe                                       =0x000044e0;
USBH_ClosePipe                                      =0x000044f4;
USBH_AllocPipe                                      =0x000044f8;
USBH_FreePipe                                       =0x00004526;
DeInitStateMachine                                  =0x0000453c;
USBH_Init                                           =0x00004584;
USBH_RegisterClass                                  =0x000045e0;
USBH_SelectInterface                                =0x000045e6;
USBH_FindInterface                                  =0x00004610;
USBH_Start                                          =0x0000468c;
USBH_Stop                                           =0x00004690;
USBH_HandleEnum                                     =0x000046c8;
USBH_Process                                        =0x000047e0;
USBH_LL_SetTimer                                    =0x0000495c;
USBH_MSC_BOT_REQ_Reset                              =0x000049c8;
USBH_MSC_BOT_REQ_GetMaxLUN                          =0x000049e0;
USBH_MSC_BOT_Init                                   =0x000049fc;
USBH_MSC_DecodeCSW									=0x00004a74;
USBH_MSC_Process                                    =0x00004b6c;
USBH_RegisterPreprocessImpl                         =0x00004f40;
USBH_RegisterDmaCallbackImpl                        =0x00004f4c;
USBH_MSC_GetLUNInfo                                 =0x00004fa8;
USBH_MSC_Read_dma                                   =0x00005048;
USBH_MSC_Write_dma                                  =0x0000508c;
Bulk_Out_Transfer_Complete_Process                  =0x00005174;
USBH_MSC_SCSI_TestUnitReady                         =0x00005458;
USBH_MSC_SCSI_ReadCapacity                          =0x000054ac;
USBH_MSC_SCSI_Inquiry                               =0x00005520;
USBH_MSC_SCSI_Read_dma                              =0x00005680;
USB_disk_write                                      =0x00005788;
USB_disk_ioctl                                      =0x000057d4;
USB_disk_hdl_register                               =0x00005824;
HAL_HCD_MspInit                                     =0x00005830;
USBH_LL_Init                                        =0x00005858;
USBH_LL_GetSpeed                                    =0x000058c8;
USBH_LL_ResetPort                                   =0x000058cc;
USBH_LL_SetToggle                                   =0x000058e8;
USBH_LL_GetToggle                                   =0x00005908;
USBH_Delay                                          =0x00005928;
USBD_CtlSendData                                    =0x0000592c;
USBD_CtlPrepareRx                                   =0x00005952;
USBD_CtlError                                       =0x00005cd4;
USBD_GetString                                      =0x00005ce6;
USBD_Init                                           =0x00005d20;
USBD_DeInit                                         =0x00005d40;
USBD_RegisterClass                                  =0x00005d68;
USBD_Start                                          =0x00005d6e;
USBD_Stop                                           =0x00005d72;
COMP_MSC_BOT_SendCSW                                =0x00006078;
USBD_COMP_EP0_RxReady                               =0x000062cc;
USBD_COMP_MSC_Init                                  =0x00006330;
USBD_COMP_HID_Setup                                 =0x00006360;
USBD_COMP_MSC_Setup                                 =0x0000641c;
USBD_COMP_MSC_DataIn                                =0x000064cc;
USBD_COMP_Init                                      =0x00006570;
USBD_COMP_MSC_DeInit                                =0x0000663c;
USBD_COMP_MSC_RegisterStorage                       =0x00006760;
USBD_COMP_Setup                                     =0x00006920;
USBD_COMP_AUDIO_RegisterInterface                   =0x00006966;
USBD_COMP_HID_SendReport                            =0x00006970;
Register_HID_Callback                               =0x000069a0;
USBD_COMP_REG_DESC                                  =0x000069ac;
COMP_SCSI_FW_READ                                   =0x00006a58;
USBD_LL_OpenEP                                      =0x000071f4;
USBD_LL_CloseEP                                     =0x000071fe;
USBD_LL_FlushEP                                     =0x00007202;
USBD_LL_Transmit                                    =0x0000723c;
USBD_LL_PrepareReceive                              =0x00007240;
COMP_STORAGE_IsReady                                =0x00007250;
COMP_STORAGE_GetMaxLun                              =0x00007257;   /* thumbObject */
USB_WritePacket                                     =0x0000725c;
USB_ReadPacket                                      =0x00007284;
USBH_BulkSendData                                   =0x000076e4;
USBH_BulkReceiveData                                =0x00007702;
USBH_MSC_Read                                       =0x0000796c;
USBH_MSC_SCSI_RequestSense                          =0x000079dc;
USB_disk_status                                     =0x00007b00;
USB_disk_read                                       =0x00007b1c;
USBH_LL_GetLastXferSize                             =0x00007b5c;
USBH_LL_GetURBState                                 =0x00007b74;
USB_ReadInterrupts                                  =0x00007b90;
USB_GetMode                                         =0x00007ba0;
USB_HC_ReadInterrupt                                =0x00007bb0;
USB_HC_Halt                                         =0x00007bd0;
HAL_HCD_IRQHandler                                  =0x0000808a;
HAL_NORF_CLK_Change                                 =0x00008522;
write_irq_set_region_addr                           =0x0000853e;
write_irq_enable_region_irq                         =0x00008574;
write_irq_is_region_pending                         =0x00008596;
write_irq_get_addr                                  =0x000085c4;
write_irq_get_pc                                    =0x000085d6;
write_irq_get_sr                                    =0x000085e8;
CMP_UPG_Nor_Init                                    =0x000085f4;
CMP_UPG_Norf_Read                                   =0x0000865a;
CMP_UPG_Nor_GetCRC                                  =0x00008666;
CMP_UPG_Norf_Write                                  =0x000086b2;
CMP_UPG_Norf_Comm                                   =0x000086ee;
nor_irq_process                                     =0x00008705;   /* thumbObject */
nor_init                                            =0x0000873c;
nor_config_change                                   =0x000087d0;
nor_rw_common                                       =0x00008800;
nor_read                                            =0x00008844;
nor_if_busy                                         =0x000088c0;
nor_write                                           =0x0000893c;
nor_erase                                           =0x00008970;
nor_crc_enable                                      =0x000089a4;
nor_crc_disable                                     =0x000089bc;
nor_workmode_change                                 =0x000089fc;
xTimerCreateTimerTask                               =0x00008bc8;
xTimerCreate                                        =0x00008c0c;
xTimerGenericCommand                                =0x00008c46;
pcTimerGetTimerName                                 =0x00008e60;
xTimerIsTimerActive                                 =0x00008e64;
pvTimerGetTimerID                                   =0x00008e7c;
xTimerPendFunctionCallFromISR                       =0x00008e80;
xTimerPendFunctionCall                              =0x00008ea8;
xTaskGenericCreate                                  =0x00008ed0;
vTaskDelete                                         =0x00009084;
vTaskStartScheduler                                 =0x00009130;
vTaskDelayUntil                                     =0x00009198;
xEventGroupCreate                                   =0x00009208;
xEventGroupWaitBits                                 =0x00009222;
xEventGroupClearBits                                =0x000092d8;
xEventGroupGetBitsFromISR                           =0x000092f2;
xEventGroupSetBits                                  =0x0000930e;
xEventGroupSync                                     =0x0000936a;
vEventGroupDelete                                   =0x000093f4;
vEventGroupSetBitsCallback                          =0x0000941a;
vEventGroupClearBitsCallback                        =0x0000941e;
vListInitialise                                     =0x00009424;
xQueueGenericReset                                  =0x0000943c;
xQueueGenericCreate                                 =0x000094a8;
xQueueCreateCountingSemaphore                       =0x000094e2;
xQueueCreateMutex                                   =0x000094f4;
xQueueAltGenericSend                                =0x0000953a;
xQueueAltGenericReceive                             =0x000095f4;
xQueueGiveFromISR                                   =0x000096dc;
xQueueReceiveFromISR                                =0x00009744;
xQueuePeekFromISR                                   =0x000097a0;
uxQueueMessagesWaiting                              =0x000097d0;
uxQueueSpacesAvailable                              =0x000097e2;
uxQueueMessagesWaitingFromISR                       =0x000097f8;
vQueueDelete                                        =0x000097fc;
xQueueIsQueueEmptyFromISR                           =0x00009800;
xQueueIsQueueFullFromISR                            =0x0000980a;
xQueueCreateSet                                     =0x00009816;
xQueueAddToSet                                      =0x0000981e;
xQueueRemoveFromSet                                 =0x00009840;
xQueueSelectFromSet                                 =0x00009860;
xQueueSelectFromSetFromISR                          =0x00009878;
inHandlerMode                                       =0x00009890;
osMessageCreate                                     =0x0000989c;
osMessagePut                                        =0x000098a6;
osMessageGet                                        =0x0000990c;
osQueueSend                                         =0x000099a0;
osQueueReceive                                      =0x000099fc;
osSemaphoreGive                                     =0x00009a54;
osSemaphoreTake                                     =0x00009a94;
osTaskNotify                                        =0x00009af8;
vAssertCalled                                       =0x00009b34;
vApplicationIdleHook                                =0x00009b48;
vApplicationTickHook                                =0x00009b5c;
vApplicationMallocFailedHook                        =0x00009b70;
vApplicationStackOverflowHook                       =0x00009b84;
vPreSleepHook                                       =0x00009b98;
vPostSleepHook                                      =0x00009bac;
pxPortInitialiseStack                               =0x00009bec;
vPortEndScheduler                                   =0x00009c10;
vPortSuppressTicksAndSleep                          =0x00009c12;
vPortEnableVFP                                      =0x00009d90;
vPortSetSystickClkHz                                =0x00009de0;
prvHeapConfig                                       =0x00009ebc;
pvPortMalloc                                        =0x00009ee4;
vPortFree                                           =0x00009fd0;
xPortGetFreeHeapSize                                =0x0000a01c;
xPortGetMinimumEverFreeHeapSize                     =0x0000a028;
vPortInitialiseBlocks                               =0x0000a034;
eTaskGetState                                       =0x0000a0dc;
uxTaskPriorityGet                                   =0x0000a13c;
uxTaskPriorityGetFromISR                            =0x0000a158;
vTaskPrioritySet                                    =0x0000a180;
vTaskResume                                         =0x0000a244;
xTaskResumeFromISR                                  =0x0000a2cc;
vTaskEndScheduler                                   =0x0000a35c;
vTaskSuspendAll                                     =0x0000a37c;
xTaskGetTickCount                                   =0x0000a38c;
xTaskGetTickCountFromISR                            =0x0000a398;
uxTaskGetNumberOfTasks                              =0x0000a3a4;
pcTaskGetTaskName                                   =0x0000a3b0;
xTaskGetIdleTaskHandle                              =0x0000a3c0;
vTaskStepTick                                       =0x0000a3cc;
xTaskIncrementTick                                  =0x0000a3dc;
xTaskResumeAll                                      =0x0000a4f8;
vTaskDelay                                          =0x0000a5cc;
prvIdleTask                                         =0x0000a62c;
vTaskSwitchContext                                  =0x0000a6ac;
vTaskSuspend                                        =0x0000a73c;
vTaskPlaceOnEventList                               =0x0000a7ec;
vTaskPlaceOnUnorderedEventList                      =0x0000a844;
vTaskPlaceOnEventListRestricted                     =0x0000a8a8;
xTaskRemoveFromEventList                            =0x0000a8ec;
xTaskRemoveFromUnorderedEventList                   =0x0000a960;
vTaskSetTimeOutState                                =0x0000a9bc;
xTaskCheckForTimeOut                                =0x0000a9d4;
vTaskMissedYield                                    =0x0000aa20;
eTaskConfirmSleepModeStatus                         =0x0000aa2c;
xTaskGetCurrentTaskHandle                           =0x0000aa4c;
xTaskGetSchedulerState                              =0x0000aa58;
vTaskPriorityInherit                                =0x0000aa78;
xTaskPriorityDisinherit                             =0x0000ab08;
uxTaskResetEventItemValue                           =0x0000ab78;
pvTaskIncrementMutexHeldCount                       =0x0000ab90;
ulTaskNotifyTake                                    =0x0000aba8;
xTaskNotifyWait                                     =0x0000ac4c;
xTaskNotify                                         =0x0000ad18;
xTaskNotifyFromISR                                  =0x0000adb4;
vTaskNotifyGiveFromISR                              =0x0000ae6c;
puTaskStackAddr                                     =0x0000af00;
pcTaskName                                          =0x0000af0c;
vListInitialiseItem                                 =0x0000af58;
vListInsertEnd                                      =0x0000af5e;
vListInsert                                         =0x0000af76;
uxListRemove                                        =0x0000afa4;
xQueueGenericSend                                   =0x0000b110;
xQueueGenericSendFromISR                            =0x0000b21c;
xQueueGenericReceive                                =0x0000b292;
vQueueWaitForMessageRestricted                      =0x0000b3b4;
vPortSVCHandler                                     =0x0000b3ed;   /* thumbObject */
vPortEnterCritical                                  =0x0000b40c;
vPortExitCritical                                   =0x0000b42c;
xPortPendSVHandler                                  =0x0000b441;   /* thumbObject */
xPortSysTickHandler                                 =0x0000b4a1;   /* thumbObject */
disk_initialize                                     =0x0000b4d0;
disk_write                                          =0x0000b4f8;
disk_ioctl                                          =0x0000b52c;
disk_config                                         =0x0000b55c;
f_mount                                             =0x0000b94c;
f_read                                              =0x0000b9d4;
//f_write                                           =0x0000bb88;
//f_sync                                            =0x0000bd7e;
//f_close                                           =0x0000bed2;
f_lseek                                             =0x0000bf04;
f_closedir                                          =0x0000c2bc;
f_readdir                                           =0x0000c4f8;
//f_open                                            =0x0000c5f8;
//f_opendir                                         =0x0000c920;
//f_unlink                                          =0x0000c9f8;
f_setcp                                             =0x0000ce7a;
f_getfree					    =0x0000c2eb;              
sync_window                                         =0x0000d7b5;
follow_path                                         =0x0000ef7d;
//create_chain                                      =0x0000dd72;
st_clust                                            =0x0000e12b;
dir_register                                        =0x0000eb0f;
store_xdir                                          =0x0000e415;
sync_fs                                             =0x0000d82f;
remove_chain                                        =0x0000dc79;
find_volume                                         =0x0000b581;  
//get_ldnumber					    =0x0000e5b4;
//FatFs						    =0x008011dc;
//Fsid						    =0x008011e4; 
//lock_fs					    =0x0000d62a;
//check_fs					    =0x0000e5ee;
//clear_lock					    =0x0000d794;
check_file_name                                     =0x0000cea1;   /* thumbObject */
into_dir                                            =0x0000cefd;   /* thumbObject */
into_borther_item                                   =0x0000d021;   /* thumbObject */
xfs_get_fileinfo                                    =0x0000d10b;   /* thumbObject */
disk_status                                         =0x0000d4b4;
disk_read                                           =0x0000d4d4;
unlock_fs                                           =0x0000d631;   /* thumbObject */
inc_lock                                            =0x0000d6c9;   /* thumbObject */
move_window                                         =0x0000d7f9;   /* thumbObject */
clst2sect                                           =0x0000d8c5;   /* thumbObject */
get_fat                                             =0x0000d8db;   /* thumbObject */
find_bitmap                                         =0x0000db0b;
change_bitmap					    =0x0000db9d;
clmt_clust                                          =0x0000dee5;   /* thumbObject */
dir_sdi                                             =0x0000df67;   /* thumbObject */
ld_clust                                            =0x0000e10d;   /* thumbObject */
dir_read                                            =0x0000e47b;   /* thumbObject */
validate                                            =0x0000e67d;   /* thumbObject */
dir_find                                            =0x0000e9ab;   /* thumbObject */
HAL_DMA_Init                                        =0x0000f054;
HAL_DMA_DeInit                                      =0x0000f07c;
HAL_DMA_Abort                                       =0x0000f104;
HAL_DMA_IRQHandler                                  =0x0000f11c;
timestamp_init                                      =0x0000f178;
us_to_timestamp                                     =0x0000f198;
ms_to_timestamp                                     =0x0000f19c;
timestamp_to_us                                     =0x0000f1a6;
timestamp_to_ms                                     =0x0000f1a8;
get_timestamp                                       =0x0000f1b6;
set_timestamp                                       =0x0000f1ec;
cdelay                                              =0x0000f204;
udelay                                              =0x0000f212;
mdelay                                              =0x0000f22e;
msleep                                              =0x0000f250;
HAL_IWDG_Init                                       =0x0000f274;
HAL_IWDG_DeInit                                     =0x0000f2a0;
HAL_IWDG_Refresh                                    =0x0000f2ac;
HAL_TRNG_Init                                       =0x0000f2c0;
HAL_TRNG_DeInit                                     =0x0000f2c4;
HAL_TRNG_GetRandomNumber                            =0x0000f2c8;
lc_acc_sync_con_handler                             =0x0000f399;
lc_send_lmp                                         =0x0000ffa0;
lc_send_pdu_not_acc                                 =0x0001015c;
lc_send_pdu_not_acc_ext4                            =0x0001019c;
lc_send_pdu_encaps_payl                             =0x000103e0;
lc_send_pdu_io_cap_res_entiy                        =0x00010829;   /* thumbObject */
lc_lmp_acc_handler                                  =0x00010acc;
lc_lmp_detach_handler                               =0x00010f59;   /* thumbObject */
lc_lmp_io_cap_req_handler                           =0x000111fc;
lc_acl_data_req_handler                             =0x00017549;   /* thumbObject */
lc_op_feat_ind_handler                               =0x000182ad;
lc_lmp_acc_handler_entity                           =0x0001837f;   /* thumbObject */
lc_lmp_sniff_req_handler_entity                     =0x00018b1d;   /* thumbObject */
lc_chg_pkt_type_cont                                =0x00019098;
lc_common_cmd_status_send                           =0x000191d6;
lc_common_nb_of_pkt_comp_evt_send                   =0x000191ea;
lc_con_cmp_evt_send                                 =0x0001920e;
lc_free_tx_acl_buf                                  =0x00019488;
lc_local_switch                                     =0x000196bc;
lc_mst_qos_done                                     =0x000197bc;
lc_enter_hold_mode                                  =0x00019c20;
lc_start_entity                                     =0x00019cf9;   /* thumbObject */
lc_start_lmp_to                                     =0x00019dd0;
lc_enter_sniff_mode                                 =0x00019fcc;
lc_start_enc                                        =0x0001a554;
lc_start_enc_key_size                               =0x0001a6d0;
lc_start_key_exch                                   =0x0001a738;
lc_stop_enc                                         =0x0001a7c8;
lc_restart_enc_cont_entity                          =0x0001a98d;   /* thumbObject */
lc_semi_key_cmp                                     =0x0001aba4;
lc_rem_sniff                                        =0x0001b978;
lc_switch_cmp                                       =0x0001bdf4;
lc_enc_cmp                                          =0x0001bfe0;
lc_epr_cmp                                          =0x0001c238;
lc_sp_fail                                          =0x0001c418;
lc_calc_link_key                                    =0x0001c484;
lc_dhkey                                            =0x0001c5e8;
lc_resp_pair_entity                                 =0x0001ca4d;   /* thumbObject */
lc_scatt_init                                       =0x0001cbec;
lc_util_rsw_en                                      =0x0001ce0c;
lc_util_set_loc_trans_coll                          =0x0001ce44;
ld_ack_received                                     =0x0001d07c;
ld_ack_received_entity                              =0x0001d08d;   /* thumbObject */
ld_next_acl_tx                                      =0x0001d2b4;
ld_next_acl_tx_entity                               =0x0001d2c5;   /* thumbObject */
ld_rx_sync                                          =0x0001d51c;
ld_rx_sync_entity                                   =0x0001d529;   /* thumbObject */
ld_data_rx                                          =0x0001d734;
ld_data_rx_entity                                   =0x0001d751;   /* thumbObject */
ld_rx_clock_entity                                  =0x0001d8cd;   /* thumbObject */
ld_rx_isr                                           =0x0001e608;
ld_clock_isr                                        =0x0001e624;
ld_acl_data_tx                                      =0x0001f30c;
ld_acl_flow_check                                   =0x0001f610;
co_list_init                                        =0x0001f620;
co_list_push_back                                   =0x0001f628;
co_list_push_front                                  =0x0001f63c;
co_list_pop_front                                   =0x0001f648;
co_list_extract                                     =0x0001f658;
co_list_extract_after                               =0x0001f68e;
co_list_find                                        =0x0001f6aa;
co_list_merge                                       =0x0001f6bc;
co_list_insert_after                                =0x0001f6f8;
co_list_size                                        =0x0001f71a;
co_alloc_buf                                        =0x0001f72c;
co_alloc_buf_entity                                 =0x0001f739;   /* thumbObject */
co_free_buf                                         =0x0001f75a;
co_free_buf_entity                                  =0x0001f765;   /* thumbObject */
co_buf_init                                         =0x0001f7a6;
co_alloc_acl_buf_tx                                 =0x0001f834;
co_alloc_acl_buf_rx                                 =0x0001f848;
co_alloc_acl_buf_rx_entity                          =0x0001f855;   /* thumbObject */
co_free_acl_buf                                     =0x0001f868;
co_slot_to_duration                                 =0x0001f8a8;
co_bdaddr_compare                                   =0x0001f8b2;
co_conhdl_init                                      =0x0001f8d2;
co_conhdl_bdaddr_to_tid                             =0x0001f958;
co_conhdl_type_get                                  =0x0001f9cc;
co_conhdl_to_tid                                    =0x0001f9dc;
ke_task_schedule                                    =0x0001fa45;   /* thumbObject */
ke_task_create                                      =0x0001fb0c;
ke_state_set                                        =0x0001fb9c;
ke_state_get                                        =0x0001fc20;
ke_msg_alloc                                        =0x0001fc5c;
ke_msg_alloc_entity                                 =0x0001fc71;   /* thumbObject */
ke_msg_send                                         =0x0001fca0;
ke_msg_send_entity                                  =0x0001fcad;   /* thumbObject */
ke_msg_send_basic                                   =0x0001fcd0;
ke_msg_forward_entity                               =0x0001fced;   /* thumbObject */
ke_msg_free                                         =0x0001fd0e;
ke_flush                                            =0x0001fd20;
ke_sleep_check                                      =0x0001fd68;
ke_timer_hw_set_entity                              =0x0001fdc9;   /* thumbObject */
ke_timer_schedule                                   =0x0001fdfd;   /* thumbObject */
ke_timer_set                                        =0x0001fe5c;
ke_timer_set_entity                                 =0x0001fe69;   /* thumbObject */
ke_timer_clear                                      =0x0001ff0c;
ke_timer_clear_entity                               =0x0001ff19;   /* thumbObject */
ke_timer_active                                     =0x0001ff7c;
ke_timer_active_entity                              =0x0001ff89;   /* thumbObject */
ke_event_init                                       =0x00020014;
ke_event_schedule                                   =0x00020024;
ke_event_set                                        =0x00020030;
ke_event_callback_set                               =0x0002003c;
ke_event_clear                                      =0x00020054;
lm_util_get_offset_clkn                             =0x00020134;
lm_util_get_offset_clke                             =0x0002014a;
lm_util_add_event                                   =0x00020160;
lm_util_delete_event                                =0x000203c0;
lm_util_init_events                                 =0x000203f8;
LM_UpdateAclPacketType                              =0x0002043c;
LM_UpdateAclEdrPacketType                           =0x00020484;
LM_CreateUnitKey                                    =0x000206d4;
LM_GetFeature                                       =0x000209cc;
LM_CheckTransactionID                               =0x00020aa0;
lm_lmp_rx_ind_handler_entity                        =0x00020be9;   /* thumbObject */
lm_sp_get_pairing_mode                              =0x00020ddc;
lm_get_encap_pdu_data                               =0x00020dfc;
lm_get_nonce                                        =0x00020fdc;
lm_init_sp                                          =0x00021054;
lm_sp_32bits_to_array                               =0x00021198;
lm_get_oob_local_commit                             =0x000211b0;
lm_oob_f1                                           =0x0002135c;
lm_g1                                               =0x00021372;
lm_dhkey_calc_init                                  =0x000217c8;
lm_sp_dhkey_get_buffer_idx                          =0x00021978;
lm_page_scan_to_handler                             =0x000219fd;
lm_initlowpow                                       =0x00021b68;
lm_common_cmd_complete_send                         =0x0002209c;
lc_lmp_sco_lk_rem_req_handler                       =0x000222f5;   /* thumbObject */
lc_lmp_esco_lk_rem_req_handler                      =0x000222c1;   /* thumbObject */
lc_sco_start_link_entity                            =0x0002293d;   /* thumbObject */
lc_sco_modify_link_entity                           =0x00022a71;   /* thumbObject */
lc_sco_stop_link_entity                             =0x00022b2d;   /* thumbObject */
lc_sco_init                                         =0x00022b32;
lc_sco_reset                                        =0x00022b4c;
lc_sco_host_accept                                  =0x00022de8;
lc_sco_peer_accept                                  =0x00023390;
lc_sco_peer_request_disc                            =0x000234e4;
lc_sco_peer_accept_disc                             =0x000235ac;
ld_access_init_cs_access_entity                     =0x00023da5;   /* thumbObject */
ld_access_f_entity                                  =0x00025619;   /* thumbObject */
ld_access_rx_access_entity                          =0x00025681;   /* thumbObject */
ld_access_setup_complete                            =0x000256e4;
ld_util_lmp_tx_entity                               =0x0002587d;   /* thumbObject */
ld_util_disable_power_cntl                          =0x00025998;
ld_util_get_nb_acl                                  =0x00025a04;
ld_util_get_min_poll_interval                       =0x00025a68;
ld_util_poll_int_update                             =0x00025b14;
lm_get_sync_offset_entity                           =0x00026681;   /* thumbObject */
lm_get_allowed_intv_bitmask_entity                  =0x0002675d;   /* thumbObject */
lm_get_sync_param_hl_req_entity                     =0x00026da9;   /* thumbObject */
lm_get_sync_param_entity                            =0x00027d71;   /* thumbObject */
lm_init_sync                                        =0x00027ebc;
ld_afh_init                                         =0x000286f8;
ld_afh_hssi_switch_instant_entity                   =0x00028fb5;   /* thumbObject */
ld_afh_implicit_disabling                           =0x0002918c;
ld_afh_implicit_enabling                            =0x00029190;
ld_lowpow_unsniff                                   =0x000294b0;
ld_init_link                                        =0x000295b8;
ld_init_link_entity                                 =0x000295c5;   /* thumbObject */
ld_init_CQDDR                                       =0x00029900;
ld_init_CQDDR_entity                                =0x0002990d;   /* thumbObject */
ld_compute_tx_type_entity                           =0x00029975;   /* thumbObject */
ld_cqddr_new                                        =0x00029a30;
ld_cqddr_new_entity                                 =0x00029a3d;   /* thumbObject */
ld_lowpowit_compute_sync_window                     =0x00029cd4;
ld_lowpowit_compute_sync_window_entity              =0x00029ce9;   /* thumbObject */
ld_lowpowit_manage_entity                           =0x0002a00d;   /* thumbObject */
ld_pbf_enhanced_flush_cs_tx                         =0x0002a86c;
ld_util_read_features                               =0x0002ab4c;
ld_util_alloc_lt_addr                               =0x0002ab5c;
ld_util_free_lt_addr                                =0x0002ab84;
ld_util_set_bd_addr                                 =0x0002aba0;
ld_util_set_poll_interval                           =0x0002ae94;
ld_util_get_master_clock                            =0x0002af38;
ld_util_get_cs_clock_offset                         =0x0002af68;
ld_util_dec_power_requested                         =0x0002b0bc;
ld_util_inc_power_requested                         =0x0002b104;
ld_util_test_mode_cntl_entity                       =0x0002b281;   /* thumbObject */
ld_util_set_sp_feature_bit                          =0x0002b73c;
ld_util_check_rsw_act                               =0x0002b77c;
ld_switch_req                                       =0x0002b7f4;
ld_switch_start_entity                              =0x0002b8dd;   /* thumbObject */
ld_acl_init_fifo                                    =0x0002c360;
ld_acl_flow_control                                 =0x0002c3fc;
ld_aclit_disc_link                                  =0x0002c490;
ld_aclit_start_acl_flush_timer                      =0x0002cad4;
ld_aclit_set_next_acl_entity                        =0x0002cb51;   /* thumbObject */
GLOBAL_INT_DISABLE                                  =0x0002cf28;
GLOBAL_INT_RESTORE                                  =0x0002cf34;
co_rand_word                                        =0x0002cf40;
ke_malloc                                           =0x0002cf4c;
ke_free                                             =0x0002cf58;
ld_sco_init                                         =0x0002d074;
ld_sco_configure_con_entity                         =0x0002d229;   /* thumbObject */
ld_sco_manage_entity                                =0x0002d849;   /* thumbObject */
lm_inq_scan_to_handler                              =0x000314d9;
lld_util_get_bd_address                             =0x00031688;
lld_util_set_bd_address                             =0x000316b0;
lld_util_ral_force_rpa_renew                        =0x000316f4;
lld_util_freq2chnl                                  =0x00031738;
lld_util_get_local_offset                           =0x00031758;
lld_util_get_peer_offset                            =0x0003176e;
lld_util_connection_param_set                       =0x00031786;
lld_util_dle_set_cs_fields                          =0x000317ca;
lld_util_anchor_point_move                          =0x000317ec;
lld_util_flush_list                                 =0x000318dc;
lld_data_rx_check                                   =0x00031926;
lld_data_rx_flush                                   =0x00031974;
lld_data_tx_check                                   =0x000319a0;
lld_data_tx_loop                                    =0x00031a1c;
lld_data_tx_push                                    =0x00031a54;
lld_data_tx_prog                                    =0x00031aa4;
lld_data_tx_flush                                   =0x00031b40;
lld_adv_start                                       =0x00031b70;
lld_adv_stop                                        =0x00031d28;
lld_scan_start                                      =0x00031d54;
lld_scan_stop                                       =0x00031ed8;
lld_con_start                                       =0x00031f04;
lld_move_to_master                                  =0x00032410;
lld_con_update_req                                  =0x00032448;
lld_con_update_after_param_req                      =0x000324d0;
lld_con_param_rsp                                   =0x00032720;
lld_con_param_req                                   =0x00032830;
lld_con_stop                                        =0x00032908;
lld_get_mode                                        =0x00032932;
lld_move_to_slave                                   =0x0003295c;
lld_ch_map_ind                                      =0x00032aa8;
lld_con_update_ind                                  =0x00032aca;
lld_crypt_isr                                       =0x00032ae0;
lld_ral_renew_req                                   =0x00032ae6;
em_buf_rx_free                                      =0x00032b00;
em_buf_rx_buff_addr_get                             =0x00032b18;
em_buf_tx_buff_addr_get                             =0x00032b24;
em_buf_tx_alloc                                     =0x00032b30;
em_buf_tx_free                                      =0x00032bb0;
lld_evt_prevent_stop                                =0x00032d50;
lld_evt_schedule                                    =0x00032dcc;
lld_evt_deferred_elt_pop                            =0x00032ec8;
lld_evt_init_evt                                    =0x00032f30;
lld_evt_elt_delete                                  =0x00032f60;
lld_evt_conhdl2elt                                  =0x0003314c;
lld_evt_drift_compute                               =0x0003316c;
lld_evt_elt_insert                                  =0x00033654;
lld_evt_canceled                                    =0x00033690;
lld_evt_schedule_next                               =0x000336bc;
lld_evt_scan_create                                 =0x000337a0;
lld_evt_move_to_master                              =0x00033830;
lld_evt_update_create                               =0x00033950;
lld_evt_ch_map_update_req                           =0x00033a78;
lld_evt_move_to_slave                               =0x00033a90;
lld_evt_slave_update                                =0x00033d44;
lld_evt_adv_create                                  =0x00033e04;
lld_evt_deffered_elt_handler                        =0x00033eac;
lld_evt_end                                         =0x00033f8e;
lld_evt_rx                                          =0x000340fc;
sp_calloc                                           =0x00034510;
sp_free                                             =0x00034520;
sp_realfft                                          =0x0005f7b0;
sp_realifft                                         =0x0005f8bc;
sp_cos_twiddle                                      =0x0006590c;
sp_sin_twiddle                                      =0x0006694c;
sp_sincos                                           =0x0005fae5;
general_imdct                                       =0x00034f20;
general_log2                                        =0x00035080;
general_pow2                                        =0x000350c0;
general_exp                                         =0x00035164;
general_sincos                                      =0x00035268;
general_inc_mem                                     =0x00035288;
general_memconfig                                   =0x000352a0;
general_malloc                                      =0x000352c8;
general_calloc                                      =0x00035300;
general_malloc_max                                  =0x0003532c;
general_calloc_max                                  =0x00035354;
general_free                                        =0x0003537c;
general_malloc_stack                                =0x0003538c;
general_calloc_stack                                =0x000353c4;
general_free_stack                                  =0x000353f0;
general_get_avaheapsize                             =0x00035408;
aac_check_enough_bits                               =0x0003541d;   /* thumbObject */
aac_framedec                                        =0x000379a1;   /* thumbObject */
aac_cmd                                             =0x000379ed;   /* thumbObject */
check_enough_bytes                                  =0x00037fad;   /* thumbObject */
range_decode_culfreq                                =0x00038045;   /* thumbObject */
range_decode_culshift                               =0x00038163;   /* thumbObject */
entropy_decode_mono_3900                            =0x0003827f;   /* thumbObject */
entropy_decode_stereo_3930                          =0x000382a7;   /* thumbObject */
entropy_decode_mono_3990                            =0x000382e7;   /* thumbObject */
entropy_decode_stereo_3990                          =0x0003830f;   /* thumbObject */
predictor_decode_stereo_3930                        =0x0003834f;   /* thumbObject */
predictor_decode_mono_3930                          =0x000383bb;   /* thumbObject */
predictor_decode_stereo_3950                        =0x0003840f;   /* thumbObject */
predictor_decode_mono_3950                          =0x000386c5;   /* thumbObject */
ape_decode_frame                                    =0x000387a5;   /* thumbObject */
ape_decode_cmd                                      =0x00038b85;   /* thumbObject */
flac_check_enough_bits                              =0x0003919c;
ff_vorbis_nth_root                                  =0x0003dbd0;
ff_vorbis_len2vlc                                   =0x0003dbf2;
ff_vorbis_ready_floor1_list                         =0x0003dcc8;
vorbis_check_enough_bits                            =0x0003dfdd;   /* thumbObject */
ff_init_vlc_sparse                                  =0x0003e0d8;
ff_free_vlc                                         =0x0003e30c;
vorbis_clear_saved                                  =0x0003e468;
vorbis_floor0_decode                                =0x0003e481;   /* thumbObject */
vorbis_floor1_decode                                =0x0003e749;   /* thumbObject */
vorbis_framedec                                     =0x0003f557;   /* thumbObject */
vorbis_cmd                                          =0x0003f99d;   /* thumbObject */
vorbis_static_init                                  =0x0003f9e2;
vorbis_static_alloc                                 =0x0003f9ec;
vorbis_static_giveback                              =0x0003fa52;
sbc_unpack_frame                                    =0x000400cd;   /* thumbObject */
msbc_unpack_frame                                   =0x00040165;   /* thumbObject */
sbc_decode                                          =0x000401a5;   /* thumbObject */
sbc_get_stream_info                                 =0x000402e0;
sbc_frame_decode                                    =0x00040375;   /* thumbObject */
av_log2                                             =0x00040c04;
wma_check_enough_bits                               =0x00040c11;   /* thumbObject */
wma_window_output                                   =0x00041155;   /* thumbObject */
wma_decode_block                                    =0x00041e0b;   /* thumbObject */
wma_decode_phdr_decode                              =0x00041f81;   /* thumbObject */
seek_to_next_block                                  =0x000420e1;   /* thumbObject */
wmadec_frame_decode                                 =0x0004218d;   /* thumbObject */
wmadec_cmd                                          =0x00042323;   /* thumbObject */
read_chunk_stts                                     =0x00042ab5;   /* thumbObject */
read_chunk_stsz                                     =0x00042afd;   /* thumbObject */
read_chunk_stsc                                     =0x00042b4f;   /* thumbObject */
qtmovie_getframe                                    =0x00042b99;
qtmovie_seek                                        =0x00042c4d;
qtmovie_cmd                                         =0x00042cfd;
qtmovie_close                                       =0x00042d19;   /* thumbObject */
read_chunk_stco                                     =0x00042d59;   /* thumbObject */
read_chunk_stsd                                     =0x000430f5;   /* thumbObject */
qtmovie_read                                        =0x000432c5;   /* thumbObject */
get_mp4_metadata                                    =0x00043d25;   /* thumbObject */
cvsd_decoder_open                                   =0x00048b3c;
cvsd_decoder_close                                  =0x00048b6c;
cvsd_decoder_proc                                   =0x00048b74;
cvsd_encoder_open                                   =0x00048c88;
cvsd_encoder_close                                  =0x00048cac;
cvsd_encoder_proc                                   =0x00048cb4;
sbc_encode_close                                    =0x000490fa;
sbc_encode_open                                     =0x0004911c;
sbc_encode_proc                                     =0x00049878;
wavenc_close                                        =0x00049c42;
wavenc_open                                         =0x00049c64;
wavenc_proc                                         =0x00049d10;
wavenc_cmd                                          =0x00049dc6;
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
ff_mpa_synth_window_float                           =0x00050988;
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
peaknotch                                           =0x0005d068;
biquad_proc                                         =0x0005d33b;   /* thumbObject */
ap1st_init                                          =0x0005d38e;
ap1st_setcoeff                                      =0x0005d398;
ap2nd_init                                          =0x0005d3d4;
ap2nd_setcoeff                                      =0x0005d3dc;
ap2nd_clear                                         =0x0005d3cc;
ap2nd_proc                                          =0x0005d3e6;
lm_default_priv_key                                 =0x0005e698;
lm_default_pub_key                                  =0x0005e6f8;
co_default_bdaddr                                   =0x0005edd4;
co_null_bdaddr                                      =0x0005edda;
co_sca2ppm                                          =0x0005ede0;
sp_log2                                             =0x0005f9f1;   /* thumbObject */
sp_pow2                                             =0x0005fa31;   /* thumbObject */
sp_arm_cos_f32                                      =0x0005fbd4;
sp_arm_sin_f32                                      =0x0005fc50;
sp_tan                                              =0x0005fcc8;
biquad_clear                                        =0x0005ced6;
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
agc_reset                                           =0x00060dcc;
agc_setrtprms                                       =0x00060af8;
agc_open                                            =0x00063145;   /* thumbObject */
agc_close                                           =0x00063159;   /* thumbObject */
sltkplc_close                                       =0x000632b8;
sltkplc_open                                        =0x00063374;
sltkplc_conceal                                     =0x000634fa;
sltkplc_addhist                                     =0x00063686;
drc_close                                           =0x00063a50;
//drc_cmd                                             =0x00063e54;
drc_init                                            =0x00063ecc;
drc_process                                         =0x00064318;
eq_close                                            =0x00064602;
eq_open                                             =0x0006460a;
eq_process                                          =0x00064a66;
eq_set_preamp                                       =0x00064a06;
preset_octave_gain                                  =0x00067330;
default_eq_coeff                                    =0x00067308;
preset_octave_freq                                  =0x0006731c;
primes                                              =0x00065400;
Cerlog                                              =0x00066980;
S                                                   =0x000669c8;
bark                                                =0x00066f38;
bark_fbin                                           =0x00066f7a;
bark_lenrecip                                       =0x00066fac;
bidx                                                =0x00066ff4;
hwindow                                             =0x00067008;
wd1                                                 =0x00067208;
delay_nop                                           =0x00067382;
boot_usb_init                                       =0x000673a0;
boot_get_info                                       =0x000675ac;
USBD_COMP_Desc_Init                                 =0x00068074;
HAL_UART_Init                                       =0x000682dc;
HAL_UART_DeInit                                     =0x00068384;
HAL_UART_Transmit                                   =0x00068388;
HAL_UART_Receive                                    =0x0006839a;
HAL_UART_EnableIT                                   =0x000683b0;
serial_debug_io_init                                =0x00068418;
xprintf                                             =0x00068424;
printf_array                                        =0x00068620;
HAL_UART_IRQHandler                                 =0x00068670;
get_pin_group_and_id                                =0x000686a0;
HAL_GPIO_Set_Func                                   =0x00068702;
HAL_GPIO_Set_Pull                                   =0x00068746;
HAL_GPIO_Multi_Driving                              =0x0006877a;
HAL_GPIO_To_Irq                                     =0x0006879c;
HAL_GPIO_Irq_Enable                                 =0x000687e0;
HAL_GPIO_Irq_Disable                                =0x000687f8;
HAL_GPIO_Set_Debounce                               =0x00068812;
HAL_GPIO_Get_Pending                                =0x00068818;
gpio_config                                         =0x00068888;
gpio_cfg_func                                       =0x000688c0;
gpio_get_value                                      =0x000688c6;
gpio_set_value                                      =0x000688d8;
pin_config                                          =0x00068912;
pin_get_value                                       =0x00068954;
pin_set_value_rom                                   =0x00068976;
pin_irq_clk_config                                  =0x00068994;
pin_irq_config                                      =0x000689aa;
pin_get_irq_pending                                 =0x000689da;
pin_clear_irq_pending                               =0x000689fe;
pin_get_wakeup_pending                              =0x00068a1a;
pin_clear_wakeup_pending                            =0x00068a38;
pin_set_wakeup                                      =0x00068a54;
pin_get_func                                        =0x00068a6e;
strcpy                                              =0x00068ac8;
strlen                                              =0x00068b38;
strcmp                                              =0x00068b48;
memcpy                                              =0x00068bc0;
memset                                              =0x00068c24;
memmove                                             =0x00068c74;
memcmp                                              =0x00068caa;
libc_malloc                                         =0x00068cc6;
libc_free                                           =0x00068cca;
libc_calloc                                         =0x00068cce;
qsort                                               =0x00068ce6;
atoi                                                =0x00068d74;
CpuClkHz                                            =0x0080016c;
SystickClkHz                                        =0x00800170;
TickRateHz                                          =0x00800174;
uxCriticalNesting                                   =0x00800178;
mini_stack_size                                     =0x0080017c;
timer_queue_length                                  =0x00800180;
timer_task_priority                                 =0x00800184;
g_pHhcd                                             =0x00800190;
g_pHpcd                                             =0x00800198;
USBH_msc                                            =0x008001e4;
g_pUsbdCoreDev                                      =0x00800208;
g_pUsbHidCallback                                   =0x0080020c;
HID_Buffer                                          =0x00800210;
g_pUsbAudioSystem									=0x0080021c;
USBD_COMP_HID                                       =0x0080022c;
USBD_COMP_MSC                                       =0x00800258;
USBD_COMP_AUDIO                                     =0x00800284;
USBD_COMP                                           =0x008002b0;
USBD_COMP_MSC_CfgFSDesc                             =0x008002dc;
COMP_HID_MOUSE_ReportDesc                           =0x008002fc;
USBD_COMP_HID_Desc                                  =0x00800320;
USBD_COMP_DeviceQualifierDesc                       =0x0080032c;
USBD_COMP_CfgDesc                                   =0x00800338;
COMP_Desc                                           =0x0080042c;
USBD_COMP_DISK_fops                                 =0x00800484;
COMP_STORAGE_Inquirydata                            =0x008004a8;
p_suffix_wl                                         =0x008004cc;
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
vUSBD_COMP_MSC_CfgDesc                                =0x00800e90;
hpcd                                                =0x00800fa4;
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
xfs_dir_tree                                        =0x008011f8;
afx_cur_dir_depth                                   =0x008011fc;
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
lc_sco_env                                          =0x00801da8;
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
lc_lmp_pkt_typ_tbl_req_handler                      =0x00011ee9;
lc_ptt                                              =0x00019884;

lm_set_cntl_host_flow_cmd_handler                    =0x00031195;
co_conhdl_database_empty                            =0x0001f9ac;

lm_host_nb_comp_pkts_cmd_handler                    =0x0002e4bd;
hci_host_ack_pkt_intf                                =0x0002ced4;
