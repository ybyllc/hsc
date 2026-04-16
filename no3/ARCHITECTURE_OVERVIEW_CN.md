# FC_DC50_IC 架构总览（中文版）

## 1. 项目定位

- 这是 DC50 仪表的 MCU 固件工程。
- 当前运行模式是**裸机主循环轮询**，不是 FreeRTOS 任务调度。
- 工程中保留了 FreeRTOS 目录与示例代码，但默认启动路径未启用 `rtos_start()`。

## 2. 分层架构

### 启动与内存布局

- 启动与中断向量：`Startup/startup_FC4150.S`
- 链接脚本：`Startup/FC4150_flash.ld`
- FC4150(512K) 下 Flash 分区：
  - `BOOTLOADER`：32K，起始 `0x00000000`
  - `UPDATEBOOT`（Boot1）：148K，起始 `0x00008000`
  - `APP`：332K，起始 `0x0002D000`

### 驱动/BSP 层（`App/drv`）

- 包含时钟、GPIO、DMA、UART、CAN、ADC、WDG、Flash、睡眠唤醒、I2C/SPI/IIS/LIN 等外设封装。
- 常见模式是“中断/DMA回调只搬运数据到 FIFO，业务解析放到主循环任务中”。

### 协议/中间层

- CAN 矩阵与信号解析：`App/d_can/can_matrix/*`
- 网络管理与诊断：`App/nw/AutoSar_NM.c`、`App/nw/can_obd.c`、`App/nw/iso15765_2.c`
- 串口协议收发与分发：`App/cmd/p_cmd.c`、`App/cmd/p_cmd_def_K211.c`
- 串口上报映射与打包：`App/cmd/uart_matrix.c`

### 应用层

- 主状态机：`App/main_state.c`
- 电源与电压管理：`App/pm.c`
- ACC 检测：`App/acc_check.c`
- 里程与 EEPROM 持久化：`App/mileage_calc.c`、`App/p_eeprom/p_eeprom.c`
- 心跳与健康监测：`App/heartbeat_check.c`

## 3. 主运行流程

入口：`main.c`

1. 初始化阶段：
   - 时钟/SysTick/HSM/GPIO/DMA/调试串口/UART/ADC
   - NM FIFO 初始化 + NM 初始化
   - 心跳、EEPROM、ACC/音频、看门狗
2. 主循环阶段（顺序轮询）：

```text
main_state_run
cmd_val_task
drv_adc_task
drv_gpio_ck_task
ACC_check_task
audio_app_task
mileage_task
power_manage_task
mcu_cmd_task
uart_report_task
drv_uart_send_task
can_decoder_task
can_sign_lose_check_task
cm_can_send_task
nw_loop_task
heartbeat_ck_task
drv_can_busoff_tsak
can_nm_decoder_task
周期性 uart_ICU_Vehicle_cycle_report
```

## 4. 三条关键数据链路

### CAN 业务链路

1. CAN 中断/DMA 回调收帧并写入 FIFO。
2. `can_decoder_task()` 从业务 FIFO 取帧。
3. `cm_put_data()` 按 CAN Matrix 解析信号并回调。
4. 信号驱动业务状态更新或串口上报。

### NM/UDS 链路

1. NM/UDS 报文写入独立 FIFO。
2. `can_nm_decoder_task()` 读取该 FIFO。
3. NM 报文走 `can_nw_process()`，UDS 报文走 `can_uds_process()`。
4. `nw_loop_task()` 执行 AUTOSAR NM 状态机和 UDS 诊断循环。

### UART 链路

1. UART RX DMA + 空闲中断把字节流写入 RX FIFO。
2. `mcu_cmd_task()` 解析帧、校验 CRC、分发命令处理函数。
3. `uart_report_task()` 组织周期/事件上报数据。
4. `drv_uart_send_task()` 通过 DMA 发送 TX FIFO 数据。

## 5. 状态机与电源控制

### 主状态机（`main_state_run`）

- 关键状态：`MS_INIT`、`MS_NORMAL`、`MS_UPDATE`、`MS_UDS_UPDATE`、`MS_SLEEP`、`MS_REBOOT` 等。
- 负责升级、睡眠、重启等系统级切换，以及外设关闭顺序控制。

### 电源管理（`power_manage_task`）

- 由“电压状态机 + 电源模式状态机”组成。
- 输入：ACC、电压状态、网络状态。
- 输出：模式上报、心跳行为、进入睡眠触发。

## 6. 升级架构

- 升级主逻辑：`App/updata/fw_update.c`
- 支持主机命令升级与 UDS 升级路径。
- 通过备份区 + Flag 管理升级过程。
- 跳转 Boot API：`App/drv/d_iap.c` 的 `APP_JumpToBootloader()`

## 7. 构建与打包

- 构建系统：Eclipse CDT 自动生成 Makefile（`Debug_FLASH/`）。
- 工具链：`arm-none-eabi-gcc/g++`，Cortex-M4F，硬浮点，`-O2`。
- 链接脚本来自 `Startup/FC4150_flash.ld`。
- 后处理脚本 `pack_bin.py` 会合并 bootloader + boot1 + app，并生成升级包到 `bin/`。

## 8. 新人建议阅读顺序

1. `main.c`
2. `App/user_inc.h`
3. `App/main_state.c`
4. `App/drv/d_can.c`
5. `App/d_can/can_matrix/can_matrix.c`
6. `App/nw/AutoSar_NM.c`
7. `App/cmd/p_cmd.c`
8. `App/cmd/uart_matrix.c`
9. `App/pm.c`
10. `App/updata/fw_update.c`

## 9. 常见问题快速定位

- 主循环卡顿/顺序问题：`main.c`
- CAN 收帧路由：`App/drv/d_can.c`（`can1_transfer_complete_proc`）
- CAN 信号映射错误：`App/d_can/can_matrix/*`
- 串口协议 CRC/解包问题：`App/cmd/p_cmd.c`
- 休眠/重启异常：`App/main_state.c` + `App/pm.c`
- UDS 诊断行为异常：`App/nw/can_obd.c` + `App/nw/iso15765_2.c`

