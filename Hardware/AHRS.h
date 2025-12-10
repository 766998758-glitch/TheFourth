#ifndef __AHRS_H
#define __AHRS_H

// 角度结构体
typedef struct{
		float pitch;  // 俯仰
		float yaw;		// 偏航
		float roll;   // 横滚
}Angles_t;   //名叫Angles_t的模板


void AHRS_Init(void);   //初始化
void AHRS_Update(int16_t acc[3], int16_t gyro[3], float dt);  //角度更新（互补滤波）
Angles_t AHRS_GetAngles(void);  //获取角度

float AHRS_GetYawBias(void);
void AHRS_SetYawBias(float bias);
void AHRS_ResetYaw(void);


#endif
