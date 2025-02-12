#ifndef _GDE_V_HART_STATE_H_
#define _GDE_V_HART_STATE_H_

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

// rv is a C library, and breaks under C++ compilation
extern "C"{
#include <rv.h>
}

#include <gde-v/bus_device.h>
#include <gde-v/utils.h>

// I don't like this, but can't do static godot properties or runtime DLL failures
#include <map>

// Forward declaration of the bus access function used under the hood
rv_res rv_bus_access(void *user, rv_u32 addr, rv_u8 *data, rv_u32 is_store, rv_u32 width);

// Wrapper around the RV numbers
// Worked best as a global, cause forward declaring enums is a hassle in classes
enum RVExceptions{
	RV_ENUM_WRAP(EIALIGN),
	RV_ENUM_WRAP(EIFAULT),
	RV_ENUM_WRAP(EILL),
	RV_ENUM_WRAP(EBP),
	RV_ENUM_WRAP(ELALIGN),
	RV_ENUM_WRAP(ELFAULT),
	RV_ENUM_WRAP(ESALIGN),
	RV_ENUM_WRAP(ESFAULT),
	RV_ENUM_WRAP(EUECALL),
	RV_ENUM_WRAP(ESECALL),
	RV_ENUM_WRAP(EMECALL),
	RV_ENUM_WRAP(EIPAGE),
	RV_ENUM_WRAP(ELPAGE),
	RV_ENUM_WRAP(ESPAGE),
	ETRAP_NONE = RV_TRAP_NONE,
	ETRAP_WFI = RV_TRAP_WFI
};

VARIANT_ENUM_CAST(RVExceptions)

namespace godot {

/**
 * Base class for a HART state. Inherits from Resource, but may change to
 * RefCounted instead since. Primary reason for doing Resource is to be
 * able to save and load from disk.
 */
class RVHartState : public Resource {
	GDCLASS(RVHartState, Resource)

private:
	// The underlying hart device. Guard it jealously. Also it's small enough
	// to not need to be allocated on heap, since this class will always be
	// created on the heap.
	rv hart_device;

protected:
	// Standard Godot binding method to let it know about our class.
	static void _bind_methods();

	// Whether or not execution is done. May end through exceptions.
	bool exited;
	// The last exception that occurred. May be ETRAP_NONE, meaning nothing bad.
	RVExceptions pending_exception;
	// Set if there is a pending interrupt. Easier than trying to manipulate the underlying
	// register, but it should be rewritten to use that. Or moved to a CLINT implementation.
	bool pending_interrupt;

	// Constant of how to map the API namings to the numbers.
	const std::map<String, int64_t> register_mappings = {{"zero", 0}, {"ra", 1}, {"sp", 2}, {"gp", 3}, {"tp", 4},
{"t0", 5}, {"t1", 6}, {"t2", 7}, {"s0", 8}, {"s1", 9}, {"a0", 10}, {"a1", 11},
{"a2", 12}, {"a3", 13}, {"a4", 14}, {"a5", 15}, {"a6", 16}, {"a7", 17},
{"s2", 18}, {"s3", 19}, {"s4", 20}, {"s5", 21}, {"s6", 22}, {"s7", 23},
{"s8", 24}, {"s9", 25}, {"s10", 26}, {"s11", 27},
{"t3", 28}, {"t4", 29}, {"t5", 30}, {"t6", 31}};
public:
	RVHartState();
	~RVHartState();

	// Use a wrapper to define the get/set functions, since they are all the same.
	GET_SET(exited, bool)
	GET_SET(pending_exception, RVExceptions)
	// Clears the pending exception flag. We don't want anyone but the rv engine to
	// tell us of an exception though.
	void clear_pending_exception();
	GET_SET(pending_interrupt, bool)

	// Wrapper get/set, but using a sub structure/class. Assumes it's not a pointer
	// and exists in this class.
	GET_SET_STRUCT(pc, int64_t, hart_device)
	GET_SET_STRUCT(priv, int64_t, hart_device)

	// Defined differently so the passed in array is hopefully passed in by reference
	// instead of by value.
	PackedInt32Array get_registers();
	void set_registers(const PackedInt32Array& values);

	// Helper functions to set an individual register. by_name versions use the ABI
	// register mapping definitions.
	int64_t get_register_by_num(int64_t reg_number);
	int64_t get_register_by_name(String reg_name);
	void set_register_by_num(int64_t reg_num, int64_t value);
	void set_register_by_name(String reg_name, int64_t value);

	// CSR register mapping start
	GET_SET_STRUCT(sie, int64_t, hart_device.csr)
	GET_SET_STRUCT(stvec, int64_t, hart_device.csr)
	GET_SET_STRUCT(scounteren, int64_t, hart_device.csr)
	GET_SET_STRUCT(sscratch, int64_t, hart_device.csr)
	GET_SET_STRUCT(sepc, int64_t, hart_device.csr)
	GET_SET_STRUCT(scause, int64_t, hart_device.csr)
	GET_SET_STRUCT(stval, int64_t, hart_device.csr)
	GET_SET_STRUCT(sip, int64_t, hart_device.csr)
	GET_SET_STRUCT(satp, int64_t, hart_device.csr)

	GET_SET_STRUCT(mstatus, int64_t, hart_device.csr)
	GET_SET_STRUCT(misa, int64_t, hart_device.csr)
	GET_SET_STRUCT(medeleg, int64_t, hart_device.csr)
	GET_SET_STRUCT(mideleg, int64_t, hart_device.csr)
	GET_SET_STRUCT(mie, int64_t, hart_device.csr)
	GET_SET_STRUCT(mtvec, int64_t, hart_device.csr)
	GET_SET_STRUCT(mcounteren, int64_t, hart_device.csr)
	GET_SET_STRUCT(mstatush, int64_t, hart_device.csr)
	GET_SET_STRUCT(mscratch, int64_t, hart_device.csr)
	GET_SET_STRUCT(mepc, int64_t, hart_device.csr)
	GET_SET_STRUCT(mcause, int64_t, hart_device.csr)
	GET_SET_STRUCT(mtval, int64_t, hart_device.csr)
	GET_SET_STRUCT(mip, int64_t, hart_device.csr)
	GET_SET_STRUCT(mtime, int64_t, hart_device.csr)
	GET_SET_STRUCT(mtimeh, int64_t, hart_device.csr)
	GET_SET_STRUCT(mvendorid, int64_t, hart_device.csr)
	GET_SET_STRUCT(marchid, int64_t, hart_device.csr)
	GET_SET_STRUCT(mimpid, int64_t, hart_device.csr)
	GET_SET_STRUCT(mhartid, int64_t, hart_device.csr)
	// CSR register mapping end

	// Copy around the hart info. Used primarily to make a duplicate for forking or threading.
	void copy_from_info(rv* hart_info);
	void copy_into_info(rv* hart_info);
	// Gets a pointer to the underlying rv device. DO NOT FREE, IT IS NOT DYNAMICALLY ALLOCATED.
	rv* get_hart_info();

	//Check if there are any pending interrupts of the given type.
	bool is_software_interrupt();
	bool is_timer_interrupt();
	bool is_external_interrupt();
};

}

#endif