#ifndef MY_DS18B20_H_
#define MY_DS18B20_H_

void DS_init(void);
void DS_process(void);
float DS_getTemp(void);
int DS_getTargetTemp(void);
void DS_setTargetTemp(int temp);

#endif 
