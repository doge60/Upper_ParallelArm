/*
 * @Author: doge60 3020118317@qq.com
 * @Date: 2024-04-16 19:35:37
 * @LastEditors: doge60 3020118317@qq.com
 * @LastEditTime: 2024-05-02 18:41:11
 * @FilePath: \Upper_ParallelArm\User\Arm\StateMachine\Arm_StateMachine.c
 * @Description: 机械臂状态机
 * 
 * Copyright (c) 2024 by doge60 3020118317@qq.com, All Rights Reserved. 
 */
#include "Arm_StateMachine.h"

ARM_MOVING_STATE ArmControl;
ARM_MOVING_STATE ArmState;

/**
 * @brief: 状态机线程
 * @return {*}
 */
void Arm_StateMachine_Task(void *argument)
{
    osDelay(1000);
    for (;;) {
        vPortEnterCritical();  //进入临界段（大概是因为遥控通信过程是不可打断的，所以将其放在一个临界段里）
        Remote_t RemoteCtl_RawData_tmp = RemoteCtl_RawData;
        vPortExitCritical();  //退出临界段
        switch (RemoteCtl_RawData_tmp.choice) {
            case Store:
                xSemaphoreTakeRecursive(ArmControl.xMutex_control, portMAX_DELAY);
                ArmControl.position.x = 0;
                ArmControl.position.y = -240;
                ArmControl.position.z = 50;
                xSemaphoreGiveRecursive(ArmControl.xMutex_control);
                break;
            case Catch:
                xSemaphoreTakeRecursive(ArmControl.xMutex_control, portMAX_DELAY);
                DeadBandOneDimensional(RemoteCtl_RawData_tmp.x , &(ArmControl.position.x), 0.05);
                DeadBandOneDimensional(RemoteCtl_RawData_tmp.y , &(ArmControl.position.y), 0.05);
                DeadBandOneDimensional(RemoteCtl_RawData_tmp.z , &(ArmControl.position.z), 0.05);
                xSemaphoreGiveRecursive(ArmControl.xMutex_control);
                break;
            case Wait:
                xSemaphoreTakeRecursive(ArmControl.xMutex_control, portMAX_DELAY);
                ArmControl.position.x = 0;
                ArmControl.position.y = 200;
                ArmControl.position.z = 0;
                xSemaphoreGiveRecursive(ArmControl.xMutex_control);
                break;
            default:
                
                break;
        }
        vTaskDelay(2);
    }
}

/**
 * @brief: 状态机线程启动
 * @return {*}
 */
void Arm_StateMachine_TaskStart()
{
    osThreadId_t Arm_StateMachine_TaskHandle;
    const osThreadAttr_t Arm_StateMachine_Task_attributes = {
    .name = "Arm_StateMachine_Task",
    .stack_size = 128 * 10,
    .priority = (osPriority_t) osPriorityHigh,
    };
    Arm_StateMachine_TaskHandle = osThreadNew(Arm_StateMachine_Task, NULL, &Arm_StateMachine_Task_attributes);  //cmsis v2线程启动似乎只需要这个函数，待测试
}

/**
 * @brief 初始化状态机
 * @return {*}
 */

void Arm_StateMachine_Init()
{
    ArmControl.xMutex_control = xSemaphoreCreateRecursiveMutex();
    ArmState.xMutex_control   = xSemaphoreCreateRecursiveMutex();
    JointComponent.xMutex_joint   = xSemaphoreCreateRecursiveMutex();
}

void Arm_SteerinfMotorCorrect(){
} //??从学长代码里copy过来就是这样一个空函数，不懂