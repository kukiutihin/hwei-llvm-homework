#ifndef SIM_H
#define SIM_H

#define SIM_X_SIZE 1536  // 512
#define SIM_Y_SIZE 768   // 256

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __sim__
void simInit();
void app();
void simExit();
void simFlush();
void simPutPixel(int x, int y, int argb);
int simRand();
int simHasClick();
int simGetClick();
#endif  // __sim__

#ifdef __cplusplus
}
#endif

#endif  // SIM_H
