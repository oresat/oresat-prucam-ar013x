/**
 * @file    ar013x_regs.h
 * @brief   AR013x CMOS Digital Image Sensor registers.
 *
 * @addtogroup AR013x
 */

#ifndef AR013X_REGS_H
#define AR013X_REGS_H

/**
 * @name AR013X Register Addresses
 * @{
 */
#define AR013X_AD_CHIP_VERSION_REG              0x3000U
#define AR013X_AD_Y_ADDR_START                  0x3002U
#define AR013X_AD_X_ADDR_START                  0x3004U
#define AR013X_AD_Y_ADDR_END                    0x3006U
#define AR013X_AD_X_ADDR_END                    0x3008U
#define AR013X_AD_FRAME_LEN_LINES               0x300AU
#define AR013X_AD_LINE_LENGTH_PCK               0x300CU
#define AR013X_AD_REVISION_NUMBER               0x300EU
#define AR013X_AD_COARSE_INTEGRATION_TIME       0x3012U
#define AR013X_AD_FINE_INTERGRATION_TIME        0x3014U
#define AR013X_AD_COARSE_INTEGRATION_TIME_CB    0x3016U
#define AR013X_AD_FINE_INTERGRATION_TIME_CB     0x3018U
#define AR013X_AD_RESET_REGISTER                0x301AU
#define AR013X_AD_DATA_PEDESTAL                 0x301EU
#define AR013X_AD_GPI_STATUS                    0x3026U
#define AR013X_AD_ROW_SPEED                     0x3028U
#define AR013X_AD_VT_PIX_CLK_DIV                0x302AU
#define AR013X_AD_VT_SYS_CLK_DIV                0x302CU
#define AR013X_AD_PRE_PLL_CLK_DIV               0x302EU
#define AR013X_AD_PLL_MULTIPLIER                0x3030U
#define AR013X_AD_DIGITAL_BINNING               0x3032U
#define AR013X_AD_FRAME_COUNT                   0x303AU
#define AR013X_AD_FRAME_STATUS                  0x303CU
#define AR013X_AD_READ_MODE                     0x3040U
#define AR013X_AD_DARK_CONTROL                  0x3044U
#define AR013X_AD_FLASH                         0x3046U
#define AR013X_AD_GREEN1_GAIN                   0x3056U
#define AR013X_AD_BLUE_GAIN                     0x3058U
#define AR013X_AD_RED_GAIN                      0x305AU
#define AR013X_AD_GREEN2_GAIN                   0x305CU
#define AR013X_AD_GLOBAL_GAIN                   0x305EU
#define AR013X_AD_EMBEDDED_DATA_CTRL            0x3064U
#define AR013X_AD_DATAPATH_SELECT               0x306EU
#define AR013X_AD_TEST_PATTERN_MODE             0x3070U
#define AR013X_AD_TEST_DATA_RED                 0x3072U
#define AR013X_AD_TEST_DATA_GREENR              0x3074U
#define AR013X_AD_TEST_DATA_BLUE                0x3076U
#define AR013X_AD_TEST_DATA_GREENB              0x3078U
#define AR013X_AD_TEST_RAW_MODE                 0x307AU
#define AR013X_AD_OPERATION_MODE_CTRL           0x3082U
#define AR013X_AD_OPERATION_MODE_CTRL_CB        0x3084U
#define AR013X_AD_SEQ_DATA_PORT                 0x3086U
#define AR013X_AD_SEQ_CTRL_PORT                 0x3088U
#define AR013X_AD_X_ADDR_START_CB               0x308AU
#define AR013X_AD_Y_ADDR_START_CB               0x308CU
#define AR013X_AD_X_ADDR_END_CB                 0x309EU
#define AR013X_AD_Y_ADDR_END_CB                 0x3090U
#define AR013X_AD_X_EVEN_INC                    0x30A0U
#define AR013X_AD_X_ODD_INC                     0x30A2U
#define AR013X_AD_Y_EVEN_INC                    0x30A4U
#define AR013X_AD_Y_ODD_INC                     0x30A6U
#define AR013X_AD_Y_ODD_INC_CB                  0x30A8U
#define AR013X_AD_FRAME_LEN_LINES_CB            0x30AAU
#define AR013X_AD_FRAME_EXPOSURE                0x30ACU
#define AR013X_AD_DIGITAL_TEST                  0x30B0U
#define AR013X_AD_TEMPSENS_DATA                 0x30B2U
#define AR013X_AD_TEMPSENS_CTRL                 0x30B4U
#define AR013X_AD_DIGITAL_CTRL                  0x30BAU
#define AR013X_AD_GREEN1_GAIN_CB                0x30BCU
#define AR013X_AD_BLUE_GAIN_CB                  0x30BEU
#define AR013X_AD_RED_GAIN_CB                   0x30C0U
#define AR013X_AD_GREEN2_GAIN_CB                0x30C2U
#define AR013X_AD_GLOBAL_GAIN_CB                0x30C4U
#define AR013X_AD_TEMPSENS_CALIB1               0x30C6U
#define AR013X_AD_TEMPSENS_CALIB2               0x30C8U
#define AR013X_AD_TEMPSENS_CALIB3               0x30CAU
#define AR013X_AD_TEMPSENS_CALIB4               0x30CCU
#define AR013X_AD_COLUMN_CORRECTION             0x30D4U
#define AR013X_AD_AE_CTRL_REG                   0x3100U
#define AR013X_AD_AE_LUMA_TARGET_REG            0x3102U
#define AR013X_AD_AE_MIN_EV_STEP_REG            0x3108U
#define AR013X_AD_AE_MAX_EV_STEP_REG            0x310AU
#define AR013X_AD_AE_DAMP_OFFSET_REG            0x310CU
#define AR013X_AD_AE_DAMP_GAIN_REG              0x310EU
#define AR013X_AD_AE_DAMP_MAX_REG               0x3110U
#define AR013X_AD_AE_MAX_EXPOSURE_REG           0x311CU
#define AR013X_AD_AE_MIN_EXPOSURE_REG           0x311EU
#define AR013X_AD_AE_DARK_CUR_THRESH_REG        0x3124U
#define AR013X_AD_AE_CURRENT_GAINS              0x312AU
#define AR013X_AD_AE_ROI_X_START_OFFSET         0x3140U
#define AR013X_AD_AE_ROI_Y_START_OFFSET         0x3142U
#define AR013X_AD_AE_ROI_X_SIZE                 0x3144U
#define AR013X_AD_AE_ROI_Y_SIZE                 0x3146U
#define AR013X_AD_AE_MEAN_L                     0x3152U
#define AR013X_AD_AE_COARSE_INTEGRATION_TIME    0x3164U
#define AR013X_AD_AE_AG_EXPOSURE_HI             0x3166U
#define AR013X_AD_AE_AG_EXPOSURE_LO             0x3168U
#define AR013X_AD_DELTA_DK_LEVEL                0x3188U
#define AR013X_AD_HIPSI_TIMING                  0x31C0U
#define AR013X_AD_HIPSI_CONTROL_STATUS          0x31C6U
#define AR013X_AD_HIPSI_CRC_0                   0x31C8U
#define AR013X_AD_HIPSI_CRC_1                   0x31CAU
#define AR013X_AD_HIPSI_CRC_2                   0x31CCU
#define AR013X_AD_HIPSI_CRC_3                   0x31CEU
#define AR013X_AD_HDR_COMP                      0x31D0U
#define AR013X_AD_STAT_CRC_3                    0x31D2U
#define AR013X_AD_I2C_WRT_CHECKSUM              0x31D6U
#define AR013X_AD_HORIZONTAL_CURSOR_POSITION    0x31E8U
#define AR013X_AD_VERTICAL_CURSOR_POSITION      0x31EAU
#define AR013X_AD_HORIZONTAL_CURSOR_WIDTH       0x31ECU
#define AR013X_AD_VERTICAL_CURSOR_WIDTH         0x31EEU
#define AR013X_AD_I2C_IDS                       0x31FCU
#define AR013X_AD_OTPM_DATA_0                   0x3800U
#define AR013X_AD_OTPM_DATA_1                   0x3802U
#define AR013X_AD_OTPM_DATA_2                   0x3804U
#define AR013X_AD_OTPM_DATA_3                   0x3806U
#define AR013X_AD_OTPM_DATA_4                   0x3808U
#define AR013X_AD_OTPM_DATA_5                   0x380AU
#define AR013X_AD_OTPM_DATA_6                   0x380CU
#define AR013X_AD_OTPM_DATA_7                   0x380EU
#define AR013X_AD_OTPM_DATA_8                   0x3810U
#define AR013X_AD_OTPM_DATA_9                   0x3812U
#define AR013X_AD_OTPM_DATA_10                  0x3814U
#define AR013X_AD_OTPM_DATA_11                  0x3816U
#define AR013X_AD_OTPM_DATA_12                  0x3818U
#define AR013X_AD_OTPM_DATA_13                  0x381AU
#define AR013X_AD_OTPM_DATA_14                  0x381CU
#define AR013X_AD_OTPM_DATA_15                  0x381EU
#define AR013X_AD_OTPM_DATA_16                  0x3820U
#define AR013X_AD_OTPM_DATA_17                  0x3822U
#define AR013X_AD_OTPM_DATA_18                  0x3824U
#define AR013X_AD_OTPM_DATA_19                  0x3826U
#define AR013X_AD_OTPM_DATA_20                  0x3828U
#define AR013X_AD_OTPM_DATA_21                  0x382AU
#define AR013X_AD_OTPM_DATA_22                  0x382CU
#define AR013X_AD_OTPM_DATA_23                  0x382EU
#define AR013X_AD_OTPM_DATA_24                  0x3830U
#define AR013X_AD_OTPM_DATA_25                  0x3832U
#define AR013X_AD_OTPM_DATA_26                  0x3834U
#define AR013X_AD_OTPM_DATA_27                  0x3836U
#define AR013X_AD_OTPM_DATA_28                  0x3838U
#define AR013X_AD_OTPM_DATA_29                  0x383AU
#define AR013X_AD_OTPM_DATA_30                  0x383CU
#define AR013X_AD_OTPM_DATA_31                  0x383EU
#define AR013X_AD_OTPM_DATA_32                  0x3840U
#define AR013X_AD_OTPM_DATA_33                  0x3842U
#define AR013X_AD_OTPM_DATA_34                  0x3844U
#define AR013X_AD_OTPM_DATA_35                  0x3846U
#define AR013X_AD_OTPM_DATA_36                  0x3848U
#define AR013X_AD_OTPM_DATA_37                  0x384AU
#define AR013X_AD_OTPM_DATA_38                  0x384CU
#define AR013X_AD_OTPM_DATA_39                  0x384EU
#define AR013X_AD_OTPM_DATA_40                  0x3850U
#define AR013X_AD_OTPM_DATA_41                  0x3852U
#define AR013X_AD_OTPM_DATA_42                  0x3854U
#define AR013X_AD_OTPM_DATA_43                  0x3856U
#define AR013X_AD_OTPM_DATA_44                  0x3858U
#define AR013X_AD_OTPM_DATA_45                  0x385AU
#define AR013X_AD_OTPM_DATA_46                  0x385CU
#define AR013X_AD_OTPM_DATA_47                  0x385EU
#define AR013X_AD_OTPM_DATA_48                  0x3860U
#define AR013X_AD_OTPM_DATA_49                  0x3862U
#define AR013X_AD_OTPM_DATA_50                  0x3864U
#define AR013X_AD_OTPM_DATA_51                  0x3866U
#define AR013X_AD_OTPM_DATA_52                  0x3868U
#define AR013X_AD_OTPM_DATA_53                  0x386AU
#define AR013X_AD_OTPM_DATA_54                  0x386CU
#define AR013X_AD_OTPM_DATA_55                  0x386EU
#define AR013X_AD_OTPM_DATA_56                  0x3870U
#define AR013X_AD_OTPM_DATA_57                  0x3872U
#define AR013X_AD_OTPM_DATA_58                  0x3874U
#define AR013X_AD_OTPM_DATA_59                  0x3876U
#define AR013X_AD_OTPM_DATA_60                  0x3878U
#define AR013X_AD_OTPM_DATA_61                  0x387AU
#define AR013X_AD_OTPM_DATA_62                  0x387CU
#define AR013X_AD_OTPM_DATA_63                  0x387EU
#define AR013X_AD_OTPM_DATA_64                  0x3880U
#define AR013X_AD_OTPM_DATA_65                  0x3882U
#define AR013X_AD_OTPM_DATA_66                  0x3884U
#define AR013X_AD_OTPM_DATA_67                  0x3886U
#define AR013X_AD_OTPM_DATA_68                  0x3888U
#define AR013X_AD_OTPM_DATA_69                  0x388AU
#define AR013X_AD_OTPM_DATA_70                  0x388CU
#define AR013X_AD_OTPM_DATA_71                  0x388EU
#define AR013X_AD_OTPM_DATA_72                  0x3890U
#define AR013X_AD_OTPM_DATA_73                  0x3892U
#define AR013X_AD_OTPM_DATA_74                  0x3894U
#define AR013X_AD_OTPM_DATA_75                  0x3896U
#define AR013X_AD_OTPM_DATA_76                  0x3898U
#define AR013X_AD_OTPM_DATA_77                  0x389AU
#define AR013X_AD_OTPM_DATA_78                  0x389CU
#define AR013X_AD_OTPM_DATA_79                  0x389EU
#define AR013X_AD_OTPM_DATA_80                  0x38A0U
#define AR013X_AD_OTPM_DATA_81                  0x38A2U
#define AR013X_AD_OTPM_DATA_82                  0x38A4U
#define AR013X_AD_OTPM_DATA_83                  0x38A6U
#define AR013X_AD_OTPM_DATA_84                  0x38A8U
#define AR013X_AD_OTPM_DATA_85                  0x38AAU
#define AR013X_AD_OTPM_DATA_86                  0x38ACU
#define AR013X_AD_OTPM_DATA_87                  0x38AEU
#define AR013X_AD_OTPM_DATA_88                  0x38B0U
#define AR013X_AD_OTPM_DATA_89                  0x38B2U
#define AR013X_AD_OTPM_DATA_90                  0x38B4U
#define AR013X_AD_OTPM_DATA_91                  0x38B6U
#define AR013X_AD_OTPM_DATA_92                  0x38B8U
#define AR013X_AD_OTPM_DATA_93                  0x38BAU
#define AR013X_AD_OTPM_DATA_94                  0x38BCU
#define AR013X_AD_OTPM_DATA_95                  0x38BEU
#define AR013X_AD_OTPM_DATA_96                  0x38C0U
#define AR013X_AD_OTPM_DATA_97                  0x38C2U
#define AR013X_AD_OTPM_DATA_98                  0x38C4U
#define AR013X_AD_OTPM_DATA_99                  0x38C6U
#define AR013X_AD_OTPM_DATA_100                 0x38C8U
#define AR013X_AD_OTPM_DATA_101                 0x38CAU
#define AR013X_AD_OTPM_DATA_102                 0x38CCU
#define AR013X_AD_OTPM_DATA_103                 0x38CEU
#define AR013X_AD_OTPM_DATA_104                 0x38D0U
#define AR013X_AD_OTPM_DATA_105                 0x38D2U
#define AR013X_AD_OTPM_DATA_106                 0x38D4U
#define AR013X_AD_OTPM_DATA_107                 0x38D6U
#define AR013X_AD_OTPM_DATA_108                 0x38D8U
#define AR013X_AD_OTPM_DATA_109                 0x38DAU
#define AR013X_AD_OTPM_DATA_110                 0x38DCU
#define AR013X_AD_OTPM_DATA_111                 0x38DEU
#define AR013X_AD_OTPM_DATA_112                 0x38E0U
#define AR013X_AD_OTPM_DATA_113                 0x38E2U
#define AR013X_AD_OTPM_DATA_114                 0x38E4U
#define AR013X_AD_OTPM_DATA_115                 0x38E6U
#define AR013X_AD_OTPM_DATA_116                 0x38E8U
#define AR013X_AD_OTPM_DATA_117                 0x38EAU
#define AR013X_AD_OTPM_DATA_118                 0x38ECU
#define AR013X_AD_OTPM_DATA_119                 0x38EEU
#define AR013X_AD_OTPM_DATA_120                 0x38F0U
#define AR013X_AD_OTPM_DATA_121                 0x38F2U
#define AR013X_AD_OTPM_DATA_122                 0x38F4U
#define AR013X_AD_OTPM_DATA_123                 0x38F6U
#define AR013X_AD_OTPM_DATA_124                 0x38F8U
#define AR013X_AD_OTPM_DATA_125                 0x38FAU
#define AR013X_AD_OTPM_DATA_126                 0x38FCU
#define AR013X_AD_OTPM_DATA_127                 0x38FEU
#define AR013X_AD_OTPM_DATA_128                 0x3900U
#define AR013X_AD_OTPM_DATA_129                 0x3902U
#define AR013X_AD_OTPM_DATA_130                 0x3904U
#define AR013X_AD_OTPM_DATA_131                 0x3906U
#define AR013X_AD_OTPM_DATA_132                 0x3908U
#define AR013X_AD_OTPM_DATA_133                 0x390AU
#define AR013X_AD_OTPM_DATA_134                 0x390CU
#define AR013X_AD_OTPM_DATA_135                 0x390EU
#define AR013X_AD_OTPM_DATA_136                 0x3910U
#define AR013X_AD_OTPM_DATA_137                 0x3912U
#define AR013X_AD_OTPM_DATA_138                 0x3914U
#define AR013X_AD_OTPM_DATA_139                 0x3916U
#define AR013X_AD_OTPM_DATA_140                 0x3918U
#define AR013X_AD_OTPM_DATA_141                 0x391AU
#define AR013X_AD_OTPM_DATA_142                 0x391CU
#define AR013X_AD_OTPM_DATA_143                 0x391EU
#define AR013X_AD_OTPM_DATA_144                 0x3920U
#define AR013X_AD_OTPM_DATA_145                 0x3922U
#define AR013X_AD_OTPM_DATA_146                 0x3924U
#define AR013X_AD_OTPM_DATA_147                 0x3926U
#define AR013X_AD_OTPM_DATA_148                 0x3928U
#define AR013X_AD_OTPM_DATA_149                 0x392AU
#define AR013X_AD_OTPM_DATA_150                 0x392CU
#define AR013X_AD_OTPM_DATA_151                 0x392EU
#define AR013X_AD_OTPM_DATA_152                 0x3930U
#define AR013X_AD_OTPM_DATA_153                 0x3932U
#define AR013X_AD_OTPM_DATA_154                 0x3934U
#define AR013X_AD_OTPM_DATA_155                 0x3936U
#define AR013X_AD_OTPM_DATA_156                 0x3938U
#define AR013X_AD_OTPM_DATA_157                 0x393AU
#define AR013X_AD_OTPM_DATA_158                 0x393CU
#define AR013X_AD_OTPM_DATA_159                 0x393EU
#define AR013X_AD_OTPM_DATA_160                 0x3940U
#define AR013X_AD_OTPM_DATA_161                 0x3942U
#define AR013X_AD_OTPM_DATA_162                 0x3944U
#define AR013X_AD_OTPM_DATA_163                 0x3946U
#define AR013X_AD_OTPM_DATA_164                 0x3948U
#define AR013X_AD_OTPM_DATA_165                 0x394AU
#define AR013X_AD_OTPM_DATA_166                 0x394CU
#define AR013X_AD_OTPM_DATA_167                 0x394EU
#define AR013X_AD_OTPM_DATA_168                 0x3950U
#define AR013X_AD_OTPM_DATA_169                 0x3952U
#define AR013X_AD_OTPM_DATA_170                 0x3954U
#define AR013X_AD_OTPM_DATA_171                 0x3956U
#define AR013X_AD_OTPM_DATA_172                 0x3958U
#define AR013X_AD_OTPM_DATA_173                 0x395AU
#define AR013X_AD_OTPM_DATA_174                 0x395CU
#define AR013X_AD_OTPM_DATA_175                 0x395EU
#define AR013X_AD_OTPM_DATA_176                 0x3960U
#define AR013X_AD_OTPM_DATA_177                 0x3962U
#define AR013X_AD_OTPM_DATA_178                 0x3964U
#define AR013X_AD_OTPM_DATA_179                 0x3966U
#define AR013X_AD_OTPM_DATA_180                 0x3968U
#define AR013X_AD_OTPM_DATA_181                 0x396AU
#define AR013X_AD_OTPM_DATA_182                 0x396CU
#define AR013X_AD_OTPM_DATA_183                 0x396EU
#define AR013X_AD_OTPM_DATA_184                 0x3970U
#define AR013X_AD_OTPM_DATA_185                 0x3972U
#define AR013X_AD_OTPM_DATA_186                 0x3974U
#define AR013X_AD_OTPM_DATA_187                 0x3976U
#define AR013X_AD_OTPM_DATA_188                 0x3978U
#define AR013X_AD_OTPM_DATA_189                 0x397AU
#define AR013X_AD_OTPM_DATA_190                 0x397CU
#define AR013X_AD_OTPM_DATA_191                 0x397EU
#define AR013X_AD_OTPM_DATA_192                 0x3980U
#define AR013X_AD_OTPM_DATA_193                 0x3982U
#define AR013X_AD_OTPM_DATA_194                 0x3984U
#define AR013X_AD_OTPM_DATA_195                 0x3986U
#define AR013X_AD_OTPM_DATA_196                 0x3988U
#define AR013X_AD_OTPM_DATA_197                 0x398AU
#define AR013X_AD_OTPM_DATA_198                 0x398CU
#define AR013X_AD_OTPM_DATA_199                 0x398EU
#define AR013X_AD_OTPM_DATA_200                 0x3990U
#define AR013X_AD_OTPM_DATA_201                 0x3992U
#define AR013X_AD_OTPM_DATA_202                 0x3994U
#define AR013X_AD_OTPM_DATA_203                 0x3996U
#define AR013X_AD_OTPM_DATA_204                 0x3998U
#define AR013X_AD_OTPM_DATA_205                 0x399AU
#define AR013X_AD_OTPM_DATA_206                 0x399CU
#define AR013X_AD_OTPM_DATA_207                 0x399EU
#define AR013X_AD_OTPM_DATA_208                 0x39A0U
#define AR013X_AD_OTPM_DATA_209                 0x39A2U
#define AR013X_AD_OTPM_DATA_210                 0x39A4U
#define AR013X_AD_OTPM_DATA_211                 0x39A6U
#define AR013X_AD_OTPM_DATA_212                 0x39A8U
#define AR013X_AD_OTPM_DATA_213                 0x39AAU
#define AR013X_AD_OTPM_DATA_214                 0x39ACU
#define AR013X_AD_OTPM_DATA_215                 0x39AEU
#define AR013X_AD_OTPM_DATA_216                 0x39B0U
#define AR013X_AD_OTPM_DATA_217                 0x39B2U
#define AR013X_AD_OTPM_DATA_218                 0x39B4U
#define AR013X_AD_OTPM_DATA_219                 0x39B6U
#define AR013X_AD_OTPM_DATA_220                 0x39B8U
#define AR013X_AD_OTPM_DATA_221                 0x39BAU
#define AR013X_AD_OTPM_DATA_222                 0x39BCU
#define AR013X_AD_OTPM_DATA_223                 0x39BEU
#define AR013X_AD_OTPM_DATA_224                 0x39C0U
#define AR013X_AD_OTPM_DATA_225                 0x39C2U
#define AR013X_AD_OTPM_DATA_226                 0x39C4U
#define AR013X_AD_OTPM_DATA_227                 0x39C6U
#define AR013X_AD_OTPM_DATA_228                 0x39C8U
#define AR013X_AD_OTPM_DATA_229                 0x39CAU
#define AR013X_AD_OTPM_DATA_230                 0x39CCU
#define AR013X_AD_OTPM_DATA_231                 0x39CEU
#define AR013X_AD_OTPM_DATA_232                 0x39D0U
#define AR013X_AD_OTPM_DATA_233                 0x39D2U
#define AR013X_AD_OTPM_DATA_234                 0x39D4U
#define AR013X_AD_OTPM_DATA_235                 0x39D6U
#define AR013X_AD_OTPM_DATA_236                 0x39D8U
#define AR013X_AD_OTPM_DATA_237                 0x39DAU
#define AR013X_AD_OTPM_DATA_238                 0x39DCU
#define AR013X_AD_OTPM_DATA_239                 0x39DEU
#define AR013X_AD_OTPM_DATA_240                 0x39E0U
#define AR013X_AD_OTPM_DATA_241                 0x39E2U
#define AR013X_AD_OTPM_DATA_242                 0x39E4U
#define AR013X_AD_OTPM_DATA_243                 0x39E6U
#define AR013X_AD_OTPM_DATA_244                 0x39E8U
#define AR013X_AD_OTPM_DATA_245                 0x39EAU
#define AR013X_AD_OTPM_DATA_246                 0x39ECU
#define AR013X_AD_OTPM_DATA_247                 0x39EEU
#define AR013X_AD_OTPM_DATA_248                 0x39F0U
#define AR013X_AD_OTPM_DATA_249                 0x39F2U
#define AR013X_AD_OTPM_DATA_250                 0x39F4U
#define AR013X_AD_OTPM_DATA_251                 0x39F6U
#define AR013X_AD_OTPM_DATA_252                 0x39F8U
#define AR013X_AD_OTPM_DATA_253                 0x39FAU
#define AR013X_AD_OTPM_DATA_254                 0x39FCU
#define AR013X_AD_OTPM_DATA_255                 0x39FEU
#define AR013X_AD_DAC_LD_24_25                  0x3EE4U
#define AR013X_AD_BIST_BUFFER_CONTROL1          0x3FD0U
#define AR013X_AD_BIST_BUFFER_CONTROL2          0x3FD2U
#define AR013X_AD_BIST_BUFFER_STATUS1           0x3FD4U
#define AR013X_AD_BIST_BUFFER_STATUS2           0x3FD6U
#define AR013X_AD_BIST_BUFFER_DATA1             0x3FD8U
#define AR013X_AD_BIST_BUFFER_DATA2             0x3FDAU
/** @} */

#endif /* AR013X_REGS_H */
