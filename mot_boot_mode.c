/* Reversed binary from motorola */
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <cutils/sockets.h>

/* global definitions*/

#define LOG_TAG "mot_boot_mode"
#define PROPERTY_MOT_BOOT "tcmd.suspend"
#define PROPERTY_ADB_BOOT "persist.service.adb.enable"
#define PROPERTY_SYS_CHRGNLY "sys.chargeonly.mode"
#define PU_REASON_SW_AP_RESET "0x00004000" //normal boot
#define PU_REASON_CHARGER "0x00000100" //chargeonly.mode
#define PU_REASON_CID_NORMAL "0x00"

static int ver_major = 0;
static int ver_minor = 3;

static const char* enable_adb(void){
	FILE *fp;
	fp = fopen("/dev/usb_device_mode", "w");
	if (!fp) {
		LOGE("Error at opening file", fp);
		return NULL;
	}
	fprintf(fp, "msc_adb");
	fclose(fp);
	LOGI("ADB Enabled");
	return NULL;
}

int parse(const char * haystack, const char * needle, const char * compare)
	{
	char *tmp = NULL;

	tmp = strstr(haystack, needle);
	if(tmp) {
	tmp = strstr(tmp, ":") + 2;
	if(!strncmp(tmp, compare, strlen(compare))) return tmp;
	}

return NULL;
	} 

int get_bootinfo(void){
        FILE *f;
	char *bootin;
        char str[256];
	f = fopen("/proc/bootinfo", "r");
        if (!f) {
                LOGE("Error at opening %s", f);
                return NULL;
        }
	while(fgets(str, 128, f)){
	if(bootin = parse(str, "POWERUPREASON", PU_REASON_SW_AP_RESET)) {
	LOGI(":MOTO: pwr_rsn:%s", bootin);
	property_set(PROPERTY_MOT_BOOT, "0");
	LOGI(":MOTO: Normal boot");
	}		
	if(bootin = parse(str, "POWERUPREASON", PU_REASON_CHARGER)){ 
	LOGI(":MOTO: pwr_rsn chargemode:%s", bootin);
	property_set(PROPERTY_SYS_CHRGNLY, "1");
	LOGI("Charge only mode enabled");
	}
	if(bootin = parse(str, "CID_RECOVER_BOOT", PU_REASON_CID_NORMAL)) {
	LOGI(":MOTO: cid_recover_boot:%s", bootin);
	}}
	return NULL;
}
int main(int argc, char **argv){
	LOGI(":MOTO_PUPD: mot_boot_mode %d.%d", ver_major, ver_minor);
	enable_adb();
	get_bootinfo();
	return NULL;
}
