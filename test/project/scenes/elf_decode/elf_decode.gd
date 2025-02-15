class_name ElfDecode extends Node2D

var elf_file = "res://assets/roms/testing"

func int32_to_uint32(value: int):
	var temp: PackedByteArray
	temp.resize(4)
	temp.encode_s32(0, value)
	var to_ret = temp.decode_u32(0)
	return to_ret

func test_brk_call(called_hart: ABIHart):
	var param = called_hart.hart_state.get_register_by_name("a0")
	var handled = false
	if param == 0:
		called_hart.hart_state.set_register_by_name("a0", called_hart.heap_address)
		handled = true
	else:
		var hart_device : RVHeapDevice = called_hart.get_bus_device(called_hart.heap_address)
		var new_size = param - called_hart.heap_address
		if hart_device and new_size > 0:
			hart_device.underlying_mem_size = new_size
			called_hart.hart_state.set_register_by_name("a0", param)
			handled = true
	
	if not handled:
		called_hart.hart_state.set_register_by_name("a0", 0)
	
	called_hart.hart_state.clear_pending_exception()
	called_hart.mret(true, true)

func test_sys_call(called_hart: RVHart):
	if called_hart.hart_state.mcause == RVHart.RVExceptions.EMECALL or \
		called_hart.hart_state.mcause == RVHart.RVExceptions.ESECALL or \
		called_hart.hart_state.mcause == RVHart.RVExceptions.EUECALL:
		var sys_call = called_hart.hart_state.registers[17]
		if sys_call == 0:
			#var str_ptr = called_hart.hart_state.registers[10]
			var str_ptr = int32_to_uint32(called_hart.hart_state.registers[10])
			var size = int32_to_uint32(called_hart.hart_state.registers[11])
			var ret: Variant = called_hart.bus_read(str_ptr, size)
			if typeof(ret) == TYPE_PACKED_BYTE_ARRAY:
				var bytes = ret as PackedByteArray
				var to_str = bytes.get_string_from_utf8()
				print("Read in: " + to_str)
			else:
				print("Invalid print call! " + str(ret))
		#elif sys_call == 255:
		else:
			called_hart.hart_state.exited = true
			pass
	var _test_hart_state = called_hart.hart_state
	called_hart.hart_state.registers[3] = 324
	called_hart.hart_state.registers[13] = 567
	called_hart.hart_state.clear_pending_exception()
	called_hart.mret(true, true)
	pass

func test_user_sys_call(called_hart: RVHart):
	print("User!")
	test_sys_call(called_hart)

func illegal_instruction_handler(called_hart: RVHart):
	called_hart.hart_state.exited = true

@onready var hart: ABIHart = $ABIHart

func _ready():
	var elf_file_data = FileAccess.get_file_as_bytes(elf_file)
	
	if FileAccess.get_open_error() != OK:
		print("File open fail!")
		return
	
	var elf_file_in = ElfFile.load_elf_data(elf_file_data)
	print(str(elf_file_in))
	
	var os_rom = RVMemoryDevice.load_rom_file("res://assets/roms/os_funcs.bin.txt")
	var os_rom_offset = 0x1000 - os_rom.get_size()
	var _os_rom_add_res = hart.add_bus_device(os_rom, os_rom_offset)
	hart.mret_address = os_rom_offset
	
	var test_devices = elf_file_in.generate_memory_areas()
	for cur_device_addr in test_devices:
		var _hart_add_res = hart.add_bus_device(test_devices[cur_device_addr], cur_device_addr)
		pass
	
	var stack_start_addr = 0x04000000
	var stack_end_local_addr = (16*1024)
	var _stack_end_addr = stack_start_addr + stack_end_local_addr
	var heap_start_addr = 0x03000000
	
	var heap_min_size = 128*1024
	var stack_min_size = 16*1024
	
	var stack_device_addr = hart.create_stack_device(stack_min_size, stack_start_addr)
	var _stack_device : RVMemoryDevice = hart.get_bus_device(stack_device_addr)
	
	var heap_device_addr = hart.create_heap_device(heap_min_size, heap_start_addr)
	var _heap_device : RVHeapDevice = hart.get_bus_device(heap_device_addr)
	
	hart.reset_hart(elf_file_in.header.entry_point)
	hart.hart_state.priv = 0
	
	var _param_addr = hart.encode_parameters(["testing", "abc"])
	
	hart.machine_system_call.connect(test_sys_call)
	hart.user_system_call.connect(test_user_sys_call)
	hart.illegal_instruction.connect(illegal_instruction_handler)
	hart.load_page_fault.connect(illegal_instruction_handler)
	hart.except_load_fault.connect(illegal_instruction_handler)
	hart.pending_brk.connect(test_brk_call)
	
	var _debug = hart.bus_read(elf_file_in.header.entry_point, 12)
	
	var fs_dispatcher = VirtFSDispatcher.new()
	pass

func perform_hart_step():
	var _debug_hart_state = hart.hart_state
	if(hart.hart_state.exited):
		set_process(false)
		return false
	var cur_pc = hart.hart_state.pc
	var step_ret = hart.step_hart()
	var to_print = "0x%x: %d" % [cur_pc, step_ret]
	print(to_print)
	return true

func _process(_delta):
	for i in range(10):
		if not perform_hart_step(): return
	pass
