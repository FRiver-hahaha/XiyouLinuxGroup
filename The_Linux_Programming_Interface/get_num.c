/* get_num.c */
#include <limits.h>

int
getInt(const char *arg, int flags, const char *name)
{
    long res;
    
    res = getLong(arg, flags, name);  /* 调用 getLong() 做实际转换 */
    
    if (res > INT_MAX || res < INT_MIN)
        cmdLineErr("getInt: %s (%ld) not in integer range", name, res);
        
    return (int) res;
}