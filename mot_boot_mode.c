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

static int ver_major = 0;
static int ver_minor = 1;

int main(int argc, char **argv)
{
	LOGI(":MOTO_PUPD: mot_boot_mode %d.%d", ver_major, ver_minor);
	LOGI(":MOTO_PUPD: cid_recover_boot=0x00");

//print content of /proc/bootinfo
	FILE *f;
	f = fopen("/proc/bootinfo", "r");
	char str[128];
	if (!f) {
	
		LOGE("Error at opening '%s'", f);
		return NULL;
	}
	while(fgets(str, 128, f))
		LOGI("%s", str);
	fclose(f);
	property_set(PROPERTY_MOT_BOOT, "0");
	LOGI("Set tcmd.suspend to 0");
	property_set(PROPERTY_ADB_BOOT, "1");

	LOGI("ADB at boot enabled");
	LOGD(":MOTO_PUPD: mot_boot_mode 12m: 0");

	FILE *fp;
	fp = fopen("/dev/usb_device_mode", "w");
        if (!fp) {

                LOGE("Error at opening '%s'", fp);
                return NULL;
        }
	fprintf(fp, "msc_adb");
	fclose(fp);
	return NULL;
}
