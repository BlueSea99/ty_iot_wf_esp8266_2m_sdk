/***********************************************************
*  File: device.c 
*  Author: nzy
*  Date: 20150605
***********************************************************/
#define __DEVICE_GLOBALS
#include "device.h"
#include "mem_pool.h"
#include "smart_wf_frame.h"
#include "key.h"
#include "led_indicator.h"
#include "system/sys_timer.h"
#include "smart_link.h"
#include "wf_sdk_adpt.h"
#include "system/uni_thread.h"


/***********************************************************
*************************micro define***********************
***********************************************************/
#define WF_RESET_KEY GPIO_ID_PIN(0)
#define WF_DIR_LED GPIO_ID_PIN(14)

/***********************************************************
*************************variable define********************
***********************************************************/
LED_HANDLE wf_light = NULL;

/***********************************************************
*************************function define********************
***********************************************************/
STATIC OPERATE_RET device_differ_init(VOID);
STATIC VOID wfl_timer_cb(UINT timerID,PVOID pTimerArg);
STATIC VOID key_process(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt);

VOID set_firmware_tp(IN OUT CHAR *firm_name, IN OUT CHAR *firm_ver)
{
	strcpy(firm_name,APP_BIN_NAME);
	strcpy(firm_ver,USER_SW_VER);
	return;
}

VOID pre_app_init(VOID)
{

}

BOOL gpio_func_test(VOID)
{
    return TRUE;
}

VOID device_cb(SMART_CMD_E cmd,cJSON *root)
{
    CHAR *buf = cJSON_PrintUnformatted(root);
    if(NULL == buf) {
        PR_ERR("malloc error");
        return;
    }

    PR_DEBUG("here buf:%s",buf);
    
    OPERATE_RET op_ret = sf_obj_dp_report(get_single_wf_dev()->dev_if.id,buf);
    if(OPRT_OK != op_ret) {
        PR_ERR("sf_obj_dp_report err:%d",op_ret);
        PR_DEBUG_RAW("%s\r\n",buf);
        Free(buf);
        return;
    }
    Free(buf);
}

//当设备处于低功耗模式，扫描到tuya_mdev_test1时进入产测
VOID prod_test(BOOL flag, CHAR rssi)
{
    PR_DEBUG("flag:%d,rssi:%d", flag,rssi);
    return;
}

VOID app_init(VOID)
{
	set_console(FALSE);
	app_cfg_set(WCM_SPCL_MODE,prod_test);
}

TIMER_ID timer_init_dpdata;

STATIC VOID init_upload_proc(VOID)
{
    cJSON *root_test = NULL;
    OPERATE_RET op_ret;
    CHAR *out = NULL;

    if( get_gw_status() != STAT_WORK ) {
		return;
	}
	root_test = cJSON_CreateObject();
	if(NULL == root_test) {
		return;
	}
	cJSON_AddTrueToObject(root_test, "1");

    out=cJSON_PrintUnformatted(root_test);
	cJSON_Delete(root_test);
	if(NULL == out) {
		PR_ERR("cJSON_PrintUnformatted err:");
        Free(out);
		return;
	}
	PR_DEBUG("out[%s]", out);
    op_ret = sf_obj_dp_report(get_single_wf_dev()->dev_if.id,out);
    if(OPRT_OK != op_ret) {
        PR_ERR("sf_obj_dp_report err:%d",op_ret);
        Free(out);
        return;
    }
	Free(out);
	return;

}

STATIC VOID idu_timer_cb(UINT timerID,PVOID pTimerArg)
{
    if(get_gw_mq_conn_stat()== TRUE){
        init_upload_proc();
        sys_stop_timer(timer_init_dpdata);
    }
}


VOID device_query_cb(IN CONST TY_DP_QUERY_S *dp_qry)
{
    if(NULL == dp_qry) {
        PR_DEBUG("dp_qry is NULL");
        return ;
    }

    PR_DEBUG("dp_qry->cnt:%d",dp_qry->cnt);
    UINT i = 0;
    for(i = 0; i < dp_qry->cnt; i++) {
        if(dp_qry->cid) {
            PR_DEBUG("cid:%s",dp_qry->cid);
        }else {
            PR_DEBUG("no cid");
        }
        PR_DEBUG("dpid:%d",dp_qry->dpid[i]);
    }
}

/***********************************************************
*  Function: device_init
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
OPERATE_RET device_init(VOID)
{
    OPERATE_RET op_ret;

    op_ret = smart_frame_init(device_cb,USER_SW_VER);
    if(op_ret != OPRT_OK) {
        return op_ret;
    }

    TY_IOT_CBS_S wf_cbs = {
        device_query_cb,
        NULL
    };

    op_ret = gw_register_cbs(&wf_cbs);
    if(op_ret != OPRT_OK) {
        PR_ERR("gw_register_cbs op_ret:%d",op_ret);
        return op_ret;
    }

    DEV_DESC_IF_S def_dev_if;

    strcpy(def_dev_if.product_key,PRODECT_KEY);
    strcpy(def_dev_if.sw_ver,SW_VER);
    def_dev_if.ability = DEV_SINGLE;

    op_ret = single_wf_device_init_pk(&def_dev_if);
    if(op_ret != OPRT_OK) {
        return op_ret;
    }

    op_ret = device_differ_init();
    if(op_ret != OPRT_OK) {
        return op_ret;
    }

    return op_ret;
}

STATIC VOID key_process(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt)
{
    PR_DEBUG("gpio_no: %d",gpio_no);
    PR_DEBUG("type: %d",type);
    PR_DEBUG("cnt: %d",cnt);

    if(WF_RESET_KEY == gpio_no) {
        if(LONG_KEY == type) {
            single_dev_reset_factory();
        }else if(SEQ_KEY == type && cnt >= 5) { // data restore factory            
            auto_select_wf_cfg();
        }else if(SEQ_KEY == type && cnt == 2) {
            ShowSysMemPoolInfo();
        }else if(SEQ_KEY == type && cnt == 3) {
            system_print_meminfo();
            PR_NOTICE("remain size:%d",system_get_free_heap_size());
        }
        else if(NORMAL_KEY == type) {

            CONST CHAR *tmp[] = {"{\"1\":true}","{\"1\":false}"};
            STATIC UCHAR i = 0;
            if(i == 1) {
                i = 0;
            }else {
                i = 1;
            }
            OPERATE_RET op_ret = sf_obj_dp_report(get_single_wf_dev()->dev_if.id,tmp[i]);
            if(op_ret != OPRT_OK) {
                PR_DEBUG("op_ret:%d",op_ret);
            }
            PR_NOTICE("remain size:%d",system_get_free_heap_size());
        }
    }
}

STATIC OPERATE_RET device_differ_init(VOID)
{
    OPERATE_RET op_ret;

    set_key_detect_time(0);

//    set_key_detect_high_valid(TRUE); 

    // key process init
    op_ret = key_init(NULL,0,25);
    if(OPRT_OK  != op_ret) {
        return op_ret;
    }

    // register key to process
    op_ret = reg_proc_key(WF_RESET_KEY,3000,key_process);
    if(OPRT_OK  != op_ret) {
        return op_ret;
    }

    // create led handle
    op_ret = create_led_handle(WF_DIR_LED,&wf_light);
    if(OPRT_OK  != op_ret) {
        return op_ret;
    }
    
    op_ret = sys_add_timer(idu_timer_cb,NULL,&timer_init_dpdata);
    if(OPRT_OK != op_ret) {
        return op_ret;
    }else {
//        sys_start_timer(timer_init_dpdata,300,TIMER_CYCLE);
    }
    
    TIMER_ID timer;
    op_ret = sys_add_timer(wfl_timer_cb,NULL,&timer);
    if(OPRT_OK != op_ret) {
        return op_ret;
    }else {
        sys_start_timer(timer,300,TIMER_CYCLE);
    }

    return OPRT_OK;
}

STATIC VOID wfl_timer_cb(UINT timerID,PVOID pTimerArg)
{
    STATIC UINT last_wf_stat = 0xffffffff;
    GW_WIFI_STAT_E wf_stat = get_wf_gw_status();
    
    if(last_wf_stat != wf_stat) {
        PR_DEBUG("wf_stat:%d",wf_stat);
        switch(wf_stat) {
            case STAT_UNPROVISION: {
				PR_DEBUG("STAT_UNPROVISION");
                set_led_light_type(wf_light,OL_FLASH_HIGH,250);
            }
            break;
            
            case STAT_AP_STA_UNCONN:
            case STAT_AP_STA_CONN: {
				PR_DEBUG("STAT_AP_STA_UNCONN");
                set_led_light_type(wf_light,OL_FLASH_HIGH,1500);
            }
            break;
            
            case STAT_STA_UNCONN: 
            case STAT_LOW_POWER: {
                if(wf_stat == STAT_STA_UNCONN) {
				    PR_DEBUG("STAT_STA_UNCONN");
                }else {
                    PR_DEBUG("LOW POWER");
                }
                set_led_light_type(wf_light,OL_HIGH,0);
            }
            break;
            
            case STAT_STA_CONN: {
				PR_DEBUG("STAT_STA_CONN");
                set_led_light_type(wf_light,OL_LOW,0);
            }
            break;
        }

        last_wf_stat = wf_stat;
    }
}







