#ifndef _GDE_V_BUS_DEVICE_H_
#define _GDE_V_BUS_DEVICE_H_

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/core/type_info.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>

#include <rv.h>

#include "gde-v/utils.h"

enum RVMemResult{
    MEM_OK = RV_OK,
    RV_ENUM_WRAP(BAD),
    RV_ENUM_WRAP(BAD_ALIGN),
    RV_ENUM_WRAP(PAGEFAULT)
};

VARIANT_ENUM_CAST(RVMemResult)

namespace godot{

class RVBusDevice : public Resource{
	GDCLASS(RVBusDevice, Resource)

private:

protected:
	static void _bind_methods();

public:
    RVBusDevice(){}
    ~RVBusDevice(){}


    GDVIRTUAL2R(Variant, _read, int64_t, int64_t);
    GDVIRTUAL2R(RVMemResult, _write, int64_t, PackedByteArray);
    virtual Variant _read(int64_t offset, int64_t size){ return Error::ERR_INVALID_DATA; }
	virtual RVMemResult _write(int64_t offset, PackedByteArray new_data){ return RVMemResult::BAD; }
    Variant read(int64_t offset, int64_t size);
    RVMemResult write(int64_t offset, PackedByteArray new_data);

    GDVIRTUAL0R(int64_t, _get_size);
    GDVIRTUAL1R(RVMemResult, _set_size, int64_t);
    virtual int64_t _get_size(){ return -1; }
	virtual RVMemResult _set_size(int64_t new_size){ return RVMemResult::BAD; }
    int64_t get_size();
    RVMemResult set_size(int64_t new_size);
};

class RVMemoryDevice : public RVBusDevice{
    GDCLASS(RVMemoryDevice, RVBusDevice)

private:

protected:
	static void _bind_methods();

    PackedByteArray data;
    bool read_only;
public:
    RVMemoryDevice();
    ~RVMemoryDevice();


    virtual Variant _read(int64_t offset, int64_t size) override;
	virtual RVMemResult _write(int64_t offset, PackedByteArray new_data) override;

	virtual int64_t _get_size() override;
	virtual RVMemResult _set_size(int64_t new_size) override;

    bool get_read_only();
    void set_read_only(bool new_mode);

    static Ref<RVMemoryDevice> load_rom_file(String file_path);
    static Ref<RVMemoryDevice> create_from_buffer(PackedByteArray buffer, int64_t min_size = 0);
};

class RVHeapDevice : public RVMemoryDevice{
    GDCLASS(RVHeapDevice, RVMemoryDevice)

private:

protected:
	static void _bind_methods();

    int64_t reserved_size;
public:
    RVHeapDevice(){ reserved_size = -1; }
    ~RVHeapDevice(){}

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

    int64_t get_underlying_mem_size(){ return this->RVMemoryDevice::_get_size(); }
    void set_underlying_mem_size(int64_t new_size){
        if(new_size > reserved_size) return;
        this->RVMemoryDevice::_set_size(new_size);
    }
};

}

#endif
