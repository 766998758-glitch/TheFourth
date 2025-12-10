#include "stm32f10x.h"   
#include "AHRS.h"
#include "math.h"

static Angles_t angles = {0, 0, 0};  //全局角度变量，先初始化0

//互补滤波系数
#define GYRO_WEIGHT 0.98f  //陀螺仪权重
#define ACC_WEIGHT 0.02f   //加速度计权重

//模拟量转化成显示物理量 - 传感器标度因子
#define GYRO_SCALE 131.0f  //±250°/s~131
#define ACC_SCALE 16384.0f  //±2g~16384

//Yaw处理参数
#define YAW_LPF_ALPHA 0.95f  //Yaw低通滤波系数
#define YAW_DEADBAND 0.1f    //死区阈值

//Yaw状态变量
static float yaw_angle = 0.0f;   //Yaw角度
static float yaw_rate_filtered;  //滤波后的角速度
static float yaw_bias;           //Yaw零偏

void AHRS_Init(void)
{
	angles.pitch = 0;
	angles.yaw = 0;
	angles.roll = 0;
	
	yaw_angle = 0.0f;
	yaw_rate_filtered = 0.0f;
	yaw_bias = 0.0f;    //牵扯到Calibrate.c的校准
}



/**********对Yaw的低通滤波处理**********/
static float Yaw_SimpleUpdate(float gz, float dt)
{
	//1.减去零偏（特别重要）
	float gz_corrected = gz - yaw_bias;
	
	//2.简单低通滤波
	yaw_rate_filtered = YAW_LPF_ALPHA * yaw_rate_filtered
											+ (1.0f - YAW_LPF_ALPHA) * gz_corrected;
	
	//3.死区处理
	if(fabs(yaw_rate_filtered) < YAW_DEADBAND){ return yaw_angle;}
	
	//4.积分得到角度
	yaw_angle += yaw_rate_filtered * dt;
	
	return yaw_angle;
}
/**********对Yaw的低通滤波处理***********/




void AHRS_Update(int16_t acc_raw[3], int16_t gyro_raw[3], float dt)
{	
		//1.转化成物理单位
		float ax = acc_raw[0] / ACC_SCALE;
		float ay = acc_raw[1] / ACC_SCALE;
		float az = acc_raw[2] / ACC_SCALE;

		float gx = gyro_raw[0] / GYRO_SCALE;
		float gy = gyro_raw[1] / GYRO_SCALE;
		float gz = gyro_raw[2] / GYRO_SCALE;

    //2.从加速度轴计算角度
		float pitch_acc = atan2(-ax, sqrt(ay*ay + az*az)) * 57.29578f;  //57.3=180/Π
		float roll_acc = atan2(ay, az) * 57.29578f;

		//3.只对p和r进行互补滤波(最简化版本)
		angles.pitch = GYRO_WEIGHT * (angles.pitch + gy * dt) + ACC_WEIGHT * pitch_acc;
		angles.roll = GYRO_WEIGHT * (angles.roll + gx * dt) + ACC_WEIGHT * roll_acc;

    //4.上方的低通滤波对yaw简单处理
		 angles.yaw = Yaw_SimpleUpdate(gz, dt);

    //5.保持Yaw别突破0/360°了
		if(angles.yaw > 360){angles.yaw -= 360;}
		if(angles.yaw < 0){angles.yaw += 360;}
}

Angles_t AHRS_GetAngles(void)
{
	return angles;
}

//API函数
float AHRS_GetYawBias(void) { return yaw_bias; }
void AHRS_SetYawBias(float bias) { yaw_bias = bias; }
void AHRS_ResetYaw(void) { yaw_angle = 0.0f; angles.yaw = 0.0f; }









