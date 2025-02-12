#ifndef _GDE_V_HART_H_
#define _GDE_V_HART_H_

#include "gde-v.h"

namespace godot{

class RVHart : public Node {
	GDCLASS(RVHart, Node)

private:
	double time_passed;

protected:
	static void _bind_methods();

	Ref<RVHartState> hart_state;
	rv* get_hart_device();

	bool hart_state_valid();

	Dictionary bus_devices;
	int64_t get_closest_device_offset(int64_t address);
	int64_t find_empty_area(int64_t hint, int64_t size);
	int64_t create_stack_heap_device(int64_t hint, int64_t size, bool create_heap);

	int64_t mret_address;
	int64_t stack_address;
	int64_t heap_address;

	GDVIRTUAL1R(bool, _handle_exception, RVExceptions);
	virtual bool _handle_exception(RVExceptions exception);
	bool handle_traps();

	void _trigger_interrupt(rv_cause type);
	void _clear_interrupt(rv_cause type);

	bool _set_special_address(int64_t* ptr, int64_t new_address);
public:
	RVHart();
	~RVHart();

	void _process(double delta) override;

	Error add_bus_device(Ref<RVBusDevice> new_device, int64_t offset);
	Error remove_bus_device_by_offset(int64_t offset);
	Error remove_bus_device(Ref<RVBusDevice> to_remove);
	Ref<RVBusDevice> get_bus_device(int64_t offset);

	int64_t create_stack_device(int64_t size, int64_t start_hint = -1);
	void set_stack_address(int64_t new_address);
	int64_t get_stack_address();

	int64_t create_heap_device(int64_t size, int64_t start_hint = 0xE0000000);
	void set_heap_address(int64_t new_address);
	int64_t get_heap_address();

	Variant bus_read(int64_t address, int64_t size);
	RVMemResult bus_write(int64_t address, PackedByteArray data);

	void reset_hart(uint64_t pc_start = 0x80000000);
	RVExceptions step_hart();

	Ref<RVHartState> get_hart_state() const;
	void set_hart_state(const Ref<RVHartState>& new_state);

	GET_SET(mret_address, int64_t)

	void mret(bool clear_vals, bool increment_mepc);

	void trigger_software_interrupt();
	void clear_software_interrupt();
	void trigger_timer_interrupt();
	void clear_timer_interrupt();
	void trigger_external_interrupt();
	void clear_external_interrupt();

	int64_t encode_parameters(Variant data);
};

}

#endif