/***********************************************************
*  File: gpio_test.c  
*  Author: husai
*  Date: 20151215
***********************************************************/
#define __DEVICE_GLOBALS
#include "gpio_test.h"
#include "uart.h"
#include "gpio.h"

STATIC BOOL e1s_test(VOID);
STATIC BOOL e2s_test(VOID);
STATIC BOOL e3s_test(VOID);
STATIC BOOL lc6_test(VOID);
STATIC BOOL lc6e_test(VOID);
STATIC BOOL lc5_test(VOID);


typedef BOOL (*TEST_CB)(VOID);

TEST_CB test_table[] = {e1s_test,e2s_test,e3s_test,lc6_test,lc6e_test,lc5_test};

STATIC INT __read_gpio_level(USHORT gpio_no)
{
	if(gpio_no < 16) {
		GPIO_ConfigTypeDef config = {BIT(gpio_no),GPIO_Mode_Input,GPIO_PullUp_EN,GPIO_PIN_INTR_DISABLE};
		gpio_config(&config);
		return (((gpio_input_get() & BIT(gpio_no)) >> gpio_no) & 0x01);
	}else {
		gpio16_input_conf();
		return gpio16_input_get();
	}
}

STATIC VOID __write_gpio_level(USHORT gpio_no, UCHAR level)
{
	if(gpio_no < 16) {
		GPIO_ConfigTypeDef config = {BIT(gpio_no),GPIO_Mode_Output,GPIO_PullUp_EN,GPIO_PIN_INTR_DISABLE};
		gpio_config(&config);
		GPIO_OUTPUT_SET(gpio_no,level);
	}else {
		gpio16_output_conf();
		gpio16_output_set(level);
	}
}

STATIC VOID set_gpio_level(USHORT gpio[], INT size, INT index)
{
    INT i = 0;
    for( i = 0; i < size; i ++ ) {
        if( i == index ) {
            __write_gpio_level(gpio[i],1);
        }else {
            __write_gpio_level(gpio[i],0);
        }
    }
}

STATIC BOOL get_gpio_level(USHORT gpio[], INT size, INT index)
{
    INT i, ret;
    for( i = 0; i < size; i ++ ) {
        if( i == index ) {
            ret = __read_gpio_level(gpio[i]);
            if(ret != 1) {
                return FALSE;
            }
        }else {
            ret = __read_gpio_level(gpio[i]);
            if(ret != 0) {
                return FALSE;
            }
        }
    }
    return TRUE;
}


/*IO15只能做输出*/
STATIC BOOL e1s_test(VOID)
{
    USHORT gp_in[] = {15,13,0,2};
    USHORT gp_out[] = {12,14,4,5};

    INT i = 0, st_pos = 1;
    BOOL flag;
    
	//gpio in test
    for( i = 0; i < CNTSOF(gp_in); i ++ ) {
        set_gpio_level(gp_in,CNTSOF(gp_in),i);
        flag = get_gpio_level(gp_out,CNTSOF(gp_out),i);
        if(flag == FALSE) {
            return FALSE;
        }
    }

	//gpio out test, jump IO15
    for( i = 0; i < CNTSOF(gp_out)-st_pos; i ++ ) {
        set_gpio_level(&gp_out[st_pos],CNTSOF(gp_out)-st_pos,i);
        flag = get_gpio_level(&gp_in[st_pos],CNTSOF(gp_in)-st_pos,i);
        if(flag == FALSE) {
			return FALSE;
        }
    }
	return TRUE;
}

STATIC BOOL e2s_test(VOID)
{
    USHORT gp_in[] = {5,12,13};
    USHORT gp_out[] = {14,4,0};

    INT i;
    BOOL flag;
    
	//gpio in test
    for( i = 0; i < CNTSOF(gp_in); i ++ ) {
        set_gpio_level(gp_in,CNTSOF(gp_in),i);
        flag = get_gpio_level(gp_out,CNTSOF(gp_out),i);
        if(flag == FALSE) {
            return FALSE;
        }
    }

	//gpio out test
    for( i = 0; i < CNTSOF(gp_out); i ++ ) {
        set_gpio_level(gp_out,CNTSOF(gp_out),i);
        flag = get_gpio_level(gp_in,CNTSOF(gp_in),i);
        if(flag == FALSE) {
            return FALSE;
        }
    }
	return TRUE;
}

STATIC BOOL read_write_gpio_test(USHORT in, USHORT out_a, USHORT out_b)
{
    int ret;
    
    __write_gpio_level(in,1);
    ret = __read_gpio_level(out_a);
    if(0 == ret) {
        return FALSE;
    }
    ret = __read_gpio_level(out_b);
    if(0 == ret) {
        return FALSE;
    }

    __write_gpio_level(in,0);
    ret = __read_gpio_level(out_a);
    if(1 == ret) {
        return FALSE;
    }
    ret = __read_gpio_level(out_b);
    if(1 == ret) {
        return FALSE;
    }

    return TRUE;
}

STATIC BOOL e3s_test(VOID)
{
    USHORT gp_in[] = {13,0,15};
    USHORT gp_out[] = {14,5,4};


    INT i,ret;
    BOOL flag;

    flag = read_write_gpio_test(16,12,2);
    if(FALSE == flag) {
        return FALSE;
    }

    flag = read_write_gpio_test(12,2,16);
    if(FALSE == flag) {
        return FALSE;
    }  
    
    flag = read_write_gpio_test(2,12,16);
    if(FALSE == flag) {
        return FALSE;
    }
  
	//gpio in test
    for( i = 0; i < CNTSOF(gp_in); i ++ ) {
        set_gpio_level(gp_in,CNTSOF(gp_in),i);
        flag = get_gpio_level(gp_out,CNTSOF(gp_out),i);
        if(flag == FALSE) {
            return FALSE;
        }
    }

	//gpio out test
    for( i = 0; i < CNTSOF(gp_out); i ++ ) {
        set_gpio_level(gp_out,CNTSOF(gp_out),i);
        flag = get_gpio_level(gp_in,CNTSOF(gp_in),i);
        if(flag == FALSE) {
            return FALSE;
        }
    }
    
	return TRUE;
}

STATIC BOOL lc6e_test(VOID)
{

    USHORT gp_in[] = {5,4,0};
    USHORT gp_out[] = {13,12,14};

    INT i;
    BOOL flag;
	//gpio in test
    
    for( i = 0; i < CNTSOF(gp_in); i ++ ) {
        //PR_NOTICE("io in:%d",i);
        set_gpio_level(gp_in,CNTSOF(gp_in),i);
        flag = get_gpio_level(gp_out,CNTSOF(gp_out),i);
        if(flag == FALSE) {
            return FALSE;
        }
    }

	//gpio out test
    for( i = 0; i < CNTSOF(gp_out); i ++ ) {
        //PR_NOTICE("io out:%d",i);
        set_gpio_level(gp_out,CNTSOF(gp_out),i);
        flag = get_gpio_level(gp_in,CNTSOF(gp_in),i);
        if(flag == FALSE) {
            return FALSE;
        }
    }
	return TRUE;
}


STATIC BOOL lc5_test(VOID)
{

    USHORT gp_in[] = {4,0};
    USHORT gp_out[] = {12,14};

    INT i;
    BOOL flag;
	//gpio in test
    
    for( i = 0; i < CNTSOF(gp_in); i ++ ) {
        //PR_NOTICE("io in:%d",i);
        set_gpio_level(gp_in,CNTSOF(gp_in),i);
        flag = get_gpio_level(gp_out,CNTSOF(gp_out),i);
        if(flag == FALSE) {
            return FALSE;
        }
    }

	//gpio out test
    for( i = 0; i < CNTSOF(gp_out); i ++ ) {
        //PR_NOTICE("io out:%d",i);
        set_gpio_level(gp_out,CNTSOF(gp_out),i);
        flag = get_gpio_level(gp_in,CNTSOF(gp_in),i);
        if(flag == FALSE) {
            return FALSE;
        }
    }
	return TRUE;
}

STATIC BOOL lc6_test(VOID)
{
    USHORT gp_in[] = {12,0};
    USHORT gp_out[] = {14,13};

    INT i;
    BOOL flag;
    
	//gpio in test
    for( i = 0; i < CNTSOF(gp_in); i ++ ) {
        set_gpio_level(gp_in,CNTSOF(gp_in),i);
        flag = get_gpio_level(gp_out,CNTSOF(gp_out),i);
        if(flag == FALSE) {
            return FALSE;
        }
    }

	//gpio out test
    for( i = 0; i < CNTSOF(gp_out); i ++ ) {
        set_gpio_level(gp_out,CNTSOF(gp_out),i);
        flag = get_gpio_level(gp_in,CNTSOF(gp_in),i);
        if(flag == FALSE) {
            return FALSE;
        }
    }
	return TRUE;
}

BOOL all_gpio_test(VOID)
{
    
    INT i = 0;
    BOOL result = FALSE;
    for(i = 0;i < CNTSOF(test_table);i++) {
        result = test_table[i]();
        if(result == TRUE) {
            break;
        }
    }
    return result;
}

