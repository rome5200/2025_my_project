# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct C:\repo\SoC_Test\test3_led_pmod\vitis_proj\soc_pmod\platform.tcl
# 
# OR launch xsct and run below command.
# source C:\repo\SoC_Test\test3_led_pmod\vitis_proj\soc_pmod\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {soc_pmod}\
-hw {C:\repo\SoC_Test\test3_led_pmod\vitis_proj\system_wrapper.xsa}\
-proc {ps7_cortexa9_0} -os {standalone} -out {C:/repo/SoC_Test/test3_led_pmod/vitis_proj}

platform write
platform generate -domains 
platform active {soc_pmod}
platform generate
platform generate
