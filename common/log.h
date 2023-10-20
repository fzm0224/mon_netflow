#ifndef _LYF_LOG_
#define _LYF_LOG_


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdarg.h>


#define FLAG_DEBUG           4
#define FLAG_INFO            2
#define FLAG_ERROR           1


#define LYF_LOG(flag, fmt, args...)  \
    do{         \
        lyf_log(flag, "[%s:%d]" fmt"\n", __FUNCTION__, __LINE__, ##args);   \
    }while(0)   \


static void lyf_log(int flag, const char *fmt, ...) {
	va_list args;
	FILE *fdb;
	struct tm *tstruct;
	time_t tsec;
	struct stat ST;

    int check_flag = 0;
    if (!access("./release/log/log_error_flag", F_OK)) {
        check_flag |= FLAG_ERROR; 
    }
    if (!access("./release/log/log_info_flag", F_OK)) {
        check_flag |= FLAG_ERROR | FLAG_INFO;  
    }
    if (!access("./release/log/log_debug_flag", F_OK)) {
        check_flag |= FLAG_ERROR | FLAG_INFO | FLAG_DEBUG;  
    }

    if (flag & check_flag) {
        if (0 == stat("./release/log/log_file", &ST)) {
            if (ST.st_size > 2 * 1024 * 1024) {
                fdb = fopen("./release/log/log_file", "w");
            } else {
                fdb = fopen("./release/log/log_file", "a");
            }
        } else {
            fdb = fopen("./release/log/log_file", "w+");
        }

        if (!fdb) {
            return;
        }

        tsec = time(NULL);
        tstruct = localtime(&tsec);

        if (FLAG_ERROR == flag) {
            fprintf(fdb, "[ERROR][%04d-%02d-%02d %02d:%02d:%02d]", tstruct->tm_year + 1900, tstruct->tm_mon + 1,
                tstruct->tm_mday, tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec);
        } else if (FLAG_INFO == flag) {
            fprintf(fdb, "[INFO][%04d-%02d-%02d %02d:%02d:%02d]", tstruct->tm_year + 1900, tstruct->tm_mon + 1,
                tstruct->tm_mday, tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec);
        } else if (FLAG_DEBUG == flag) {
            fprintf(fdb, "[DEBUG][%04d-%02d-%02d %02d:%02d:%02d]", tstruct->tm_year + 1900, tstruct->tm_mon + 1,
                tstruct->tm_mday, tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec);
        }

        va_start(args, fmt);
        vfprintf(fdb, fmt, args);
        va_end(args);
        fclose(fdb);
    } else if (0 == check_flag) {
        tsec = time(NULL);
        tstruct = localtime(&tsec);

        if (FLAG_ERROR == flag) {
            fprintf(stdout, "[ERROR][%04d-%02d-%02d %02d:%02d:%02d]", tstruct->tm_year + 1900, tstruct->tm_mon + 1,
                tstruct->tm_mday, tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec);
        } else if (FLAG_INFO == flag) {
            fprintf(stdout, "[INFO][%04d-%02d-%02d %02d:%02d:%02d]", tstruct->tm_year + 1900, tstruct->tm_mon + 1,
                tstruct->tm_mday, tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec);
        } else if (FLAG_DEBUG == flag) {
            fprintf(stdout, "[DEBUG][%04d-%02d-%02d %02d:%02d:%02d]", tstruct->tm_year + 1900, tstruct->tm_mon + 1,
                tstruct->tm_mday, tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec);
        }

        va_start(args, fmt);
        vfprintf(stdout, fmt, args);
        va_end(args);
    }

    return;
}


#endif