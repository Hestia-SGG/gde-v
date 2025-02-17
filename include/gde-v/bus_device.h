#ifndef _GDE_V_BUS_DEVICE_H_
#define _GDE_V_BUS_DEVICE_H_

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/core/type_info.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>

#include <rv.h>

#include "gde-v/utils.h"

// Wrapper around RV values.
// Defined at the global level so forward declaration isn't needed
enum RVMemResult{
    MEM_OK = RV_OK,
    RV_ENUM_WRAP(BAD),
    RV_ENUM_WRAP(BAD_ALIGN),
    RV_ENUM_WRAP(PAGEFAULT)
};

// Remember to cast it into the Godot engine
VARIANT_ENUM_CAST(RVMemResult)

namespace godot{

/**
 * Base class for a Bus Device. Anything that inherits from this can override the
 * functions _read, _write, _get_size, and _set_size to fully implement a new
 * device. NOTE: These do not need to be bound in child class _bind_method definitions.
 * They're defined below both using GDVIRTUAL and as default implementations. This allows
 * the wrapper functions to call overridden versions without being bound.
 */
class RVBusDevice : public Resource{
	GDCLASS(RVBusDevice, Resource)

protected:
    // Standard Godot binding method to let it know about our class.
	static void _bind_methods();

public:
    RVBusDevice(){}
    ~RVBusDevice(){}

    // GDVIRTUALx[RC] are macros that create "virtual" functions. These are defined in the engine
    // as virtual, but that really only applies to GDScript or calling through object.call[p/_deferred].
    GDVIRTUAL2R(Variant, _read, int64_t, int64_t)
    GDVIRTUAL2R(RVMemResult, _write, int64_t, PackedByteArray)
    GDVIRTUAL0R(int64_t, _get_size)
    GDVIRTUAL1R(RVMemResult, _set_size, int64_t)
    GDVIRTUAL2R(int64_t, _find_byte, int64_t, int64_t)

    // Adding the default definition here allows subclass overrides to be called without binding them
    // into the Godot engine through ClassDB::bind_method or bind_virtual_method.
    virtual Variant _read(int64_t offset, int64_t size){ return Error::ERR_INVALID_DATA; }
	virtual RVMemResult _write(int64_t offset, PackedByteArray new_data){ return RVMemResult::BAD; }
    virtual int64_t _get_size(){ return -1; }
	virtual RVMemResult _set_size(int64_t new_size){ return RVMemResult::BAD; }
    virtual int64_t _find_byte(int64_t to_find, int64_t offset_to_start){ return -1; }

    // Wrapper functions that call the underlying _ versions. NOTE: All offsets will be passed in relative
    // to the device's start, not global start.
    Variant read(int64_t offset, int64_t size);
    RVMemResult write(int64_t offset, PackedByteArray new_data);
    int64_t get_size();
    RVMemResult set_size(int64_t new_size);
    int64_t find_byte(int64_t to_find, int64_t offset_to_start);

    String get_string_from_memory(int64_t offset);
    RVMemResult set_string_to_memory(int64_t offset, String to_set);
};

/**
 * A bus device that is represented by a PackedByteArray. Can be instantiated from another PackedByteArray
 * or a file. Primary device used by the extension for memory management. Can be made read only using
 * the flag. See also RVHeapDevice for an extension that has two sizings.
 */
class RVMemoryDevice : public RVBusDevice{
    GDCLASS(RVMemoryDevice, RVBusDevice)

protected:
    // Standard Godot binding method to let it know about our class.
	static void _bind_methods();

    // Holds the data for this memory region. PackedByteArrays are supposed to be more memory efficient,
    // so using these to get the most.
    PackedByteArray data;
    // Whether or not the memory region is read_only. Need to see how to better map this into the rv object
    // to also set up protections on that level.
    bool read_only;
public:
    RVMemoryDevice();
    ~RVMemoryDevice();

    // These functions will return around the data object.
    virtual Variant _read(int64_t offset, int64_t size) override;
	virtual RVMemResult _write(int64_t offset, PackedByteArray new_data) override;
	virtual int64_t _get_size() override;
	virtual RVMemResult _set_size(int64_t new_size) override;
    virtual int64_t _find_byte(int64_t to_find, int64_t offset_to_start) override;

    // Getter/setter for read_only property.
    bool get_read_only();
    void set_read_only(bool new_mode);

    // Static function to load from the given file. Will return nullptr wrapped reference, which can be checked
    // with ret_value.is_null().
    static Ref<RVMemoryDevice> load_rom_file(String file_path);
    static Ref<RVMemoryDevice> create_from_buffer(PackedByteArray buffer, int64_t min_size = 0);
};

/**
 * Small wrapper around RVMemoryDevice. It rewires get_size and set_size a reserved_size property, which controls
 * how big this device appears to the system. It's actual size in memory is controlled by the "virtual"
 * underlying_mem_size property. These route to the data property defined in RVMemoryDevice.
 */
class RVHeapDevice : public RVMemoryDevice{
    GDCLASS(RVHeapDevice, RVMemoryDevice)

protected:
    // Standard Godot binding method to let it know about our class.
	static void _bind_methods();

    // How big the memory area seems to the system.
    int64_t reserved_size;
public:
    RVHeapDevice(){ reserved_size = -1; }
    ~RVHeapDevice(){}

    // Wrap the virtual sizing properties around reserved_size, so the system can allocate
    // address space without using host memory.
	virtual int64_t _get_size() override{
        return this->reserved_size;
    }
	virtual RVMemResult _set_size(int64_t new_size) override{
        if(this->reserved_size == -1){
            this->reserved_size = new_size;
            return RVMemResult::MEM_OK;
        } else{
            return RVMemResult::BAD;
        }
    }

    // Deal with the actual size of the data. Can never go larger than the device's allocated/reserved size.
    int64_t get_underlying_mem_size(){ return this->RVMemoryDevice::_get_size(); }
    void set_underlying_mem_size(int64_t new_size){
        if(new_size > reserved_size) return;
        this->RVMemoryDevice::_set_size(new_size);
    }
};

}

#endif
