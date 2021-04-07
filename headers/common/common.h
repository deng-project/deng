/*    ____         ________     __      _        ______  
 *   |  _ \_      |  ______|   |  \    | |     _/  ____\
 *   | | \_ \_    |  |         |   \   | |    /  _/   
 *   | |   \  |   |  |_____    | |\ \  | |   |  |   _____ 
 *   | |    | |   |  ______|   | | \ \ | |   |  |  |___  | 
 *   | |  _/ _|   |  |         | |  \ \| |    \  \    / /
 *   | |_/ _/     |  |_____    | |   \   |     \_ \__/ /
 *   |____/       |________|   |_|    \__|       \____/
 *               
 * __________________________________________________________
 * __________________________________________________________
 *                       Project DENG 
 *
 *
 * Copyright (C) 2020 - 2021
 * This Software is licensed under Apache License as described 
 * in the LICENSE.md file, which you should have recieved with 
 * this distribution.
 * 
 * You may reproduce and distribute copies of the
 * Work or Derivative Works thereof in any medium, with or without
 * modifications, and in Source or Object form, provided that You
 * meet the following conditions:
 *
 * (a) You must give any other recipients of the Work or
 *     Derivative Works a copy of this License; and
 *
 * (b) You must cause any modified files to carry prominent notices
 *     stating that You changed the files; and
 *
 * (c) You must retain, in the Source form of any Derivative Works
 *     that You distribute, all copyright, patent, trademark, and
 *     attribution notices from the Source form of the Work,
 *     excluding those notices that do not pertain to any part of
 *     the Derivative Works; and
 *
 * (d) If the Work includes a "NOTICE" text file as part of its
 *     distribution, then any Derivative Works that You distribute must
 *     include a readable copy of the attribution notices contained
 *     within such NOTICE file, excluding those notices that do not
 *     pertain to any part of the Derivative Works, in at least one
 *     of the following places: within a NOTICE text file distributed
 *     as part of the Derivative Works; within the Source form or
 *     documentation, if provided along with the Derivative Works; or,
 *     within a display generated by the Derivative Works, if and
 *     wherever such third-party notices normally appear. The contents
 *     of the NOTICE file are for informational purposes only and
 *     do not modify the License. You may add Your own attribution
 *     notices within Derivative Works that You distribute, alongside
 *     or as an addendum to the NOTICE text from the Work, provided
 *     that such additional attribution notices cannot be construed
 *     as modifying the License.
 *
 * You may add Your own copyright statement to Your modifications and
 * may provide additional or different license terms and conditions
 * for use, reproduction, or distribution of Your modifications, or
 * for any such Derivative Works as a whole, provided Your use,
 * reproduction, and distribution of the Work otherwise complies with
 * the conditions stated in this License.
 */ 


#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Universal macro for getting the size of an array
 */
#define C_ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))

#ifdef __COMMON_C
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h> 
    #include <common/cerr_def.h>
    #include <common/base_types.h>
    #define __DENG_LOG_INIT_MSG "#ENTRY POINT\n";

    // Date and time formating
    #define __SECONDS_PER_YEAR 31536000 
    #define __SECONDS_PER_LEAP_YEAR 31622400
    #define __SECONDS_PER_31_DAY_MONTH 2678400
    #define __SECONDS_PER_30_DAY_MONTH 2592000
    #define __SECONDS_IN_FEBRUARY 2419200
    #define __SECONDS_IN_LEAP_YEAR_FEBRUARY 2505600

    void __cm_FindAllSubstringsRec();

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


/*
 * Make all the chars into upper case letters
 */
char *cm_MakeUpperCase(const char *str); 


/*
 * Make all chars into lower case
 */
char *cm_MakeLowerCase(const char *str);


/*
 * Replace all instances of find_ch in str
 * with rep_ch
 */
char *cm_StrReplaceCh (
    const char *str,
    char find_ch,
    char rep_ch
);


/* 
 * Extract the file name from the file path
 */
char *cm_ExtractFileName(char *path);


/* 
 * Get the file extension from file name
 */
char *cm_ExtractFileExtName(char *file_name);


// Inefficient ! ! !  
/*********************************************************/
/************* Universally applicable algorithms *********/
/*********************************************************/

/*
 * Specify information about date formating
 */
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


/*
 * Specify information about time formating
 */
typedef enum TimeFormat {
    TIME_FORMAT_24_HOUR_SP_FULL_STOP    = 0,
    TIME_FORMAT_24_HOUR_SP_COLON        = 1,
    TIME_FORMAT_12_HOUR_SP_FULL_STOP    = 2,
    TIME_FORMAT_12_HOUR_SP_COLON        = 3
} TimeFormat;


/*
 * Store information about date and time
 */
typedef struct TimeInfo {
    deng_ui16_t days;
    deng_ui16_t months;
    deng_ui16_t years;

    deng_ui8_t hours;
    deng_ui8_t minutes;
    deng_ui8_t seconds;
} TimeInfo;


/*
 * Convert time from epoch into TimeInfo struct
 */
void cm_ConvertTime (
    deng_i64_t epocht,
    TimeInfo *p_out_info
);


/*
 * Find all instances of sub_str from str and
 * put their character positions into postions array
 * This function uses heap allocation on dereferenced p_pos 
 * argument. That memory has to be freed afterwards
 */
void cm_FindAllSubstrings (
    char *str, 
    char *sub_str,
    deng_ui32_t **p_ind,
    deng_ui32_t *p_ind_c
);


/* 
 * Format time from epoch according to the specified
 * DateFormat and TimeFormat. Time from epoch is measured
 * in seconds. Output strings need to be valid pointers to
 * char buffer memory
 */
void cm_FormatTimeFromEpoch (
    DateFormat date_format,
    TimeFormat time_format,
    deng_i64_t epocht,
    char **p_out_date,
    char **p_out_time
);


/* 
 * Sort all the strings in alphabetical order
 */
void cm_SortAlphabetically(char **buffer, size_t buffer_count);


// Log writers
void cm_OpenLogger(char* file_name);
void cm_LogWrite(const char *content);
void cm_CloseLogger();


#ifdef __cplusplus
}
#endif

#endif
