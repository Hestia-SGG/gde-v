#ifndef _GDE_V_HART_H_
#define _GDE_V_HART_H_

#include "gde-v/hart_state.h"

#include <godot_cpp/classes/node.hpp>

namespace godot{

/**
 * Baseclass for a HART definition. 
 * 
 * This inherits from Node because it should be run in a Node context to
 * gain access to the _process(delta) method. May move to Resource if
 * this moves to using a separate thread.
 */
class RVHart : public Node {
	GDCLASS(RVHart, Node)

private:
	// Left over from the example code. Would be useful to know how long
	// the current HART has been alive.
	double time_passed;

protected:
	// Standard Godot binding method to let it know about our class.
	static void _bind_methods();

	// The underlying HART. Should move the addresses into it, so the node
	// can be serialized easier.
	Ref<RVHartState> hart_state;
	rv* get_hart_device();

	// Returns if the hart_state property has been initialized.
	bool hart_state_valid();

	// List of all the bus devices that can be asked for data.
	Dictionary bus_devices;
	// Gets the offset for a device that overlaps with the given address. Useful
	// for finding a device that services an address.
	int64_t get_closest_device_offset(int64_t address);
	// Returns the closest memory area to hint of the given size, or -1 if unable to find.
	// DOES NOT WRAP!
	int64_t find_empty_area(int64_t hint, int64_t size);
	// Create either a stack or heap device given the hint and size. Returns -1 if unable
	// to do so.
	int64_t create_stack_heap_device(int64_t hint, int64_t size, bool create_heap);

	// Addresses to somewhere in memory the mret instruction is encoded.
	// This needs to be factored out, as it's required to make RVHart::mret work, but
	// it's 4 bytes of memory...
	int64_t mret_address;

	// Addresses of the stack and heap. Used to set the sp register an in subclass ABIHart
	// to implement the sbrk system call.
	int64_t stack_address;
	int64_t heap_address;

	// Register a virtual function to handle exceptions. This allows subclasses to define
	// handlers for just exceptions without having to override all traps.
	// TODO: add virtual function for handling interrupts.
	GDVIRTUAL1R(bool, _handle_exception, RVExceptions);
	virtual bool _handle_exception(RVExceptions exception);
	// Handles all traps that may occur. Returns true if all are cleared, false if either an
	// exception or interrupt is pending. Check hart_state->pending_exception and
	// hart_state->pending_interrupt to determine if there are any that need servicing.
	bool handle_traps();

	// Triggers or clears the given interrupt. See RISC-V Priviledged ISA, mcause.
	void _trigger_interrupt(rv_cause type);
	void _clear_interrupt(rv_cause type);

	// Sets the given special address like stack or heap to the given address.
	// TODO: Make inline?
	bool _set_special_address(int64_t* ptr, int64_t new_address);
public:
	RVHart();
	~RVHart();

	// Override the default process function. Currently it only updates the time passed
	// property, but may add running step_hart based on a property flag.
	void _process(double delta) override;

	Error add_bus_device(Ref<RVBusDevice> new_device, int64_t offset);
	Error remove_bus_device_by_offset(int64_t offset);
	Error remove_bus_device(Ref<RVBusDevice> to_remove);
	Ref<RVBusDevice> get_bus_device(int64_t offset);

	// Sets up a stack device. This doesnt' do much other than try to find an empty area
	// and set the pointer property, so can be done out of this too. Returns either the
	// address it was created at, or -1 on error.
	int64_t create_stack_device(int64_t size, int64_t start_hint = -1);
	void set_stack_address(int64_t new_address);
	int64_t get_stack_address();

	// Sets up a heap device. This doesnt' do much other than try to find an empty area
	// and set the pointer property, so can be done out of this too. Returns either the
	// address it was created at, or -1 on error.
	int64_t create_heap_device(int64_t size, int64_t start_hint = 0xE0000000);
	void set_heap_address(int64_t new_address);
	int64_t get_heap_address();

	// Read or write from the bus at the address. It will return Error on read, but should
	// be updated to return RVMemResult instead. 
	Variant bus_read(int64_t address, int64_t size);
	RVMemResult bus_write(int64_t address, PackedByteArray data);

	// Resets the hart to a default state. Optionally specify custom starting address.
	void reset_hart(uint64_t pc_start = 0x80000000);
	// Performs a single cycle for the hart.
	RVExceptions step_hart();

	// Get/set for hart state. Custom defined so that we can hopefully get passed by
	// reference.
	Ref<RVHartState> get_hart_state() const;
	void set_hart_state(const Ref<RVHartState>& new_state);

	GET_SET(mret_address, int64_t)

	// Will perform cleanup before calling mret to return from machine mode after
	// an ecall.
	void mret(bool clear_vals, bool increment_mepc);

	// Triggers or clears the given interrupt type.
	void trigger_software_interrupt();
	void clear_software_interrupt();
	void trigger_timer_interrupt();
	void clear_timer_interrupt();
	void trigger_external_interrupt();
	void clear_external_interrupt();

	// Tries to encode whatever is passed in as a char**. Will create a ROM of
	// the data on the bus, and return the address pointing to it. This will
	// also set up the stack, so it requires it being set up first.
	// TODO: Make it not require the stack, but still set up if available.
	int64_t encode_parameters(Variant data);
};

}

#endif