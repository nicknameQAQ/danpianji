#ifndef display_driver
#define display_driver

void initialize_LCD1602();
void refresh_str(char*str,unsigned int Add);
void display_result(double*num,unsigned char*str,unsigned int add);

#endif