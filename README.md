# C615_rp2040_test
大疆C615电调的测试程序，基于Arduino平台下的RP2040芯片完成，可无缝使用平台下的其他芯片驱动

可通过串口指令实现开机初始化自动校准、设定指定速度或加速度、线性或S型加速模式等

PS：其实就是舵机驱动方式，给定PWM信号即可，具体信号值请见调参软件曲线设置

该程序为赛后开源代码 供驱动测试临时使用不保证可靠性


如需使用大疆调参软件建议购买：FT232 (队友说一次连接成功)、CP2102 (玄学连接，1-30分钟不等，请等待机魂大悦)

CH340因未知原因从未连接成功...（；´д｀）ゞ
