# 2025-04-04T10:35:41.990488600
import vitis

client = vitis.create_client()
client.set_workspace(path="captsone")

platform = client.create_platform_component(name = "platform",hw_design = "C:\Users\komlj\Downloads\design_1_wrapper.xsa",os = "standalone",cpu = "ps7_cortexa9_0",domain_name = "standalone_ps7_cortexa9_0")

platform = client.get_component(name="platform")
status = platform.build()

comp = client.create_app_component(name="lwip_test",platform = "$COMPONENT_LOCATION/../platform/export/platform/platform.xpfm",domain = "standalone_ps7_cortexa9_0")

status = platform.build()

comp = client.get_component(name="lwip_test")
comp.build()

client.delete_component(name="lwip_test")

domain = platform.get_domain(name="zynq_fsbl")

status = domain.set_lib(lib_name="lwip220", path="C:\Xilinx_2\Vitis\2024.2\data\embeddedsw\ThirdParty\sw_services\lwip220_v1_1")

status = domain.regenerate()

status = domain.set_config(option = "lib", param = "XILTIMER_en_interval_timer", value = "true", lib_name="xiltimer")

status = domain.set_config(option = "lib", param = "XILTIMER_en_interval_timer", value = "false", lib_name="xiltimer")

status = platform.build()

vitis.dispose()

