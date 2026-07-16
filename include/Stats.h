#ifndef STATS_H
#define STATS_H

void  Stats_Init();
void  Stats_Update();
float Stats_GetCPU();      // percent 0-100
float Stats_GetRAM();      // MB
int   Stats_GetRAMPercent();

#endif
