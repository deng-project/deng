/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: common.c - DENG common C language functionality implementation
/// author: Karl-Mihkel Ott


#define __COMMON_C
#include <common/common.h>

static FILE* __file;
static const char *__o_file;


/***********************************************/
/************* String functions ****************/
/***********************************************/

/// Replace all chars in string with other char
char *cm_StrReplaceCh (
    const char *str,
    char find_ch,
    char rep_ch
) {
    char *out_str = (char*) str;
    for(size_t i = 0; i < strlen(out_str); i++) {
        if(out_str[i] == find_ch)
            out_str[i] = rep_ch;
    }

    return out_str;
}


/// Get the file extension name 
char *cm_ExtractFileExtName(char *file_name) {
    deng_i64_t l_index;
    deng_bool_t is_ext_found = false;
    
    // Find the extension index
    for(l_index = strlen(file_name) - 1; l_index >= 0; l_index--) {
        if(file_name[l_index] == '.' && l_index < (deng_i64_t) strlen(file_name) - 1) {
            is_ext_found = true;
            l_index++;
            break;
        }
    }

    if(!is_ext_found) return NULL;
    return file_name + l_index;
}


/*******************************************************************************/
/* The following functions are used to get nearest number which has power of 2 */
/*******************************************************************************/

deng_ui64_t cm_ToPow2I64(deng_ui64_t n) {
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;

    return n + 1;
}


deng_ui32_t cm_ToPow2I32(deng_ui32_t n) {
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;

    return n + 1;
}


deng_ui16_t cm_ToPow2I16(deng_ui16_t n) {
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;

    return n + 1;
}


deng_ui8_t cm_ToPow2I8(deng_ui8_t n) {
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;

    return n + 1;
}


/************************************************/
/* The following functions are used to generate */
/* random cryptographycally secure numbers      */
/************************************************/

deng_ui64_t cm_RandI64() {
    deng_ui64_t out = 0;
	#if defined(__linux__)
        // Read random data from /dev/urandom
        FILE* file;
        file = fopen("/dev/urandom", "rb");
        if (!file) FILE_ERR("/dev/urandom");

        size_t res = fread(&out, sizeof(deng_ui64_t), 1, file);
        if(!res) FILE_ERR("/dev/urandom");
        fclose(file);
    #elif defined(_WIN32)
		HCRYPTPROV crypt_prov = { 0 };
        if (!CryptAcquireContext(&crypt_prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
            RUN_ERR("cm_randi64()", "Failed to acquire WIN32 cryptographic context");
        if(!CryptGenRandom(crypt_prov, sizeof(deng_ui64_t), (BYTE*) &out))
            RUN_ERR("cm_randi64()", "Failed to generate random cryptgraphically secure number");
	#endif

	return out;
}


deng_ui32_t cm_RandI32() {
    deng_ui64_t tmp = cm_RandI64();
    return *(deng_ui32_t*) &tmp;
}


deng_ui16_t cm_RandI16() {
    deng_ui64_t tmp = cm_RandI64();
    return *(deng_ui16_t*) &tmp;
}


deng_ui8_t cm_RandI8() {
    deng_ui64_t tmp = cm_RandI64();
    return *(deng_ui8_t*) &tmp;
}


/// Convert time from epoch into TimeInfo struct
void cm_ConvertTime (
    deng_i64_t epocht,
    TimeInfo *p_out_info
) {
    deng_i64_t s_offset = __SECONDS_PER_YEAR;
    p_out_info->years = 1970;
    p_out_info->months = 1;
    p_out_info->days = 2;

    // Find the total amount of years
    while(epocht - s_offset > 0) {
        p_out_info->years++;
        epocht -= s_offset;

        // Check if the next year is leap year
        if(p_out_info->years - 1 > 0 && !((p_out_info->years - 1) % 4)) 
            s_offset = __SECONDS_PER_LEAP_YEAR;
        else s_offset = __SECONDS_PER_YEAR;
    }

    // Find the total amount of remaining months
    s_offset = __SECONDS_PER_31_DAY_MONTH;
    while(epocht - s_offset > 0) {
        p_out_info->months++;
        epocht -= s_offset;

        // Check if how many days does next month have
        if(p_out_info->months + 1 != 2 && !(p_out_info->months % 2) && p_out_info->months + 1 != 8)
            s_offset = __SECONDS_PER_30_DAY_MONTH;
        else if(p_out_info->months + 1 == 2 && !((p_out_info->years - 2) % 4))
            s_offset = __SECONDS_IN_LEAP_YEAR_FEBRUARY;
        else if(p_out_info->months + 1 == 2 && (p_out_info->years - 2) % 4)
            s_offset = __SECONDS_IN_FEBRUARY;
        else s_offset = __SECONDS_PER_31_DAY_MONTH;
    }

    // Find the total amount of remaining days
    s_offset = 86400;
    while(epocht - s_offset > 0) {
        p_out_info->days++;
        epocht -= s_offset;
    }

    // Find all the remaining hours
    s_offset = 3600;
    while(epocht - s_offset > 0) {
        p_out_info->hours++;
        epocht -= s_offset;
    }

    // Find all the remaining minutes
    s_offset = 60;
    while(epocht - s_offset > 0) {
        p_out_info->minutes++;
        epocht -= s_offset;
    }

    p_out_info->seconds = (deng_ui8_t) epocht;
}


/// This function calculates the total chuck size from the required memory size
deng_ui64_t cm_FindChunkSize(deng_ui64_t chunk, deng_ui64_t req_mem) {
    return req_mem + (req_mem % chunk > 0 ? chunk - (req_mem % chunk) : 0);
}


/// Substring finder algorithm core function
void __cm_SubstrAl (
    char *str,
    size_t str_len,
    char *sub_str,
    size_t substr_len,
    deng_ui32_t **p_ind,
    deng_ui32_t *p_ind_c
) {
    deng_ui32_t ind_cap = 16;

    (*p_ind) = (deng_ui32_t*) calloc (
        ind_cap,
        sizeof(deng_ui32_t)
    );
    (*p_ind_c) = 0;

    deng_ui32_t m, i;
    deng_ui32_t rep = UINT32_MAX;

    // Find all common repeating substring sequences
    for(m = 0, i = 0; m < str_len; i++, m++) {
        // Add substring location to indices and redo the loop
        if(i >= substr_len - 1) {
            (*p_ind_c)++;
            // Check if memory reallocation is needed
            if((*p_ind_c) >= ind_cap) {
                ind_cap <<= 1;
                deng_ui32_t *tmp = (deng_ui32_t*) calloc (
                    ind_cap,
                    sizeof(deng_ui32_t)
                );

                if(!tmp) MEM_ERR("indices");

                (*p_ind) = tmp;
            }

            (*p_ind)[(*p_ind_c) - 1] = m - i; 
            rep = UINT32_MAX;
            i = 0;
        }

        // Check if current main string character matches substring first character
        if(str[m] == sub_str[0])
            rep = m;

        // Check if characters do not match and if they don't then set i to 0
        if(sub_str[i] != str[m]) {
            // No repeated characters found
            if(rep == UINT32_MAX) {
                m += i;
                i = 0;
            }
            // Found repeated character 
            else {
                i = 0;
                m = rep;
                rep = UINT32_MAX;
            }
        }
    }
}


/*
 * Find all instances of sub_str from str and
 * put their character positions into postions array
 * This function uses heap allocation on dereferenced p_pos 
 * argument. That memory has to be freed afterwards
 */
void cm_FindAllSubstrings (
    char *str, 
    char *sub_str, 
    deng_ui32_t **p_pos, 
    deng_ui32_t *p_pos_c
) {
    __cm_SubstrAl (
        str,
        strlen(str),
        sub_str,
        strlen(sub_str),
        p_pos,
        p_pos_c
    );
}


/*
 * Find all instances of sub_str from str while reading maximum of str_len character from str
 * and substr_len from sub_str.
 * This function uses heap allocation on dereferenced p_ind
 * argument. That memory has to be freed afterwards
 */
void cm_FindnAllSubstrings (
    char *str,
    size_t str_len,
    char *sub_str,
    size_t substr_len,
    deng_ui32_t **p_ind,
    deng_ui32_t *p_ind_c
) {
    __cm_SubstrAl (
        str, 
        str_len, 
        sub_str, 
        substr_len, 
        p_ind, 
        p_ind_c
    );
}


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
) {
    TimeInfo ti = {0};
    cm_ConvertTime(epocht, &ti);
    char strm[4] = {0};
    char strd[4] = {0};

    static char date[16] = {0};
    static char time[16] = {0};

    if(ti.months < 10)
        sprintf(strm, "0%d", ti.months);
    else sprintf(strm, "%d", ti.months);

    if(ti.days < 10)
        sprintf(strd, "0%d", ti.days);
    else sprintf(strd, "%d", ti.days);

    // Time formating 
    switch(date_format)
    {
    // Day, month, year format
    case DATE_FORMAT_DMY_SP_FULL_STOP:
        sprintf (
            date,
            "%s.%s.%d",
            strd,
            strm,
            ti.years
        );
        break;

    case DATE_FORMAT_DMY_SP_OBLIQUE:
        sprintf (
            date,
            "%s/%s/%d",
            strd,
            strm,
            ti.years
        );
        break;

    case DATE_FORMAT_DMY_SP_HYPHEN:
        sprintf (
            date,
            "%s-%s-%d",
            strd,
            strm,
            ti.years
        );
        break;

    case DATE_FORMAT_DMY_SP_SPACE:
        sprintf (
            date,
            "%s %s %d",
            strd,
            __month_names[ti.months - 1],
            ti.years
        );
        break;

    // Month, day, year format
    case DATE_FORMAT_MDY_SP_FULL_STOP:
        sprintf (
            date,
            "%s.%s.%d",
            strm,
            strd,
            ti.years
        );
        break;

    case DATE_FORMAT_MDY_SP_OBLIQUE:
        sprintf (
            date,
            "%s/%s/%d",
            strm,
            strd,
            ti.years
        );
        break;

    case DATE_FORMAT_MDY_SP_HYPHEN:
        sprintf (
            date,
            "%s-%s-%d",
            strm,
            strd,
            ti.years
        );
        break;

    case DATE_FORMAT_MDY_SP_SPACE:
        sprintf (
            date,
            "%s %s %d",
            __month_names[ti.months - 1],
            strd,
            ti.years
        );
        break;

    // Year, month, day format
    case DATE_FORMAT_YMD_SP_FULL_STOP:
        sprintf (
            date,
            "%d.%s.%s",
            ti.years,
            strm,
            strd
        );
        break;

    case DATE_FORMAT_YMD_SP_OBLIQUE:
        sprintf (
            date,
            "%d/%s/%s",
            ti.years,
            strm,
            strd
        );
        break;

    case DATE_FORMAT_YMD_SP_HYPHEN:
        sprintf (
            date,
            "%d-%s-%s",
            ti.years,
            strm,
            strd
        );
        break;

    case DATE_FORMAT_YMD_SP_SPACE:
        sprintf (
            date,
            "%d %s %s",
            ti.years,
            __month_names[ti.months - 1],
            strd
        );
        break;

    default:
        break;
    }

    // Time formating
    switch(time_format) 
    {
    case TIME_FORMAT_24_HOUR_SP_COLON:
        if(ti.minutes < 10) {
            sprintf (
                time,
                "0%d:%d",
                ti.hours,
                ti.minutes
            );
        }

        else {
            sprintf (
                time,
                "%d:%d",
                ti.hours,
                ti.minutes
            );
        }
        break;

    case TIME_FORMAT_24_HOUR_SP_FULL_STOP:
        if(ti.minutes < 10) {
            sprintf (
                time,
                "0%d.%d",
                ti.hours,
                ti.minutes
            );
        }

        else {
            sprintf (
                time,
                "%d.%d",
                ti.hours,
                ti.minutes
            );
        }
        break;

    case TIME_FORMAT_12_HOUR_SP_COLON: {
        char *tc = ti.hours <= 12 ? "AM" : "PM";
        ti.hours = ti.hours % 13 + 1;
        if(ti.hours < 10) {
            sprintf (
                time,
                "0%d:%d %s",
                ti.hours,
                ti.minutes,
                tc
            );
        }
        else {
            sprintf (
                time,
                "%d:%d %s",
                ti.hours,
                ti.minutes,
                tc
            );
        }
        break;
    }

    case TIME_FORMAT_12_HOUR_SP_FULL_STOP: {
        char *tc = ti.hours <= 12 ? "AM" : "PM";
        ti.hours = ti.hours % 13 + 1;
        if(ti.hours < 10) {
            sprintf (
                time,
                "0%d.%d %s",
                ti.hours,
                ti.minutes,
                tc
            );
        }
        else {
            sprintf (
                time,
                "%d.%d %s",
                ti.hours,
                ti.minutes,
                tc
            );
        }
        break;
    }

    default:
        break;
    }

    (*p_out_date) = date;
    (*p_out_time) = time;
}


/// Extract file name from file path
char *cm_ExtractFileName(char *path) {
    deng_i64_t i = 0;
    for(i = strlen(path) - 1; i >= 0; i++) {
        if(path[i] == '/' || path[i] == '\\')
            break;
    }
    
    return path + i;
}


/// Find the total line count from beg to rlen
/// rlen must be less than the amount of memory under beg
deng_ui32_t cm_FindLineCount(char *str, size_t rlen) {
    deng_ui32_t lc = 1;

    // For each character check if it is newline
    for(size_t i = 0; i < rlen; i++)
        if(str[i] == 0x0A) lc++;

    return lc;
}


/// Make all lowercase chars into uppercase chars 
char *cm_MakeUpperCase(const char *str) {
    char *out_str = (char*) str;
    
    for(size_t i = 0; i < strlen(out_str); i++) {
        if(out_str[i] > 0x60 && out_str[i] < 0x7B) 
            out_str[i] -= 0x20;
    }

    return out_str;
}


/// Make all uppercase chars into lowercase chars
char *cm_MakeLowerCase(const char *str) {
    char *out_str = (char*) str;
    for(size_t i = 0; i < strlen(str); i++) {
        if(out_str[i] > 0x40 && out_str[i] < 0x5B)
            out_str[i] += 0x20;
    }

    return out_str;
}

#ifdef __DEBUG
/// Open new file for logging (write only)
void _cm_OpenLogger(const char* file_name, const deng_bool_t overwrite) {
    if (overwrite) __file = fopen(file_name, "wb");
    else __file = fopen(file_name, "ab");

    __o_file = file_name;
    if (!__file) fprintf(stderr, "Failed to open file '%s'\n", file_name);

    if (overwrite) {
        const char* msg = __DENG_LOG_INIT_MSG;
        fwrite(msg, sizeof(char), strlen(msg), __file);
    }
}


/// Check if logging is active on some file
const char *_cm_GetLogFile() {
    return __o_file;
}


/// Close logger FILE instance 
void _cm_CloseLogger() {
    fclose(__file);
    __file = NULL;
    __o_file = NULL;
}


/// Write content into log file 
void _cm_LogWrite(const char *content) {
    fwrite(content, sizeof(const char), strlen(content), __file);
    fwrite("\n", sizeof(char), 1, __file);
}
#endif
