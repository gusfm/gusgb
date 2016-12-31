#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef DEBUG
#define printd(format, ...) printf(format, ##__VA_ARGS__)
#else
#define printd(format, ...) ((void)0)
#endif

#endif /* __UTILS_H__ */
