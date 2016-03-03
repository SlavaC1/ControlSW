#ifndef __CMDCODES_MG_H
#define __CMDCODES_MG_H

// - HitagRWD CommandCodes ---------------------------------------------------
#define        RWD_SET_POWER_DOWN           0x44    //'D'
#define        RWD_EE_READ                  0x45    //'E'
#define        RWD_FFT_COMMAND              0x46    //'F'
#define        RWD_SET_BCD                  0x46    //'F'
#define        RWD_READ_INPUT               0x49    //'I'
#define        RWD_KEY_INIT_MODE            0x4B    //'K'
#define        RWD_SET_HF_MODE              0x4C    //'L'
#define        RWD_SET_OUTPUT               0xAF	//0x4F//'O'
#define        RWD_RESET_SYS                0x52    //'R'
#define        RWD_GET_VERSION              0x56    //'V'
#define        RWD_CONFIG_PORTS             0x63    //'c'
#define        RWD_EE_WRITE                 0x65    //'e'
#define        RWD_READ_BCD                 0x66    //'f'
#define        RWD_HF_RESET                 0x68    //'h'
#define        RWD_READ_PORTS               0x69    //'i'
#define        RWD_WRITE_PORTS              0x6F    //'o'
#define        RWD_READ_LR_STATUS           0x72    //'r'
#define        RWD_POLL_KB_TAGS             0x90
#define        RWD_SETMODULEADDR            0x91
#define        RWD_SET_PROX_TRM_TIME        0xA1
#define        RWD_SET_BCD_OFFSET           0xA4
#define        RWD_STOP_CMD                 0xA6
#define        RWD_SET_BAUDRATE             0xA7
//

// - Hitag1 CommandCodes -----------------------------------------------------
#define        HT1_MUTUAL_AUTHENT           0x41    //'A'
#define        HT1_READ_BLOCK               0x42    //'B'
#define        HT1_GET_SNR                  0x47    //'G'
#define        HT1_HALT_SELECTED            0x48    //'H'
#define        HT1_READ_PAGE                0x50    //'P'
#define        HT1_SELECT_SNR               0x53    //'S'
#define        HT1_SELECT_LAST              0x53    //'S'
#define        HT1_TAG_AUTHENT              0x61    //'a'
#define        HT1_WRITE_BLOCK              0x62    //'b'
#define        HT1_POLL_TAGS                0x6C    //'l'
#define        HT1_WRITE_PAGE               0x70    //'p'
#define        HT1_POLL_KB_TAGS             0x90
#define        HT1_GET_SNR_ADV              0xA2
#define        HT1_GET_SNR_NEW_SELECT       0x78    //'x'
#define        HT1_GET_SNR_SELECT           0xA8
#define        HT1_GET_SNR_SELECT_HALT      0x79    //'y'
#define        HT1_GET_SNR_NEW_SELECT_HALT  0xA9
#define        HT1_SELECT_SNR_DIRECT        0x7A    //'z'
//

// - Hitag2 CommandCodes -----------------------------------------------------
#define        HT2_READ_MIRO                0x4D    //'M'
#define        HT2_POLL_TAGS                0x6C    //'l'
#define        HT2_GET_SNR                  0x80
#define        HT2_GET_SNR_RESET            0x80
#define        HT2_HALT_SELECTED            0x81
#define        HT2_READ_PAGE                0x82
#define        HT2_READ_PAGE_INV            0x83
#define        HT2_WRITE_PAGE               0x84
#define        HT2_READ_PIT                 0x85
#define        HT2_POLL_KB_TAGS             0x90
#define        HT2_READ_PUBLICB             0x9E
#define        HT2_READ_TTF                 0x9F
//

// - HitagS CommandCodes -----------------------------------------------------
#define        HTS_GET_SNR_RESET            0x79    //'y'
#define        HTS_SELECT_SNR_RESET         0x7A    //'z'
#define        HTS_TAG_AUTHENT              0xA8
//

// - Vegas CommandCodes ------------------------------------------------------
#define        VEGAS_READ_ALL_PAGE          0x98
#define        VEGAS_GET_DSP_VERSION        0x76    //'v'
//

// - KeyInit CommandCodes ----------------------------------------------------
#define        KI_READ_CONTROL              0x43    //'C'
#define        KI_RESET                     0x52    //'R'
#define        KI_READ_SECRET_HT2           0x56    //'V'
#define        KI_WRITE_SECRET_HT2          0x57    //'W'
#define        KI_READ_EE_DATA              0x58    //'X'
#define        KI_WRITE_EE_DATA             0x59    //'Y'
#define        KI_WRITE_CONTROL             0x63    //'c'
#define        KI_WRITE_SERNUM              0x73    //'s'
#define        KI_WRITE_PIT_SECURITY        0x76    //'v'
#define        KI_WRITE_PIT_PASSWORD        0x77    //'w'
#define        KI_READ_CONTROL_HT2          0x90
#define        KI_WRITE_CONTROL_HT2         0x91
#define        KI_READ_CONTROL_HTS          0x92
#define        KI_WRITE_CONTROL_HTS         0x93
//

#endif  // __CMDCODES_MG_H
