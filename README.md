<div align="center">
  <a href="https://github.com/JMU-Underwater/rov-master-v2"><img src="https://zengwangfa.oss-cn-shanghai.aliyuncs.com/rov/rovmaster(vector)1.png" alt=""></a>
  <a href="https://github.com/JMU-Underwater/rov-master-v2"><h1>ROV-Master-V2</h2></a>
</div>



<div align="center">
  <a href="http://wiki.friendlyarm.com/wiki/index.php/NanoPi_NEO_Core/zh"><img src="https://img.shields.io/badge/Device-Nanopi NEO Core-brigreen.svg?style=flat-square" alt="Device"></a>
</div>

# 1.介绍

- 本项目用到以下库
  - [WiringNP](https://github.com/friendlyarm/WiringNP) This is a GPIO access library for NanoPi. It is based on the WiringOP for Orange PI which is based on original WiringPi for Raspberry Pi.
  - 。。。

# 2.使用

- 运行环境： NanoPi NEO

  ```shell
  $ git clone https://github.com/JMU-Underwater/rov-master-v2.git
  or 
  $ git clone git@github.com:JMU-Underwater-Robot-Laboratory/rov-master-v2.git
  $ cd rov-master
  $ chmod +x build.sh
  $ ./build.sh
  $ make
  ```

- 若执行时提示无法打开某设备，输入 `sudo npi-config` > `Advanced Options` 中使能相关设备：

  ```shell
  $ sudo npi-config
  ```

  若系统时间不对，为时区未进行更改：
  在 `npi-config` > `Localisation Options` > `Change Timezone` 中选择修改。

# 3.协议

## 3.1 设备状态信息(仅发送)

数据说明：

| 字段           | 类型   | 描述               |
| -------------- | ------ | ------------------ |
| 温度           | float  | 当前cpu温度        |
| 航向角         | float  | 当前航向角         |
| depth_lock     | double | 当前与期望深度差值 |
| direction_lost | float  | 当前航向角/360     |

实时发送上位机所需的机器人状态信息：

```json
{
	"info":{  
		"温度"  :"28℃", //当前温度
		"航向角":"0°"    //当前航向角
	},
	"feedbacks":{
		"control_loops":{
			"depth_lock"    :-1, //当前与期望深度差值
			"direction_lost":1   //当前当前航向角/360
		},
	}
}
```

## 3.2 上位机信息(接收并发送)

数据说明：

| 字段                               | 类型   | 描述               |
| ---------------------------------- | ------ | ------------------ |
| x                                  | double | 控制机器人左右平移 |
| y                                  | double | 控制机器人前进后退 |
| z                                  | double | 控制机器人上浮下沉 |
| depth                              | float  | 期望深度           |
| set_propeller_pwm_freq_calibration | float  | 设置pca9685频率    |
| catch                              | double | 控制机械臂         |
| direction_locked                   | bool   | 设置方向锁定       |
| depth_locked                       | bool   | 设置深度锁定       |
| set_debug_mode_enabled             | bool   | 调试模式标志       |
| set_propeller_values               | short  | 设置推进器输出     |
| set_propeller_parameters           | -      | 设置推进器参数     |
| set_control_loop_parameters        | double | 设置控制环参数     |
| save_parameters                    | -      | 保存参数标志       |
| load_parameters                    | -      | 读取参数标志       |

接收并解析上位机发送的数据，并在调试模式时发送推进器参数给上位机

```json
{
	"x"    :0.0,  //左右平移
	"y"    :0.0,  //前进后退
	"z"    :0.0,  //上浮下沉
	"rot"  :0.0,  //期望航向角速度-1.0~1.0
	"depth":0.0,  //期望深度
	"set_propeller_pwm_freq_calibration":0.0, //pca9685频率
	"catch"           :0.0,     //机械臂
	"direction_locked":0,       //方向锁定
	"depth_locked"    :0,       //深度锁定
	"set_debug_mode_enabled":0, //调试模式标志
	"set_propeller_values":{    //推进器输出
		"back_left"  :0,        //推进器名称
		"center_left":0,   
		"front_left" :0    
		//...
	},
	"set_propeller_parameters":{  //推进器参数
		"back_left":{             //推进器名称
			"deadzone_upper":0,   //死区上限
			"deadzone_lower":0,   //死区下限
			"enabled"       :0,   //推进器使能
			"reversed"      :0,   //反转标志
			"power_negative":0.0, //正向动力百分比
			"power_positive":0.0  //反向动力百分比
		},
		"center_left":{
			//...
		},
		"front_left":{ 
			//...
		}
		//...
	},
	"set_control_loop_parameters":{ //控制环参数
		"depth_lock":{      //深度控制环
			"p":0.0,        //比例调节
			"i":0.0,        //积分调节
			"d":0.0         //微分调节
		},
		"direction_lock":{  //航向角控制环
			//...
		},
	},
	"save_parameters":0, //保存数据标志
	"load_parameters":0  //加载数据标志
}
```

---
#    4.进度
- 驱动模块
	- [ ] ADS1118 (ADC)
		- [ ] 电压检测 (待实际测试)
		- [ ] 电流检测 (待实际测试)
	- [x] PCA9685 (PWM)
	- [x] 深度传感器
		- [x] MS5837
	- [ ] 系统设备状态
		- [ ] CPU使用率、温度
		- [ ] 内存状态
		- [ ] 硬盘状态
		- [ ] 实时网速
	- [ ] IO设备
		- [ ] RGB
		- [ ] 按键
		- [ ] 蜂鸣器
	- [ ] PWM设备 (待实际测试)
		- [ ] 云台
		- [ ] 探照灯
		- [x] 机械臂
	- [ ] OLED
	- [x] 九轴(仅使用6轴功能)
	- [ ] 音频MIC
- 功能模块
	- [ ] OLED状态显示
	- [ ] 视频推流
	- [x] 数据通信
		- [x] 下行-控制数据
		- [ ] 上行-ROV状态数据
	- [ ] 系统自检
	- [x] 日志记录
	- [x] 传感器融合
- 控制模块
	- [x] 手柄控制 
	- [x] 定深控制
	- [x] 定向控制
	- [ ] 变焦摄像头控制
# 架构图

![](Pngs/mind.png)

