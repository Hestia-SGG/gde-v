#include "gde-v/hart.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

//------------- Static ------------------------------------------------------------------------

/**
 * Binds this class into Godot. See RVBusDevice::_bind_methods for more info.
 */
void RVHart::_bind_methods(){
	ClassDB::bind_method(D_METHOD("add_bus_device", "new_device", "offset"), &RVHart::add_bus_device);
	ClassDB::bind_method(D_METHOD("remove_bus_device_by_offset", "offset"), &RVHart::remove_bus_device_by_offset);
	ClassDB::bind_method(D_METHOD("remove_bus_device", "to_remove"), &RVHart::remove_bus_device);
	ClassDB::bind_method(D_METHOD("get_bus_device", "offset"), &RVHart::get_bus_device);

	ClassDB::bind_method(D_METHOD("bus_read", "address", "size"), &RVHart::bus_read);
	ClassDB::bind_method(D_METHOD("bus_write", "address", "data"), &RVHart::bus_write);

	ClassDB::bind_method(D_METHOD("reset_hart", "pc_start"), &RVHart::reset_hart, DEFVAL(0x80000000));
	ClassDB::bind_method(D_METHOD("step_hart"), &RVHart::step_hart);

	ClassDB::bind_method(D_METHOD("get_hart_state"), &RVHart::get_hart_state);
	ClassDB::bind_method(D_METHOD("set_hart_state", "new_state"), &RVHart::set_hart_state);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "hart_state", PropertyHint::PROPERTY_HINT_RESOURCE_TYPE, "RVHartState"), "set_hart_state", "get_hart_state");

	// These are signals for all the exceptions that might occur. ABIHart will attempt to call a custom
	// signal for the syscall asked for, and fall back to these signals.
	ADD_SIGNAL(MethodInfo("except_instruction_alignment", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("except_instruction_fault", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("illegal_instruction", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("breakpoint", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("except_load_alignment", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("except_load_fault", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("except_store_alignment", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("except_store_fault", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("user_system_call", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("supervisor_system_call", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("machine_system_call", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("instruction_page_fault", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("load_page_fault", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("store_page_fault", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	
	ADD_SIGNAL(MethodInfo("software_interrupt", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("timer_interrupt", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));
	ADD_SIGNAL(MethodInfo("external_interrupt", PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "RVHart")));

	BIND_ENUM_CONSTANT(EIALIGN)
    BIND_ENUM_CONSTANT(EIFAULT)
    BIND_ENUM_CONSTANT(EILL)
    BIND_ENUM_CONSTANT(EBP)
	BIND_ENUM_CONSTANT(ELALIGN)
    BIND_ENUM_CONSTANT(ELFAULT)
    BIND_ENUM_CONSTANT(ESALIGN)
    BIND_ENUM_CONSTANT(ESFAULT)
	BIND_ENUM_CONSTANT(EUECALL)
    BIND_ENUM_CONSTANT(ESECALL)
    BIND_ENUM_CONSTANT(EMECALL)
    BIND_ENUM_CONSTANT(EIPAGE)
	BIND_ENUM_CONSTANT(ELPAGE)
	BIND_ENUM_CONSTANT(ESPAGE)
    BIND_ENUM_CONSTANT(ETRAP_NONE)
    BIND_ENUM_CONSTANT(ETRAP_WFI)

	BIND_GET_SET(mret_address, RVHart, INT, "new_val")
	ClassDB::bind_method(D_METHOD("mret", "clear_vals", "increment_mepc"), &RVHart::mret);

	ClassDB::bind_method(D_METHOD("trigger_software_interrupt"), &RVHart::trigger_software_interrupt);
	ClassDB::bind_method(D_METHOD("clear_software_interrupt"), &RVHart::clear_software_interrupt);
	ClassDB::bind_method(D_METHOD("trigger_timer_interrupt"), &RVHart::trigger_timer_interrupt);
	ClassDB::bind_method(D_METHOD("clear_timer_interrupt"), &RVHart::clear_timer_interrupt);
	ClassDB::bind_method(D_METHOD("trigger_external_interrupt"), &RVHart::trigger_external_interrupt);
	ClassDB::bind_method(D_METHOD("clear_external_interrupt"), &RVHart::clear_external_interrupt);

	GDVIRTUAL_BIND(_handle_exception, "exception");

	BIND_GET_SET(stack_address, RVHart, INT, "new_address")
	BIND_GET_SET(heap_address, RVHart, INT, "new_address")
	ClassDB::bind_method(D_METHOD("create_stack_device", "size", "start_hint"), &RVHart::create_stack_device, DEFVAL(0xFFFF0000));
	ClassDB::bind_method(D_METHOD("create_heap_device", "size", "start_hint"), &RVHart::create_heap_device, DEFVAL(0xE0000000));

	ClassDB::bind_method(D_METHOD("encode_parameters", "data"), &RVHart::encode_parameters);
}

//------------- Member ------------------------------------------------------------------------

rv* RVHart::get_hart_device(){
	if(this->hart_state_valid()) return this->hart_state->get_hart_info();
	else return nullptr;
}

// Honestly just so we didn't have to keep copying this check everywhere.
bool RVHart::hart_state_valid(){
	return (!this->hart_state.is_null() && this->hart_state.is_valid());
}

/**
 * Tries to find a bus device close to the given offset. If not returns nullptr.
 */
Ref<RVBusDevice> RVHart::get_bus_device(int64_t offset){
	int64_t closest_offset = this->get_closest_device_offset(offset);
	if(closest_offset == -1) return nullptr;
	
	//paranoia, cast it as ref to make sure
	Ref<RVBusDevice> return_ref = this->bus_devices[closest_offset];
	return return_ref;
}

/**
 * Iterates through all registered devices, and find the one that overlaps the given address.
 */
int64_t RVHart::get_closest_device_offset(int64_t address){
	Array bus_device_keys = this->bus_devices.keys();
	for(uint64_t i = 0; i < bus_device_keys.size(); i++){
		uint64_t bus_offset = bus_device_keys[i];
		Ref<RVBusDevice> cur_bus = this->bus_devices[bus_offset];
		if(bus_offset <= address && (bus_offset + cur_bus->get_size()) > address) return bus_offset;
	}
    return -1;
}

/**
 * Tries to find an empty area of memory, and returns -1 if unable to do so.
 * 
 * Can probably be made simpler, but works for now.
 */
int64_t RVHart::find_empty_area(int64_t hint, int64_t size){
	Ref<RVBusDevice> test_device = nullptr;
	int64_t address_to_insert = hint;
	int64_t address_to_insert_end = address_to_insert + size;
	
	bool found = false;
	Array bus_device_keys = this->bus_devices.keys();

	// Try to find an empty area. Make sure we don't go beyonds the address space of
	// 32 bit. Should be changed to allow RV64.
	while(!found && address_to_insert_end <= UINT32_MAX){
		uint64_t test_overlap = this->get_closest_device_offset(address_to_insert);
		// A device was found that overlaps the start. Try again after it.
		if(test_overlap != -1){
			test_device = this->get_bus_device(test_overlap);
			address_to_insert += test_device->get_size();
			address_to_insert_end = address_to_insert + size;
			continue;
		}

		// If a device wasn't found, make sure that there isn't a device that matches up.
		// Tests to see if there is a device who starts in this range. Should handle all cases.
		// Can probably just do this check?
		for(int64_t i = 0; i < bus_device_keys.size(); i++){
			test_overlap = bus_device_keys[i];
			if(address_to_insert <= test_overlap && test_overlap <= address_to_insert_end){
				test_device = this->get_bus_device(test_overlap);
				address_to_insert += test_device->get_size();
				address_to_insert_end = address_to_insert + size;
				continue;
			}
		}

		// If we made it this far, the region is empty and big enough.
		found = true;
	}

	if(!found) return -1;
	else return address_to_insert;
}

/**
 * Tries to create a stack/heap device of the given size.
 * 
 * If a device of the given type is already defined, returns it, otherwise will try
 * to find a space big enough, create a new device of the given type, and return
 * the address to it after insert into the list of devices.
 */
int64_t RVHart::create_stack_heap_device(int64_t hint, int64_t size, bool create_heap){
	Ref<RVBusDevice> test_device = nullptr;
	//Check the specific variable based on if heap or stack
	int64_t* addr_variable_ptr = create_heap ? &(this->heap_address) : &(this->stack_address);

	if(*addr_variable_ptr != -1){
		test_device = this->get_bus_device(this->stack_address);
		if(!test_device.is_null()) return this->stack_address;
	}

	int64_t address_to_insert = this->find_empty_area(hint, size);
	if(address_to_insert == -1) return -1;
	int64_t end = address_to_insert + size;

	Ref<RVMemoryDevice> new_device;
	RVMemoryDevice* temp_device_ptr = nullptr;
	if(create_heap){
		temp_device_ptr = memnew(RVHeapDevice);
		*addr_variable_ptr = address_to_insert;
	} else{
		temp_device_ptr = memnew(RVMemoryDevice);
		*addr_variable_ptr = address_to_insert + size;
	}
	//new_device.instantiate();
	new_device.reference_ptr(temp_device_ptr);
	new_device->set_size(size);
	this->add_bus_device(new_device, address_to_insert);
	return address_to_insert;
}

/**
 * Handles all the given exceptions. TODO: Rewrite to how ABIHart::emit_syscall_signal works
 * so that it uses a String and then emits at the end.
 */
bool RVHart::_handle_exception(RVExceptions exception){
	if(exception != RVExceptions::ETRAP_NONE){
		switch(exception){
			case EIALIGN:
				this->emit_signal("except_instruction_alignment", this);
				break;
			case EIFAULT:
				this->emit_signal("except_instruction_fault", this);
				break;
			case EILL:
				this->emit_signal("illegal_instruction", this);
				break;
			case EBP:
				this->emit_signal("breakpoint", this);
				break;
			case ELALIGN:
				this->emit_signal("except_load_alignment", this);
				break;
			case ELFAULT:
				this->emit_signal("except_load_fault", this);
				break;
			case ESALIGN:
				this->emit_signal("except_store_alignment", this);
				break;
			case ESFAULT:
				this->emit_signal("except_store_fault", this);
				break;
			case EUECALL:
				this->emit_signal("user_system_call", this);
				break;
			case ESECALL:
				this->emit_signal("supervisor_system_call", this);
				break;
			case EMECALL:
				this->emit_signal("machine_system_call", this);
				break;
			case EIPAGE:
				this->emit_signal("instruction_page_fault", this);
				break;
			case ELPAGE:
				this->emit_signal("load_page_fault", this);
				break;
			case ESPAGE:
				this->emit_signal("store_page_fault", this);
				break;
			default:
				break;
		}

		//Exception wasn't cleared, return false
		if(this->hart_state->get_pending_exception() != RVExceptions::ETRAP_NONE) return false;
		else return true;
	}

	//if it made it down here, it succeeded
	return true;
}

/**
 * Tries to handle traps. Returns true if they were able to be handled, false if not.
 * 
 * This will call one of the signals based on what type of trap it is. For example, ELFAULT emits
 * except_load_fault. If any trap is still marked as pending or wasn't handled, it will return false
 * so the caller knows it's still an issue, or a new issue happened.
 */
bool RVHart::handle_traps(){
	RVExceptions cur_exception = this->hart_state->get_pending_exception();
	bool ret = false;
    if (GDVIRTUAL_CALL(_handle_exception, cur_exception, ret)) {
		//do nothing, it was handled through virtual call
	} else if(this->has_method("_handle_exception")){
        ret = this->call("_handle_exception", cur_exception);
    } else{
		ret = this->_handle_exception(cur_exception);
	}
	//Exception wasn't handled
	if(!ret) return false;

	if(this->hart_state->get_pending_interrupt()){
		if(this->hart_state->is_software_interrupt()){
			this->emit_signal("software_interrupt", this);
		} else if(this->hart_state->is_timer_interrupt()){
			this->emit_signal("timer_interrupt", this);
		} else if(this->hart_state->is_external_interrupt()){
			this->emit_signal("external_interrupt", this);
		}

		//Flag is still here, it wasn't handled
		if(this->hart_state->get_pending_interrupt()) return false;
	}
	
	//nothing stopping the running, return!
	return true;
}

/**
 * Triggers an interrupt in the underlying rv info.
 */
void RVHart::_trigger_interrupt(rv_cause type){
	if(!this->hart_state_valid()) return;

	rv_irq(this->get_hart_device(), type);
}

/**
 * Clears an interrupt in the underlying rv info.
 */
void RVHart::_clear_interrupt(rv_cause type){
	if(!this->hart_state_valid()) return;

	rv_u32 temp_mip = this->hart_state->get_mip();
	temp_mip &= ~((rv_u32)type);
	this->hart_state->set_mip(temp_mip);
}

bool RVHart::_set_special_address(int64_t* ptr, int64_t new_address){
	if(*ptr != -1){
		Ref<RVBusDevice> test_device = this->get_bus_device(*ptr);
		if(!test_device.is_null()) return false; // don't want to override without the user unmapping the current one
	}

	*ptr = new_address;
	return true;
}

RVHart::RVHart() {
	// Initialize any variables here.
	time_passed = 0.0;
	//REMEMBER, UINT32_MAX is 0xFFFFFFFF, so if we want an offset from memory end, we need to add one
	//or only subtract 7
	mret_address = UINT32_MAX-7;
	stack_address = -1;
	heap_address = -1;
}

RVHart::~RVHart() {
	
}

void RVHart::_process(double delta) {
	time_passed += delta;
}

/**
 * Adds a new bus device at the given offset if there isn't one already.
 * TODO: Add checks to make sure that the addresses dont' overlap, not just
 * that there isn't one with the same.
 */
Error RVHart::add_bus_device(Ref<RVBusDevice> new_device, int64_t offset){
	Ref<RVBusDevice> old_device = this->get_bus_device(offset);
	if(!old_device.is_null()) return Error::ERR_ALREADY_IN_USE;
	this->bus_devices[offset] = new_device;
    return Error::OK;
}

/**
 * Removes a device by offset. MUST BE EXACT.
 */
Error RVHart::remove_bus_device_by_offset(int64_t offset){
	Ref<RVBusDevice> old_device = this->get_bus_device(offset);
	if(old_device.is_null()) return Error::ERR_DOES_NOT_EXIST;

	this->bus_devices.erase(offset);
    return Error::OK;
}

/**
 * Removes a device by reference. Useful for if already have the device.
 */
Error RVHart::remove_bus_device(Ref<RVBusDevice> to_remove){
	Array bus_device_keys = this->bus_devices.keys();
	for(uint64_t i = 0; i < bus_device_keys.size(); i++){
		Ref<RVBusDevice> cur_device = this->bus_devices[bus_device_keys[i]];
		if(cur_device.is_null() || !cur_device.is_valid()) continue;

		if(to_remove == cur_device){
			this->bus_devices.erase(bus_device_keys[i]);
			return Error::OK;
		}
	}

    return Error::ERR_DOES_NOT_EXIST;
}

int64_t RVHart::create_stack_device(int64_t size, int64_t start_hint){
	//If a hint isn't given, put it at the top of memory. 
	if(start_hint == -1) start_hint = UINT32_MAX - size + 1;
	return this->create_stack_heap_device(start_hint, size, false);
}

void RVHart::set_stack_address(int64_t new_address){
	this->_set_special_address(&(this->stack_address), new_address);
}

int64_t RVHart::get_stack_address(){
	return this->stack_address;
}


int64_t RVHart::create_heap_device(int64_t size, int64_t start_hint){
	return this->create_stack_heap_device(start_hint, size, true);
}

void RVHart::set_heap_address(int64_t new_address){
	this->_set_special_address(&(this->heap_address), new_address);
}

int64_t RVHart::get_heap_address(){
	return this->heap_address;
}

/**
 * Performs a read on the bus.
 * 
 * Looks for a device that will be able to service this address, then call it's
 * underlying read. Returns either a slice of the data or RVMemResult.
 */
Variant RVHart::bus_read(int64_t address, int64_t size){
	int64_t offset = this->get_closest_device_offset(address);
	if(offset == -1) return RVMemResult::BAD;

	Ref<RVBusDevice> bus_device = this->get_bus_device(offset);
	if(bus_device.is_null() || !bus_device.is_valid()) return RVMemResult::BAD;
	
	int64_t true_address = address - offset;

	return bus_device->read(true_address, size);
}

/**
 * Performs a write on the bus.
 * 
 * Looks for a device that will be able to service this address, then call it's
 * underlying write. Returns RVMemResult of either OK or BAD usually.
 */
RVMemResult RVHart::bus_write(int64_t address, PackedByteArray data){
	int64_t offset = this->get_closest_device_offset(address);
	if(offset == -1) return RVMemResult::BAD;

	Ref<RVBusDevice> bus_device = this->get_bus_device(offset);
	if(bus_device.is_null() || !bus_device.is_valid()) return RVMemResult::BAD;
	
	int64_t true_address = address - offset;

	return bus_device->write(true_address, data);
}

/**
 * Resets the hart to a default state.
 * 
 * This will create a hart state if it doesn't exist or is invalid according to Ref.
 * Also sets the sp register if it's been set up. The user pointer in the underlying rv
 * struct will hold the pointer to this instance. 
 */
void RVHart::reset_hart(uint64_t pc_start){
	if(!this->hart_state_valid()){
		this->hart_state.instantiate();
	}
	
	rv* hart_device = this->get_hart_device();
	if(hart_device == nullptr) return;

	// Pass in the pointer to this as the user pointer, so the callback can get info from this class.
	rv_init(hart_device, (void*)this, rv_bus_access);
	hart_device->pc = pc_start;

	this->hart_state->set_exited(false);
	this->hart_state->clear_pending_exception();
	
	//SP init
	if(this->stack_address != -1){
		//hart_device->r[2] = this->stack_address;
		this->hart_state->set_register_by_name("sp", this->stack_address);
	}
}

/**
 * Performs a single step of the hart device.
 * 
 * First validates and makes sure it's good to execute. Then attempts to handle traps (exceptions and
 * interrupts). If that fails, return Instruction Fault (TODO: add a new exception type). Then calls
 * the underlying rv_step function on the hart. If it returned that an interrupt or exception occurred,
 * store it so it can be processed on the next step.
 */
RVExceptions RVHart::step_hart(){
	if(!this->hart_state_valid() || this->hart_state->get_exited()) return RVExceptions::ETRAP_NONE;

	rv* hart_device = this->get_hart_device();
	if(hart_device == nullptr) return RVExceptions::EIFAULT;

	if(!this->handle_traps()){
		UtilityFunctions::print("Handle traps failed!");
		return RVExceptions::EIFAULT;
	}

	rv_res hart_result = rv_step(hart_device);
	RVExceptions casted_result = RVExceptions::ETRAP_NONE;
	if(hart_result >= 0x80000000 && hart_result != RV_TRAP_NONE && hart_result != RV_TRAP_WFI){
		this->hart_state->set_pending_interrupt(true);
	} else{
		casted_result = RVExceptions(hart_result);
		this->hart_state->set_pending_interrupt(false);
	}
	this->hart_state->set_pending_exception(casted_result);

    return casted_result;
}

Ref<RVHartState> RVHart::get_hart_state() const{
	return this->hart_state;
}

void RVHart::set_hart_state(const Ref<RVHartState>& new_state){
	this->hart_state = new_state;
}

/**
 * Performs clears if asked, sets up mepc, then jumps to the address that
 * holds the mret instruction.
 */
void RVHart::mret(bool clear_vals, bool increment_mepc){
	if(clear_vals){
		this->hart_state->set_mcause(0);
		this->hart_state->set_mtval(0);
	}
	if(increment_mepc) this->hart_state->set_mepc(this->hart_state->get_mepc() + 4);
	this->hart_state->set_pc(this->mret_address);
}

void RVHart::trigger_software_interrupt(){
	this->_trigger_interrupt(RV_CSI);
}

void RVHart::clear_software_interrupt(){
	this->_clear_interrupt(RV_CSI);
}

void RVHart::trigger_timer_interrupt(){
	this->_trigger_interrupt(RV_CTI);
}

void RVHart::clear_timer_interrupt(){
	this->_clear_interrupt(RV_CTI);
}

void RVHart::trigger_external_interrupt(){
	this->_trigger_interrupt(RV_CEI);
}

void RVHart::clear_external_interrupt(){
	this->_clear_interrupt(RV_CEI);
}

/**
 * Encodes parameters into a char** array, adds it as a ROM to the bus, and returns the
 * address to it. This will also set up the stack correctly. It REQUIRES a stack right now,
 * but will probably update it to be optional, it will just require manual setup by the
 * developer.
 * 
 * TODO: Should this return the char** pointer instead? We can use that to find the others,
 * since if ret_value = encode_parameters(...) and it returned the char**, then ret_value[0]
 * holds the start of the ROM because it's always encoded at offset 0 in the ROM.
 */
int64_t RVHart::encode_parameters(Variant data){
	// If we don't have a stack, we can't operate.
	if(this->stack_address == -1) return -1;

	Array to_iterate;

	// Instead of checking for every possible item, check if it has them function "slice" as it
	// is an array type only function.
	if(!data.has_method("slice")){
		to_iterate.push_back(data);
	} else{
		to_iterate = data;
	}

	// The addresses will be encoded as relative until an empty area is found
	// but we need to know how big an area we need first.
	// Convert each item passed in into a string.
	PackedInt32Array addresses;
	PackedByteArray end_data;
	for(int64_t i = 0; i < to_iterate.size(); i++){
		String to_encode = UtilityFunctions::str(to_iterate[i]);
		addresses.append(end_data.size());
		end_data.append_array(to_encode.to_utf8_buffer());
		end_data.append(0);
	}

	// The actual size needed will be the size of the data plus the addresses.
	// The addresses will be encoded after the data, because we can add a null address
	// for safety.
	addresses.append(0);
	int64_t addresses_full_size = addresses.size() * 4;
	int64_t end_data_size = end_data.size();
	int64_t to_encode_size = end_data_size + addresses_full_size;

	int64_t to_start_addr = this->find_empty_area(0xE0000000, to_encode_size);
	if(to_start_addr == -1) return -1;

	// Add the actual offset to each address.
	// paranoia makes grabbing from the value then adding make more sense cause overflow.
	for(int64_t i = 0; i < addresses.size(); i++){
		int64_t val = addresses[i];
		val += to_start_addr;
		addresses[i] = val;
	}

	// Append the addresses to the data, then encode it.
	// It would only fail if the address was bad, but we confirmed above it can't be,
	// cause find_empty_area would return -1 if it would fail during add_bus_device.
	end_data.append_array(addresses.to_byte_array());
	Ref<RVMemoryDevice> parameter_memory = RVMemoryDevice::create_from_buffer(end_data);
	this->add_bus_device(parameter_memory, to_start_addr);

	// Finally, add the argc, argv, and environ (NULL for now) items.
	PackedInt32Array stack_data;
	stack_data.append(to_iterate.size()); // argc
	stack_data.append(to_start_addr+end_data_size); // the char** argv pointer should point to the end of the data, start of addresses
	stack_data.append(0); // environ
	PackedByteArray stack_data_byte = stack_data.to_byte_array();
	this->stack_address -= stack_data_byte.size();
	this->bus_write(this->stack_address, stack_data_byte);

	// Set stack pointer.
	this->hart_state->set_register_by_name("sp", this->stack_address);

	// Return the start of the parameter area of mem, which will be the first param value.
	return to_start_addr;
}

/**
 * Bridge function between the rv code and GDExtension code. The user data stores the pointer
 * to the hart address, so we can use it to perform the true bus read, copy the data to the
 * memory address rv needs, and return the result.
 */
rv_res rv_bus_access(void *user, rv_u32 addr, rv_u8 *data, rv_u32 is_store, rv_u32 width)
{
    RVHart* hart = Object::cast_to<RVHart>((Object*)user);
	if(hart == nullptr) return RV_BAD;

	RVMemResult bus_ret = RVMemResult::MEM_OK;
	if(is_store){
		PackedByteArray byte_data_arr;
		byte_data_arr.resize(width);
		for(uint64_t i = 0; i < width && i < byte_data_arr.size(); i++){
			byte_data_arr[i] = data[i];
		}
		bus_ret = hart->bus_write(addr, byte_data_arr);
	} else{
		Variant bus_read_ret = hart->bus_read(addr, width);
		if(bus_read_ret.get_type() == Variant::PACKED_BYTE_ARRAY){
			PackedByteArray arr_data = bus_read_ret;
			for(uint64_t i = 0; i < width && i < arr_data.size(); i++){
				data[i] = arr_data[i];
			}
		} else if(bus_read_ret.get_type() == Variant::INT){
			//Right now they don't translate
			bus_ret = RVMemResult((int64_t)bus_read_ret);
		} else{
			bus_ret = RVMemResult::BAD;
		}
	}

	// Cast the result? Isn't really needed cause of C shenanigans with enums, but paranoia.
	return rv_res(bus_ret);
}
