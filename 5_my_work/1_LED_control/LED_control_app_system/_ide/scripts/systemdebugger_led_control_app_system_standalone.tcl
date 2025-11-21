# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: C:\repo\my_project\5_my_work\1_LED_control\LED_control_app_system\_ide\scripts\systemdebugger_led_control_app_system_standalone.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source C:\repo\my_project\5_my_work\1_LED_control\LED_control_app_system\_ide\scripts\systemdebugger_led_control_app_system_standalone.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent Zybo Z7 210351BD2319A" && level==0 && jtag_device_ctx=="jsn-Zybo Z7-210351BD2319A-23727093-0"}
fpga -file C:/repo/my_project/5_my_work/1_LED_control/LED_control_app/_ide/bitstream/system_wrapper.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw C:/repo/my_project/5_my_work/1_LED_control/LED_control/export/LED_control/hw/system_wrapper.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source C:/repo/my_project/5_my_work/1_LED_control/LED_control_app/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow C:/repo/my_project/5_my_work/1_LED_control/LED_control_app/Debug/LED_control_app.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
