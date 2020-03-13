/***********************************************************
*  File: device.h 
*  Author: nzy
*  Date: 20150605
***********************************************************/
#ifndef _DEVICE_H
    #define _DEVICE_H

    #include "sys_adapter.h"
    #include "error_code.h"
    
#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __DEVICE_GLOBALS
    #define __DEVICE_EXT
#else
    #define __DEVICE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
// device information define
#define SW_VER USER_SW_VER
//#define PRODECT_KEY "XPpKfY2eyFN7rt7w" // 彩灯
//#define PRODECT_KEY "1mUEvwutNqKKaAxd" // 先锋油汀
//#define PRODECT_KEY "PKYpqry8qmxcH3m0" // 自动化测试
//#define PRODECT_KEY "PjK3wjTrPflaFX8p" // 涂鸦测试30-dp
//#define PRODECT_KEY "ZH0QJsansRET4bTW" // 涂鸦压测产品
//#define PRODECT_KEY "O95Mw3t2MZc7a8eh" // 水时代
//#define PRODECT_KEY "1KKA7FrKxaPgajO1" // 家碧水时代测试
//#define PRODECT_KEY "2dROy1OLYJDXK1Mz" // 白灯
//#define PRODECT_KEY "hABOtJIkr2FnT5KO" // 高森测试版
//#define PRODECT_KEY "dXa9QyM6B9mn58f7"
#define PRODECT_KEY "dXa9QyM6B9mn58f7"
//#define PRODECT_KEY "VG4fIA0k8Gzaaffi"  //日常测试
//#define PRODECT_KEY "pMNYN9mCvKMIzNyX"   //微信配网测试
//#define PRODECT_KEY "AqHUMdcbYzIq1Of4"   //微信配网测试
//#define PRODECT_KEY "keykkfvuy8pnn5q3" // 高森测试版
//#define PRODECT_KEY "4Ic6GgKI0vUG65Jd" // 爱贝源奶机
//#define PRODECT_KEY "vmXoHkTKj817b9Mt" // 全类型开发数据测试
//#define PRODECT_KEY "WULRoMmK"
#define DEF_DEV_ABI DEV_SINGLE
/***********************************************************
*************************variable define********************
***********************************************************/


/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: device_init
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__DEVICE_EXT \
OPERATE_RET device_init(VOID);


#ifdef __cplusplus
}
#endif
#endif

