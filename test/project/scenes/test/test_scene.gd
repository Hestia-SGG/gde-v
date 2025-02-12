extends Node2D

var memory_device: RVMemoryDevice
@onready var hart: ABIHart = $ABIHart

const default_offset = 0x80000000
const mem_max = 0x100000000

func int32_to_uint32(value: int):
	var temp: PackedByteArray
	temp.resize(4)
	temp.encode_s32(0, value)
	var to_ret = temp.decode_u32(0)
	return to_ret

func test_abi_call(called_hart: ABIHart):
	print("HERE!")
	called_hart.hart_state.clear_pending_exception()
	called_hart.mret(true, true)
	pass

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
		elif sys_call == 255:
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

func _ready():
	var test_rom = RVMemoryDevice.load_rom_file("res://assets/roms/example.bin.txt")
	var _read_out = test_rom.read(0, 8);
	var _hart_add_res = hart.add_bus_device(test_rom, default_offset)
	var os_rom = RVMemoryDevice.load_rom_file("C:/Users/gwent/Documents/gde-v/test/project/assets/roms/os_funcs.bin.txt")
	var os_rom_offset = mem_max-os_rom.get_size()
	_hart_add_res = hart.add_bus_device(os_rom, os_rom_offset)
	_read_out = os_rom.read(0, 8);
	
	hart.reset_hart(default_offset)
	#set to user mode
	hart.hart_state.priv = 0
	
	hart.machine_system_call.connect(test_sys_call)
	hart.user_system_call.connect(test_user_sys_call)
	hart.illegal_instruction.connect(illegal_instruction_handler)
	hart.except_load_fault.connect(illegal_instruction_handler)
	hart.pending_brk.connect(test_abi_call)
	
	pass

func _process(_delta):
	var _debug_hart_state = hart.hart_state
	if(hart.hart_state.exited):
		set_process(false)
		return
	var step_ret = hart.step_hart()
	print(step_ret)
	pass
