#include "Stats.h"
#include <cstdio>
#include <unistd.h>
#include <sys/time.h>

static long prevCPUTime  = 0;
static long prevTotalTime = 0;
static float cpuPercent = 0.0f;
static float ramMB = 0.0f;
static int ramPercent = 0;

static long ReadCPUTime() {
    FILE* f = fopen("/proc/self/stat", "r");
    if (!f) return 0;
    long utime, stime, cutime, cstime;
    fscanf(f, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld %ld %ld",
           &utime, &stime, &cutime, &cstime);
    fclose(f);
    return utime + stime + cutime + cstime;
}

static long ReadTotalCPU() {
    FILE* f = fopen("/proc/stat", "r");
    if (!f) return 0;
    long user, nice, system, idle, iowait, irq, softirq;
    fscanf(f, "cpu %ld %ld %ld %ld %ld %ld %ld",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    fclose(f);
    return user + nice + system + idle + iowait + irq + softirq;
}

static void ReadRAM() {
    FILE* f = fopen("/proc/self/status", "r");
    if (!f) return;
    char line[256];
    long vmrss = 0;
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "VmRSS: %ld", &vmrss) == 1) break;
    }
    fclose(f);
    ramMB = vmrss / 1024.0f;

    FILE* mf = fopen("/proc/meminfo", "r");
    if (!mf) return;
    long total = 0, available = 0;
    while (fgets(line, sizeof(line), mf)) {
        if (sscanf(line, "MemTotal: %ld", &total) == 1) continue;
        if (sscanf(line, "MemAvailable: %ld", &available) == 1) break;
    }
    fclose(mf);
    if (total > 0) ramPercent = (int)((1.0f - (float)available / (float)total) * 100.0f);
}

void Stats_Init() {
    prevCPUTime = ReadCPUTime();
    prevTotalTime = ReadTotalCPU();
    ReadRAM();
}

void Stats_Update() {
    long curCPUTime = ReadCPUTime();
    long curTotalTime = ReadTotalCPU();

    long dCPU = curCPUTime - prevCPUTime;
    long dTotal = curTotalTime - prevTotalTime;
    if (dTotal > 0)
        cpuPercent = (float)dCPU / (float)dTotal * 100.0f;

    prevCPUTime = curCPUTime;
    prevTotalTime = curTotalTime;

    ReadRAM();
}

float Stats_GetCPU()       { return cpuPercent; }
float Stats_GetRAM()       { return ramMB; }
int   Stats_GetRAMPercent() { return ramPercent; }
