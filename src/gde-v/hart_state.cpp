#include "gde-v/hart_state.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// Macro redefined here from rv.c since it isn't globally available.
#define rv_ext(c) (1 << (rv_u8)((c) - 'A')) /* isa extension bit in misa */

/**
 * Does initial setup of a HART device, trying to set it to a known good state.
 * Starts in Machine mode, but setting priv to another value will move it.
 */
void blank_rv(rv* hart_device){
	hart_device->pc = 0x80000000;

	hart_device->csr.sie        = 0;
	hart_device->csr.stvec      = 0;
	hart_device->csr.scounteren = 0;
	hart_device->csr.sscratch   = 0;
	hart_device->csr.sepc       = 0;
	hart_device->csr.scause     = 0;
	hart_device->csr.stval      = 0;
	hart_device->csr.sip        = 0;
	hart_device->csr.satp       = 0;
	hart_device->csr.mstatus    = 0;
	hart_device->csr.medeleg    = 0;
	hart_device->csr.mideleg    = 0;
	hart_device->csr.mie        = 0;
	hart_device->csr.mcounteren = 0;
	hart_device->csr.mstatush   = 0;
	hart_device->csr.mscratch   = 0;
	hart_device->csr.mepc       = 0;
	hart_device->csr.mcause     = 0;
	hart_device->csr.mtval      = 0;
	hart_device->csr.mip        = 0;
	hart_device->csr.mtime      = 0;
	hart_device->csr.mtimeh     = 0;
	hart_device->csr.mvendorid  = 0;
	hart_device->csr.marchid    = 0;
	hart_device->csr.mimpid     = 0;
	hart_device->csr.mhartid    = 0;

	hart_device->csr.misa = (1 << 30)     /* MXL = 1 [XLEN=32] */
					| rv_ext('M') /* Multiplication and Division */
					| rv_ext('C') /* Compressed Instructions */
					| rv_ext('A') /* Atomics */
					| rv_ext('S') /* Supervisor Mode */
					| rv_ext('U') /* User Mode */;
	hart_device->priv = RV_PMACH;
	for(int64_t i = 0; i < 32; i++){
		hart_device->r[i] = 0;
	}
}

//------------- Static ------------------------------------------------------------------------

/**
 * Binds this class into Godot. See RVBusDevice::_bind_methods for more info.
 */
void RVHartState::_bind_methods(){
	BIND_GET_SET(exited, RVHartState, BOOL, "new_val")
	BIND_GET_SET(pending_exception, RVHartState, INT, "new_val")
	ClassDB::bind_method(D_METHOD("clear_pending_exception"), &RVHartState::clear_pending_exception);
	BIND_GET_SET(pending_interrupt, RVHartState, BOOL, "new_val")

	BIND_GET_SET(pc, RVHartState, INT, "new_val")
	BIND_GET_SET(priv, RVHartState, INT, "new_val")
	BIND_GET_SET(registers, RVHartState, PACKED_INT32_ARRAY, "values")

	BIND_GET_SET(sie, RVHartState, INT, "new_val")
	BIND_GET_SET(stvec, RVHartState, INT, "new_val")
	BIND_GET_SET(scounteren, RVHartState, INT, "new_val")
	BIND_GET_SET(sscratch, RVHartState, INT, "new_val")
	BIND_GET_SET(sepc, RVHartState, INT, "new_val")
	BIND_GET_SET(scause, RVHartState, INT, "new_val")
	BIND_GET_SET(stval, RVHartState, INT, "new_val")
	BIND_GET_SET(sip, RVHartState, INT, "new_val")
	BIND_GET_SET(satp, RVHartState, INT, "new_val")
	BIND_GET_SET(mstatus, RVHartState, INT, "new_val")
	BIND_GET_SET(misa, RVHartState, INT, "new_val")
	BIND_GET_SET(medeleg, RVHartState, INT, "new_val")
	BIND_GET_SET(mideleg, RVHartState, INT, "new_val")
	BIND_GET_SET(mie, RVHartState, INT, "new_val")
	BIND_GET_SET(mtvec, RVHartState, INT, "new_val")
	BIND_GET_SET(mcounteren, RVHartState, INT, "new_val")
	BIND_GET_SET(mstatush, RVHartState, INT, "new_val")
	BIND_GET_SET(mscratch, RVHartState, INT, "new_val")
	BIND_GET_SET(mepc, RVHartState, INT, "new_val")
	BIND_GET_SET(mcause, RVHartState, INT, "new_val")
	BIND_GET_SET(mtval, RVHartState, INT, "new_val")
	BIND_GET_SET(mip, RVHartState, INT, "new_val")
	BIND_GET_SET(mtime, RVHartState, INT, "new_val")
	BIND_GET_SET(mtimeh, RVHartState, INT, "new_val")
	BIND_GET_SET(mvendorid, RVHartState, INT, "new_val")
	BIND_GET_SET(marchid, RVHartState, INT, "new_val")
	BIND_GET_SET(mimpid, RVHartState, INT, "new_val")
	BIND_GET_SET(mhartid, RVHartState, INT, "new_val")

	ClassDB::bind_method(D_METHOD("is_software_interrupt"), &RVHartState::is_software_interrupt);
	ClassDB::bind_method(D_METHOD("is_timer_interrupt"), &RVHartState::is_timer_interrupt);
	ClassDB::bind_method(D_METHOD("is_external_interrupt"), &RVHartState::is_external_interrupt);

	ClassDB::bind_method(D_METHOD("get_register_by_num", "reg_number"), &RVHartState::get_register_by_num);
	ClassDB::bind_method(D_METHOD("get_register_by_name", "reg_name"), &RVHartState::get_register_by_name);
	ClassDB::bind_method(D_METHOD("set_register_by_num", "reg_number", "value"), &RVHartState::set_register_by_num);
	ClassDB::bind_method(D_METHOD("set_register_by_name", "reg_name", "value"), &RVHartState::set_register_by_name);
}

//------------- Member ------------------------------------------------------------------------

void RVHartState::clear_pending_exception(){
	this->pending_exception = RVExceptions::ETRAP_NONE;
}

/**
 * Default constructor. Resets the HART to a hopefully known good state, and set the defaults
 * for pending and exited.
 */
RVHartState::RVHartState(){
	blank_rv(&(this->hart_device));
	this->pending_exception = RVExceptions::ETRAP_NONE;
	this->pending_interrupt = false;
	this->exited = false;
}

RVHartState::~RVHartState(){

}

/**
 * Returns the register set, but a duplicate, not the original. This is
 * so that a caller can't manipulate them without calling set_registers.
 * Does cause some wonkiness in Godot with the editor, but doesn't seem
 * to affect in-code or in-game.
 */
PackedInt32Array RVHartState::get_registers(){
	PackedInt32Array ret;
	ret.resize(32);
	for(int64_t i = 0; i < 32; i++){
		ret[i] = this->hart_device.r[i];
	}
	return ret;
}

/**
 * Copy the values over instead of copying the input or setting directly
 * equal to it. Safety measure to make sure that it can't be manipulated
 * outside of this function.
 */
void RVHartState::set_registers(const PackedInt32Array& values){
	for(int64_t i = 0; i < 32 && i < values.size(); i++){
		this->hart_device.r[i] = values[i];
	}
}

/**
 * Gets a register by number. Hard coded to RV32 though.
 */
int64_t RVHartState::get_register_by_num(int64_t reg_number){
	if(reg_number < 0 || reg_number > 32) return 0;
	else return this->hart_device.r[reg_number];
}

/**
 * Gets a register by name using the ABI. Hard coded to RV32 though.
 */
int64_t RVHartState::get_register_by_name(String reg_name){
	std::map<String, int64_t>::const_iterator iter = this->register_mappings.find(reg_name);
	if(iter == this->register_mappings.end()) return -1;

	return this->get_register_by_num(iter->second);
}

/**
 * Sets a register by number. Hard coded to RV32 though.
 */
void RVHartState::set_register_by_num(int64_t reg_num, int64_t value){
	if(reg_num >= 0 && reg_num < 32) this->hart_device.r[reg_num] = value;
}

/**
 * Sets a register by name using the ABI. Hard coded to RV32 though.
 */
void RVHartState::set_register_by_name(String reg_name, int64_t value){
	std::map<String, int64_t>::const_iterator iter = this->register_mappings.find(reg_name);
	if(iter == this->register_mappings.end()) return;

	this->set_register_by_num(iter->second, value);
}

void RVHartState::copy_from_info(rv* hart_info){
	if(hart_info == nullptr) return;
	/*this->hart_device.pc = hart_info->pc;
	this->hart_device.priv = hart_info->priv;
	for(int64_t i = 0; i < 32; i++){
		this->hart_device.r[i] = hart_info->r[i];
	}*/

	// EXPERIMENTAL: use memcpy on the devices
	memcpy(&(this->hart_device), hart_info, sizeof(rv));
}

void RVHartState::copy_into_info(rv* hart_info){
	if(hart_info == nullptr) return;
	/*hart_info->pc = this->hart_device.pc;
	hart_info->priv = this->hart_device.priv;
	for(int64_t i = 0; i < 32; i++){
		hart_info->r[i] = this->hart_device.r[i];
	}*/

	// EXPERIMENTAL: use memcpy on the devices
	memcpy(hart_info, &(this->hart_device), sizeof(rv));
}

rv* RVHartState::get_hart_info(){
	return &(this->hart_device);
}

/**
 * Check if the given interrupt bit is set for Machine mode.
 */
bool RVHartState::is_software_interrupt(){
	return (this->hart_device.csr.mip & RV_CSI) != 0;
}

/**
 * Check if the given interrupt bit is set for Machine mode.
 */
bool RVHartState::is_timer_interrupt(){
	return (this->hart_device.csr.mip & RV_CTI) != 0;
}

/**
 * Check if the given interrupt bit is set for Machine mode.
 * NOTE: RV_CEI in the official repository is set to 512, which is
 * the bitmask for Supervisor mode External Interrupt, not Machine mode.
 */
bool RVHartState::is_external_interrupt(){
	return (this->hart_device.csr.mip & RV_CEI) != 0;
}
