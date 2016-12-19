#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef DEBUG
#define PRINTD(format, ...) printf(format, ##__VA_ARGS__)
#else
#define PRINTD(format, args...) ((void) 0)
#endif

#endif /* __UTILS_H__ */
