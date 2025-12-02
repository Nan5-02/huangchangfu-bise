#ifndef __MQ2_H_
#define __MQ2_H_

void calibrateMQ2();
float readGas(const struct MQCurve &curve);

void MQ2_update();
float MQ2_getData();

#endif