#ifndef _GDE_V_H_
#define _GDE_V_H_

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

//rv is a C library, and breaks under C++ compilation
extern "C"{
#include <rv.h>
}

#include <gde-v/bus_device.h>

#include <gde-v/utils.h>

//I don't like this, but can't do static godot properties
#include <map>

//Forward declaration of the bus access function used under the hood
rv_res rv_bus_access(void *user, rv_u32 addr, rv_u8 *data, rv_u32 is_store, rv_u32 width);

//Wrapper around the RV numbers
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

class RVHartState : public Resource {
	GDCLASS(RVHartState, Resource)

private:
	rv hart_device;

protected:
	static void _bind_methods();

	bool exited;
	RVExceptions pending_exception;
	bool pending_interrupt;

	const std::map<String, int64_t> register_mappings = {{"zero", 0}, {"ra", 1}, {"sp", 2}, {"gp", 3}, {"tp", 4},
{"t0", 5}, {"t1", 6}, {"t2", 7}, {"s0", 8}, {"s1", 9}, {"a0", 10}, {"a1", 11},
{"a2", 12}, {"a3", 13}, {"a4", 14}, {"a5", 15}, {"a6", 16}, {"a7", 17},
{"s2", 18}, {"s3", 19}, {"s4", 20}, {"s5", 21}, {"s6", 22}, {"s7", 23},
{"s8", 24}, {"s9", 25}, {"s10", 26}, {"s11", 27},
{"t3", 28}, {"t4", 29}, {"t5", 30}, {"t6", 31}};
public:
	RVHartState();
	~RVHartState();

	GET_SET(exited, bool)
	GET_SET(pending_exception, RVExceptions)
	void clear_pending_exception();
	GET_SET(pending_interrupt, bool)

	GET_SET_STRUCT(pc, int64_t, hart_device)
	GET_SET_STRUCT(priv, int64_t, hart_device)

	PackedInt32Array get_registers();
	void set_registers(const PackedInt32Array& values);

	int64_t get_register_by_num(int64_t reg_number);
	int64_t get_register_by_name(String reg_name);

	void set_register_by_num(int64_t reg_num, int64_t value);
	void set_register_by_name(String reg_name, int64_t value);

	//CSR start
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
	//CSR end

	void copy_from_info(rv* hart_info);
	void copy_into_info(rv* hart_info);
	rv* get_hart_info();

	bool is_software_interrupt();
	bool is_timer_interrupt();
	bool is_external_interrupt();
};

}

#endif