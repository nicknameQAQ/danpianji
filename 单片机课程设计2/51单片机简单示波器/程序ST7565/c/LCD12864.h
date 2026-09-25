

void write_data(unsigned char dat);
void write_command(unsigned char cmd);

void init_st7565(void);
void write_char8(unsigned char page,unsigned char column,unsigned char chr);
void write_string8(unsigned char page,unsigned char column,unsigned char *chr);
void disp_clear(void);
void disp_grid(unsigned char gridmode);
void disp_osc_settings(void);
void dispwave(unsigned char gridmode);
void dispwave2(unsigned char gridmode);
void dispchar16(unsigned char page,unsigned char column,unsigned char hz,unsigned char n,unsigned char *p);

