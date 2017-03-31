#ifndef __GB_AI_H__
#define __GB_AI_H__

int gb_ai_init(int width, int height, float window_zoom, const char *rom_path);
int gb_ai_load(const char *filename);
void gb_ai_main(const char *ai_path);
void gb_ai_step(void);
void gb_ai_tr_main(const char *tr_path);
void gb_ai_finish(void);

#endif /* __GB_AI_H__ */
