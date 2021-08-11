/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: common.h - DENG common C language functionality header
/// author: Karl-Mihkel Ott


#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define C_ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define ZERO_MEM(buf)   memset(buf, 0, sizeof(buf));


/// Calculate the remaining number needed to ceil given number x to have zero reminder when dividing with divider div
#define ZERO_MOD_CEIL_REM(x, div) ((div - (x % div)) == div ? 0 : (div - (x % div)))

#ifdef __COMMON_C
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h> 
	#include <wchar.h>

    #include <common/cerr_def.h>
    #include <common/base_types.h>
    #define __DENG_LOG_INIT_MSG "#ENTRY POINT\n"

	#ifdef _WIN32
		#include <windows.h>
		#include <wincrypt.h>
	#endif

    // Date and time formating
    #define __SECONDS_PER_YEAR 31536000 
    #define __SECONDS_PER_LEAP_YEAR 31622400
    #define __SECONDS_PER_31_DAY_MONTH 2678400
    #define __SECONDS_PER_30_DAY_MONTH 2592000
    #define __SECONDS_IN_FEBRUARY 2419200
    #define __SECONDS_IN_LEAP_YEAR_FEBRUARY 2505600

    /// Substring finder algorithm core function
    void __cm_SubstrAl (
        char *str,
        size_t str_len,
        char *sub_str,
        size_t substr_len,
        deng_ui32_t **p_ind,
        deng_ui32_t *p_ind_c
    );

    // This is needed when date and time formating requires it
    char *__month_names[] = {
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"
    };
#endif



/***********************************************/
/************* String functions ****************/
/***********************************************/


/// Find the total line count from beg to rlen
/// rlen must be less than the amount of memory under beg
deng_ui32_t cm_FindLineCount(char *str, size_t rlen);


/// Make all the chars into upper case letters
char *cm_MakeUpperCase(const char *str); 


/// Make all chars into lower case
char *cm_MakeLowerCase(const char *str);


/// Replace all instances of find_ch in str with rep_ch
char *cm_StrReplaceCh (
    const char *str,
    char find_ch,
    char rep_ch
);


/// Extract the file name from the file path
char *cm_ExtractFileName(char *path);


/// Get the file extension from file name
char *cm_ExtractFileExtName(char *file_name);


/*********************************************************/
/************* Universally applicable algorithms *********/
/*********************************************************/

/// Specify information about date formating
typedef enum DateFormat {
    // The style Japanese and Chinese use
    DATE_FORMAT_YMD_SP_FULL_STOP    = 0,
    DATE_FORMAT_YMD_SP_OBLIQUE      = 1,
    DATE_FORMAT_YMD_SP_HYPHEN       = 2,
    DATE_FORMAT_YMD_SP_SPACE        = 3,

    // The style most of the world uses
    DATE_FORMAT_DMY_SP_FULL_STOP    = 4,
    DATE_FORMAT_DMY_SP_OBLIQUE      = 5,
    DATE_FORMAT_DMY_SP_HYPHEN       = 6,
    DATE_FORMAT_DMY_SP_SPACE        = 7,

    // The style America uses
    DATE_FORMAT_MDY_SP_FULL_STOP    = 8,
    DATE_FORMAT_MDY_SP_OBLIQUE      = 9,
    DATE_FORMAT_MDY_SP_HYPHEN       = 10,
    DATE_FORMAT_MDY_SP_SPACE        = 11
} DateFormat;


/// Specify information about time formating
typedef enum TimeFormat {
    TIME_FORMAT_24_HOUR_SP_FULL_STOP    = 0,
    TIME_FORMAT_24_HOUR_SP_COLON        = 1,
    TIME_FORMAT_12_HOUR_SP_FULL_STOP    = 2,
    TIME_FORMAT_12_HOUR_SP_COLON        = 3
} TimeFormat;


/// Store information about date and time
typedef struct TimeInfo {
    deng_ui16_t days;
    deng_ui16_t months;
    deng_ui16_t years;

    deng_ui8_t hours;
    deng_ui8_t minutes;
    deng_ui8_t seconds;
} TimeInfo;


/// Convert time from epoch into TimeInfo struct
void cm_ConvertTime (
    deng_i64_t epocht,
    TimeInfo *p_out_info
);


/// This function calculates the total chuck size from the required memory size
deng_ui64_t cm_FindChunkSize(deng_ui64_t chunk, deng_ui64_t req_mem);


/// The following functions are used to get nearest number which has power of 2
deng_ui64_t cm_ToPow2I64(deng_ui64_t n);
deng_ui32_t cm_ToPow2I32(deng_ui32_t n);
deng_ui16_t cm_ToPow2I16(deng_ui16_t n);
deng_ui8_t cm_ToPow2I8(deng_ui8_t n);


/// The following functions are used to generate cryptographically secure random numbers
deng_ui64_t cm_RandI64();
deng_ui32_t cm_RandI32();
deng_ui16_t cm_RandI16();
deng_ui8_t cm_RandI8();


/// Find all instances of sub_str from str and put their character positions into postions array
/// This function uses heap allocation on dereferenced p_ind argument. That memory has to be freed afterwards
void cm_FindAllSubstrings (
    char *str, 
    char *sub_str,
    deng_ui32_t **p_ind,
    deng_ui32_t *p_ind_c
);


/// Find all instances of sub_str from str while reading maximum of str_len character from str and substr_len from sub_str.
/// This function uses heap allocation on dereferenced p_ind argument. That memory has to be freed afterwards
void cm_FindnAllSubstrings (
    char *str,
    size_t str_len,
    char *sub_str,
    size_t substr_len,
    deng_ui32_t **p_ind,
    deng_ui32_t *p_ind_c
);


/// Format time from epoch according to the specified DateFormat and TimeFormat. Time from epoch is measured
/// in seconds. Output strings need to be valid pointers to char buffer memory
void cm_FormatTimeFromEpoch (
    DateFormat date_format,
    TimeFormat time_format,
    deng_i64_t epocht,
    char **p_out_date,
    char **p_out_time
);


// Log writer functions
void cm_OpenLogger(const char* file_name, const deng_bool_t overwrite);
const char *cm_GetLogFile();
void cm_LogWrite(const char *content);
void cm_CloseLogger();


#ifdef __cplusplus
}
#endif

#endif
