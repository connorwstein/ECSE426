
/*******************************************************************************
*  FF_WU_SRC_1 Register: Interrupt 1 source register.
*  Reading at this address clears FF_WU_SRC_1 register and the FF, WU 1 interrupt
*  and allow the refreshment of data in the FF_WU_SRC_1 register if the latched option
*  was chosen.
*  Read only register
*  Default value: 0x00
*  7 Reserved
*  6 IA: Interrupt active.
*        0: no interrupt has been generated
*        1: one or more interrupts have been generated
*  5 ZH: Z high. 
*        0: no interrupt
*        1: ZH event has occurred 
*  4 ZL: Z low.
*        0: no interrupt
*        1: ZL event has occurred
*  3 YH: Y high.
*        0: no interrupt
*        1: YH event has occurred 
*  2 YL: Y low.
*        0: no interrupt
*        1: YL event has occurred
*  1 YH: X high.
*        0: no interrupt
*        1: XH event has occurred 
*  0 YL: X low.
*        0: no interrupt
*        1: XL event has occurred
*******************************************************************************/



#define LSM9DS1_ACT_THS	0x04
#define LSM9DS1_ACT_DUR	0x05
#define LSM9DS1_INT_GEN_CFG_XL	0x06
#define LSM9DS1_INT_GEN_THS_X_XL	0x07
#define	LSM9DS1_INT_GEN_THS_Y_XL	0x08
#define	LSM9DS1_INT_GEN_THS_Z_XL	0x09
#define	LSM9DS1_INT_GEN_DUR_XL	0x0A
#define	LSM9DS1_REFERENCE_G	0x0B
#define	LSM9DS1_INT1_CTRL	0x0C
#define	LSM9DS1_INT2_CTRL	0x0D
#define	LSM9DS1_WHO_AM_I	0x0F
#define	LSM9DS1_CTRL_REG1_G	0x10
#define	LSM9DS1_CTRL_REG2_G	0x11
#define	LSM9DS1_CTRL_REG3_G	0x12
#define	LSM9DS1_ORIENT_CFG_G	0x13
#define	LSM9DS1_INT_GEN_SRC_G	0x14
#define	LSM9DS1_OUT_TEMP_L	0x15
#define	LSM9DS1_OUT_TEMP_H	0x16
#define	LSM9DS1_STATUS_REG_1	0x17
#define	LSM9DS1_OUT_X_L_G	0x18
#define	LSM9DS1_OUT_X_H_G	0x19
#define	LSM9DS1_OUT_Y_L_G	0x1A
#define	LSM9DS1_OUT_Y_H_G	0x1B
#define	LSM9DS1_OUT_Z_L_G	0x1C
#define	LSM9DS1_OUT_Z_H_G	0x1D
#define	LSM9DS1_CTRL_REG4	0x1E
#define	LSM9DS1_CTRL_REG5_XL	0x1F
#define	LSM9DS1_CTRL_REG6_XL	0x20
#define	LSM9DS1_CTRL_REG7_XL	0x21
#define	LSM9DS1_CTRL_REG8	0x22
#define	LSM9DS1_CTRL_REG9	0x23
#define	LSM9DS1_CTRL_REG10	0x24
#define	LSM9DS1_INT_GEN_SRC_XL	0x26
#define	LSM9DS1_STATUS_REG_2	0x27
#define	LSM9DS1_OUT_X_L_XL	0x28
#define	LSM9DS1_OUT_X_H_XL	0x29
#define	LSM9DS1_OUT_Y_L_XL	0x2A
#define	LSM9DS1_OUT_Y_H_XL	0x2B
#define	LSM9DS1_OUT_Z_L_XL	0x2C
#define	LSM9DS1_OUT_Z_H_XL	0x2D
#define	LSM9DS1_FIFO_CTRL	0x2E
#define	LSM9DS1_FIFO_SRC	0x2F
#define	LSM9DS1_INT_GEN_CFG_G	0x30
#define	LSM9DS1_INT_GEN_THS_XH_G	0x31
#define	LSM9DS1_INT_GEN_THS_XL_G	0x32
#define	LSM9DS1_INT_GEN_THS_YH_G	0x33
#define	LSM9DS1_INT_GEN_THS_YL_G	0x34
#define	LSM9DS1_INT_GEN_THS_ZH_G	0x35
#define	LSM9DS1_INT_GEN_THS_ZL_G	0x36
#define	LSM9DS1_INT_GEN_DUR_G	0x37
