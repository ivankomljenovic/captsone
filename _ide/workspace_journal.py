# 2025-04-04T11:15:56.940532900
import vitis

client = vitis.create_client()
client.set_workspace(path="captsone")

platform = client.get_component(name="platform")
status = platform.build()

platform = client.create_platform_component(name = "design2",hw_design = "C:\Users\komlj\Downloads\design_2_wrapper.xsa",os = "standalone",cpu = "ps7_cortexa9_0",domain_name = "standalone_ps7_cortexa9_0")

platform = client.get_component(name="design2")
domain = platform.get_domain(name="zynq_fsbl")

status = domain.set_lib(lib_name="lwip220", path="C:\Xilinx_2\Vitis\2024.2\data\embeddedsw\ThirdParty\sw_services\lwip220_v1_1")

client.delete_component(name="platform")

domain = platform.add_domain(cpu = "ps7_cortexa9_1",os = "standalone",name = "test",display_name = "test",support_app = "lwip_tcp_perf_client")

domain = platform.get_domain(name="standalone_ps7_cortexa9_0")

status = domain.set_lib(lib_name="lwip220", path="C:\Xilinx_2\Vitis\2024.2\data\embeddedsw\ThirdParty\sw_services\lwip220_v1_1")

status = platform.build()

status = platform.build()

client.delete_component(name="design2")

platform = client.create_platform_component(name = "design",hw_design = "C:\Users\komlj\Downloads\design_2_wrapper.xsa",os = "standalone",cpu = "ps7_cortexa9_0",domain_name = "standalone_ps7_cortexa9_0")

platform = client.get_component(name="design")
status = domain.set_lib(lib_name="lwip220", path="C:\Xilinx_2\Vitis\2024.2\data\embeddedsw\ThirdParty\sw_services\lwip220_v1_1")

status = domain.set_config(option = "lib", param = "XILTIMER_en_interval_timer", value = "true", lib_name="xiltimer")

status = domain.set_config(option = "lib", param = "XILTIMER_tick_timer", value = "ps7_scutimer_0", lib_name="xiltimer")

status = domain.set_config(option = "lib", param = "XILTIMER_en_interval_timer", value = "false", lib_name="xiltimer")

status = domain.set_config(option = "lib", param = "XILTIMER_tick_timer", value = "None", lib_name="xiltimer")

