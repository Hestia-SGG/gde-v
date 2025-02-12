#include "gde-v/bus_device.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/file_access.hpp>

using namespace godot;

//------------- Static ------------------------------------------------------------------------

void RVBusDevice::_bind_methods(){
    GDVIRTUAL_BIND(_read, "offset", "size");
    GDVIRTUAL_BIND(_write, "offset", "new_data");
    GDVIRTUAL_BIND(_get_size);
    GDVIRTUAL_BIND(_set_size, "new_size");
    ClassDB::bind_method(D_METHOD("read", "offset", "size"), &RVBusDevice::read);
    ClassDB::bind_method(D_METHOD("write", "offset", "new_data"), &RVBusDevice::write);
    ClassDB::bind_method(D_METHOD("get_size"), &RVBusDevice::get_size);
    ClassDB::bind_method(D_METHOD("set_size", "new_size"), &RVBusDevice::set_size);

    BIND_ENUM_CONSTANT(MEM_OK)
    BIND_ENUM_CONSTANT(BAD)
    BIND_ENUM_CONSTANT(BAD_ALIGN)
    BIND_ENUM_CONSTANT(PAGEFAULT)
}

void RVMemoryDevice::_bind_methods(){
    //ClassDB::bind_method(D_METHOD("_read", "offset", "size"), &RVMemoryDevice::_read);
    //ClassDB::bind_method(D_METHOD("_write", "offset", "data"), &RVMemoryDevice::_write);

    //ClassDB::bind_method(D_METHOD("_get_size"), &RVMemoryDevice::_get_size);
    //ClassDB::bind_method(D_METHOD("_set_size", "new_size"), &RVMemoryDevice::_set_size);

    ClassDB::bind_method(D_METHOD("get_read_only"), &RVMemoryDevice::get_read_only);
    ClassDB::bind_method(D_METHOD("set_read_only", "new_mode"), &RVMemoryDevice::set_read_only);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "read_only"), "set_read_only", "get_read_only");

    ClassDB::bind_static_method("RVMemoryDevice", D_METHOD("load_rom_file", "file_path"), &RVMemoryDevice::load_rom_file);
    ClassDB::bind_static_method("RVMemoryDevice", D_METHOD("create_from_buffer", "buffer", "min_size"), &RVMemoryDevice::create_from_buffer, DEFVAL(0));
}

void RVHeapDevice::_bind_methods(){
    ClassDB::bind_method(D_METHOD("get_underlying_mem_size"), &RVHeapDevice::get_underlying_mem_size);
    ClassDB::bind_method(D_METHOD("set_underlying_mem_size", "new_size"), &RVHeapDevice::set_underlying_mem_size);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "underlying_mem_size"), "set_underlying_mem_size", "get_underlying_mem_size");
}

Ref<RVMemoryDevice> RVMemoryDevice::load_rom_file(String file_path){
    if(!FileAccess::file_exists(file_path)) return nullptr;

    PackedByteArray data = FileAccess::get_file_as_bytes(file_path);
    Ref<RVMemoryDevice> ret;
    ret.instantiate();
    ret->data = data;
    ret->read_only = true;
    return ret;
}

Ref<RVMemoryDevice> RVMemoryDevice::create_from_buffer(PackedByteArray buffer, int64_t min_size){
    Ref<RVMemoryDevice> ret;
    ret.instantiate();
    ret->data = buffer;
    ret->read_only = true;
    if(min_size > 0 && min_size > ret->data.size()){
        ret->data.resize(min_size);
    }
    return ret;
}

//------------- Member ------------------------------------------------------------------------

Variant RVBusDevice::read(int64_t offset, int64_t size){
    Variant ret;
    if (GDVIRTUAL_CALL(_read, offset, size, ret)) {
		return ret;
	} else if(this->has_method("_read")){
        return this->call("_read", offset, size);
    } else{
        return this->_read(offset, size);
    }
    
    return RVMemResult::BAD;
}

RVMemResult RVBusDevice::write(int64_t offset, PackedByteArray new_data){
    RVMemResult ret;
    if (GDVIRTUAL_CALL(_write, offset, new_data, ret)) {
		return ret;
	} else if(this->has_method("_write")){
        return RVMemResult((int64_t)this->call("_write", offset, new_data));
    } else{
        return this->_write(offset, new_data);
    }

    return RVMemResult::BAD;
}

int64_t RVBusDevice::get_size(){
    int64_t ret;
    if (GDVIRTUAL_CALL(_get_size, ret)) {
		return ret;
	} else if(this->has_method("_get_size")){
        return this->call("_get_size");
    } else{
        return this->_get_size();
    }
    
    return -1;
}

RVMemResult RVBusDevice::set_size(int64_t new_size){
    RVMemResult ret;
    if (GDVIRTUAL_CALL(_set_size, new_size, ret)) {
		return ret;
	} else if(this->has_method("_set_size")){
        return RVMemResult((int64_t)this->call("_set_size", new_size));
    } else{
        return this->_set_size(new_size);
    }

    return RVMemResult::BAD;
}

RVMemoryDevice::RVMemoryDevice(){
    this->read_only = false;
}

RVMemoryDevice::~RVMemoryDevice(){
}

Variant RVMemoryDevice::_read(int64_t offset, int64_t size){
    if((offset+size) > this->data.size()) return Error::ERR_INVALID_DATA;
    return this->data.slice(offset, offset+size);
}

RVMemResult RVMemoryDevice::_write(int64_t offset, PackedByteArray new_data){
    if(this->read_only) return RVMemResult::BAD;
    if((offset+new_data.size()) > this->data.size()) return RVMemResult::BAD;
    
    for(int64_t i = 0; i < new_data.size(); i++){
        this->data[offset+i] = new_data[i];
    }

    return RVMemResult::MEM_OK;
}

int64_t RVMemoryDevice::_get_size(){
    return this->data.size();
}

RVMemResult RVMemoryDevice::_set_size(int64_t new_size){
    if(this->read_only) return RVMemResult::BAD;
    //TODO: Figure out what this really returns
    this->data.resize(new_size);
    return RVMemResult::MEM_OK;
}

bool RVMemoryDevice::get_read_only(){
    return this->read_only;
}

void RVMemoryDevice::set_read_only(bool new_mode){
    this->read_only = new_mode;
}
