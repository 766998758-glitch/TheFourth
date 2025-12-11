#include "stm32f10x.h"   
#include "AHRS.h"
#include "math.h"

static Angles_t angles = {0, 0, 0};  //全局角度变量，先初始化0

//互补滤波系数
#define GYRO_WEIGHT 0.97f  //陀螺仪权重
#define ACC_WEIGHT 0.03f   //加速度计权重

//模拟量转化成显示物理量 - 传感器标度因子
#define GYRO_SCALE 65.5f  //±500°/s~131
#define ACC_SCALE 16384.0f  //±8g~16384
#define RAD_TO_DEG 57.295779513082320876798154814105f

//Yaw处理参数
#define YAW_LPF_ALPHA 0.95f  //Yaw低通滤波系数
#define YAW_DEADBAND 0.05f    //死区阈值

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
	static uint8_t init_done = 0;
    if(!init_done) {
        yaw_rate_filtered = gz_corrected;
        init_done = 1;
    } else {
        yaw_rate_filtered = YAW_LPF_ALPHA * yaw_rate_filtered
                          + (1.0f - YAW_LPF_ALPHA) * gz_corrected;
    }
	
	//3.死区处理
	if(fabs(yaw_rate_filtered) < YAW_DEADBAND){ return yaw_angle;}
	
	//4.积分得到角度
	yaw_angle += yaw_rate_filtered * dt;
	
	//5.角度标准化0-360
	while(yaw_angle >= 360.0f) yaw_angle -= 360.0f;
  while(yaw_angle < 0.0f) yaw_angle += 360.0f;
	
	return yaw_angle;
}
/**********对Yaw的低通滤波处理***********/




void AHRS_Update(int16_t acc_raw[3], int16_t gyro_raw[3], float dt)
{	
	//0.验证数据范围
    for(int i = 0; i < 3; i++) {
        if(acc_raw[i] > 32767 || acc_raw[i] < -32768) {
            //数据溢出就不更新
            return;
        }
    }
	
		//1.转化成物理单位
		float ax = acc_raw[0] / ACC_SCALE;
		float ay = acc_raw[1] / ACC_SCALE;
		float az = acc_raw[2] / ACC_SCALE;

		float gx = gyro_raw[0] / GYRO_SCALE;
		float gy = gyro_raw[1] / GYRO_SCALE;
		float gz = gyro_raw[2] / GYRO_SCALE;

    float norm = sqrt(ax*ax + ay*ay + az*az);   //计算g
		
    if(norm > 0.1f) {  //避免除0
			//归一化
        ax /= norm;
        ay /= norm;
        az /= norm;
        
        //2.计算pitch和roll
        float pitch_acc = atan2(-ax, sqrt(ay*ay + az*az)) * RAD_TO_DEG;
        float roll_acc = atan2(ay, az) * RAD_TO_DEG;
        
        //3.p和r的互补滤波
        //陀螺仪积分
        float gyro_pitch = angles.pitch + gy * dt;
        float gyro_roll = angles.roll + gx * dt;
			
        //融合
        angles.pitch = GYRO_WEIGHT * gyro_pitch + ACC_WEIGHT * pitch_acc;
        angles.roll = GYRO_WEIGHT * gyro_roll + ACC_WEIGHT * roll_acc;
        
        //限制角度范围
				if(angles.pitch > 180.0f) angles.pitch -= 360.0f;
				if(angles.pitch < -180.0f) angles.pitch += 360.0f;
				if(angles.roll > 180.0f) angles.roll -= 360.0f;
				if(angles.roll < -180.0f) angles.roll += 360.0f;
    }

    //4.上方的低通滤波对yaw简单处理
		 angles.yaw = Yaw_SimpleUpdate(gz, dt);
}

Angles_t AHRS_GetAngles(void)
{
	return angles;
}

//API函数
float AHRS_GetYawBias(void) { return yaw_bias; }
void AHRS_SetYawBias(float bias) { yaw_bias = bias; }
void AHRS_ResetYaw(void) { 
    yaw_angle = 0.0f; 
    angles.yaw = 0.0f; 
}







