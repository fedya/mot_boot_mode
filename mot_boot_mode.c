#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#include <cutils/properties.h>
#define LOG_TAG "mot_boot_mode"
#include <cutils/log.h>

#define MOTO_PU_REASON_CHARGE_ONLY    "0x00000100" 
#define MOTO_CID_RECOVER_BOOT	      "0x01"
#define MOTO_DATA_12M		      "1"

static int ver_major = 0;
static int ver_minor = 6;

/********************************************************************
 * Check POWERUPREASON, decide phone powerup to charge only mode or not 
 * Return value:
 * Type: int
 * 1: charge_only_mode
 * 2: enable adb at boot
 * 0: NOT charge_only_mode 
 ********************************************************************/

int enable_adb(void){
	char value[PROPERTY_VALUE_MAX];
	FILE *fp;
	fp = fopen("/dev/usb_device_mode", "w");

	if (!fp) {
		LOGE("Error at opening file");
		return 0;
	}
	fprintf(fp, "msc_adb");
	fclose(fp);
	
	if (property_get("persist.service.adb.enable", value, 0)) {
		LOGD("ADB status is - %s ", value);
/* property_set with adb enable needed for first launch of system or adb on boot will be disabled*/
		property_set("persist.service.adb.enable", "1");
	}
	
	if (property_get("persist.service.adb.enable", value, "1")){
		LOGD("adb service already enabled");
	}
	return 0;
	}


int boot_reason_charge_only(void)
{
    char data[1024], powerup_reason[32];
    int fd, n;
    char *x, *pwrup_rsn;


    fd = open("/proc/bootinfo", O_RDONLY);
    if (fd < 0) return 0;

    n = read(fd, data, 1023);
    close(fd);
    if (n < 0) return 0;

    data[n] = '\0';

    memset(powerup_reason, 0, 32);

    pwrup_rsn = strstr(data, "POWERUPREASON");
    LOGD("MOTO : pwr_rsn = %s", pwrup_rsn);
    if (pwrup_rsn) {
        x = strstr(pwrup_rsn, ": ");
        if (x) {
            x += 2;
            n = 0;
            while (*x && !isspace(*x)) {
                powerup_reason[n++] = *x;
                x++;
                if (n == 31) break;
            }
            powerup_reason[n] = '\0';
            LOGD("MOTO_PUPD: powerup_reason=%s", powerup_reason);
        }
    }
    if (!strncmp(powerup_reason, MOTO_PU_REASON_CHARGE_ONLY, 
		 		(sizeof(MOTO_PU_REASON_CHARGE_ONLY)-1)))
	return 1; 
    else 
	return 0; 
}

/********************************************************************
 * Check CID_RECOVER_BOOT, decide phone powerup to recovery mode or not
 * Return value:
 * Type: int
 * 1: Recovery mode
 * 0: NOT recovery mode
 * ******************************************************************/
int check_cid_recover_boot(void)
{
    char data[1024], cid_recover_boot[32];
    int fd, n;
    char *x, *m_bmode;

    memset(cid_recover_boot, 0, 32);
    
    fd = open("/proc/bootinfo", O_RDONLY);
    if (fd < 0) return 0;
   
    n = read(fd, data, 1023);
    close(fd);
    if (n < 0) return 0;

    data[n] = '\0';

    m_bmode = strstr(data, "CID_RECOVER_BOOT");
    if (m_bmode) {
        x = strstr(m_bmode, ": ");
        if (x) {
            x += 2;
            n = 0;
            while (*x && !isspace(*x)) {
                cid_recover_boot[n++] = *x;
                x++;
                if (n == 31) break;
            }
            cid_recover_boot[n] = '\0';
            LOGD("MOTO_PUPD: cid_recover_boot=%s", cid_recover_boot);
        }
    }
    
    if (!strncmp(cid_recover_boot, MOTO_CID_RECOVER_BOOT, 
		 		(sizeof(MOTO_CID_RECOVER_BOOT)-1)))
	return 1; 
    else 
	return 0; 
}

/********************************************************************
 * Check 12m file is set or not (tcmd suspend related) 
 * return value:
 * Type: int
 * 1: 12m is set
 * 0: 12m is not set
 ********************************************************************/
int check_data_12m(void)
{
    /*TODO: This 12m feature for TCMD need to be locked down*/
    /*and implemented by TCMD team */
    return 0;
}

/********************************************************************
 * 1. Check cid_recover_boot, set property
 * 2. Check boot reason is charge only mode or not, set property
 * 3. Check 12m (tcmd suspend related), set property
 * TODO: Is the priority/order right?
 ********************************************************************/
int main(int argc, char **argv)
{   
    LOGD("MOTO_PUPD: mot_boot_mode %d.%d", ver_major, ver_minor);
    enable_adb();
    if (check_cid_recover_boot()){

        LOGD("MOTO_PUPD: check_cid_recover_boot: 1");
        property_set("tcmd.cid.recover.boot", "1");
        property_set("tcmd.suspend", "1");

    }else if (boot_reason_charge_only()){

    	LOGD("MOTO_PUPD: boot_reason_charge_only: 1");
        property_set("sys.chargeonly.mode", "1");

    }else if (check_data_12m()){

        LOGD("MOTO_PUPD: mot_boot_mode 12m: 1");
        property_set("tcmd.12m.test", "1");
        property_set("tcmd.suspend", "1");

    }else{

       	LOGD("MOTO_PUPD: mot_boot_mode 12m: 0");
       	property_set("tcmd.suspend", "0");
    }

    return 0;
}
