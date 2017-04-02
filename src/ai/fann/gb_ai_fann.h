#ifndef __GB_AI_H__
#define __GB_AI_H__

int gb_ai_init(int width, int height, float window_zoom, const char *rom_path);
void gb_ai_main(const char *nn_path);
void gb_ai_tr_main(const char *tr_path);
void gb_ai_finish(void);

#endif /* __GB_AI_H__ */
