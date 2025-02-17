#include "gde-v/bus_device.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/file_access.hpp>

using namespace godot;

//------------- Static ------------------------------------------------------------------------

/**
 * Binds this class into Godot.
 * 
 * GDVIRTUAL_BIND will allow GDScript to define and other classes bind methods.
 * NOTE: Unknown why as of 2024/02/12, but using this and bind_method or bind_virtual_method
 * in child classes in GDExtensions DOES NOT work. The child function's won't be called when
 * using GDVIRTUAL_CALL. This is why later all three methods of calling are tried.
 */
void RVBusDevice::_bind_methods(){
    GDVIRTUAL_BIND(_read, "offset", "size");
    GDVIRTUAL_BIND(_write, "offset", "new_data");
    GDVIRTUAL_BIND(_get_size);
    GDVIRTUAL_BIND(_set_size, "new_size");
    GDVIRTUAL_BIND(_find_byte, "to_find", "offset_to_start")
    ClassDB::bind_method(D_METHOD("read", "offset", "size"), &RVBusDevice::read);
    ClassDB::bind_method(D_METHOD("write", "offset", "new_data"), &RVBusDevice::write);
    ClassDB::bind_method(D_METHOD("get_size"), &RVBusDevice::get_size);
    ClassDB::bind_method(D_METHOD("set_size", "new_size"), &RVBusDevice::set_size);
    ClassDB::bind_method(D_METHOD("find_byte", "to_find", "offset_to_start"), &RVBusDevice::find_byte);

    // Bind enum values into Godot. They'll show up under the class instead of the global level
    // because it's easier than attempting to add something to global.
    BIND_ENUM_CONSTANT(MEM_OK)
    BIND_ENUM_CONSTANT(BAD)
    BIND_ENUM_CONSTANT(BAD_ALIGN)
    BIND_ENUM_CONSTANT(PAGEFAULT)

    ClassDB::bind_method(D_METHOD("get_string_from_memory", "offset"), &RVBusDevice::get_string_from_memory);
    ClassDB::bind_method(D_METHOD("set_string_to_memory", "offset", "to_set"), &RVBusDevice::set_string_to_memory);
}

/**
 * Binds this class into Godot. See RVBusDevice::_bind_methods for more info.
 */
void RVMemoryDevice::_bind_methods(){
    ClassDB::bind_method(D_METHOD("get_read_only"), &RVMemoryDevice::get_read_only);
    ClassDB::bind_method(D_METHOD("set_read_only", "new_mode"), &RVMemoryDevice::set_read_only);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "read_only"), "set_read_only", "get_read_only");

    ClassDB::bind_static_method("RVMemoryDevice", D_METHOD("load_rom_file", "file_path"), &RVMemoryDevice::load_rom_file);
    ClassDB::bind_static_method("RVMemoryDevice", D_METHOD("create_from_buffer", "buffer", "min_size"), &RVMemoryDevice::create_from_buffer, DEFVAL(0));
}

/**
 * Binds this class into Godot. See RVBusDevice::_bind_methods for more info.
 */
void RVHeapDevice::_bind_methods(){
    ClassDB::bind_method(D_METHOD("get_underlying_mem_size"), &RVHeapDevice::get_underlying_mem_size);
    ClassDB::bind_method(D_METHOD("set_underlying_mem_size", "new_size"), &RVHeapDevice::set_underlying_mem_size);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "underlying_mem_size"), "set_underlying_mem_size", "get_underlying_mem_size");
}

/**
 * Loads a ROM file 1:1 into memory. Set's as read only since Read Only Memory.
 */
Ref<RVMemoryDevice> RVMemoryDevice::load_rom_file(String file_path){
    if(!FileAccess::file_exists(file_path)) return nullptr;

    PackedByteArray data = FileAccess::get_file_as_bytes(file_path);
    Ref<RVMemoryDevice> ret;
    ret.instantiate();
    ret->data = data;
    ret->read_only = true;
    return ret;
}

/**
 * Create a memory device from a buffer. From what we can see of the PackedByteArray operator=(const PackedByteArray&)
 * definition, it will make a copy of the passed in buffer and not a reference.
 */
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

/**
 * Tries to call an overridden version of this function. Used so subclasses can do different
 * logic for their own needs. A register device would use the memory addresses to find which
 * register to return instead of a buffer.
 * 
 * Three methods are used to try calling the function:
 * -GDVIRTUAL_Call: This will call a GDSCript override of the virtual function given. For unknown
 * reason, this does not work from our testing when using ClassDB::bind_virtual_method outside
 * of GDScript.
 * -call: This uses the Godot method of calling a function, and works when either the subclass
 * is defined in Godot or through Class::bind_method.
 * -Directly: As a last resort, call the function directly. Only calls the compiled version,
 * won't call a function defined in GDScript.
 */
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

/**
 * Tries to call an overridden version of this function. See RVBusDevice::read.
 */
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

/**
 * Tries to call an overridden version of this function. See RVBusDevice::read.
 */
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

/**
 * Tries to call an overridden version of this function. See RVBusDevice::read.
 */
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

// Newer way to define the virtual callers
VIRT_CALL_WRAPPER2R(int64_t, RVBusDevice, find_byte, int64_t, to_find, int64_t, offset_to_start)

String RVBusDevice::get_string_from_memory(int64_t offset){
    int64_t end_offset = this->find_byte(0, offset);
    if(end_offset == -1) return "";

    PackedByteArray to_convert = this->read(offset, end_offset-offset);
    return to_convert.get_string_from_utf8();
}

RVMemResult RVBusDevice::set_string_to_memory(int64_t offset, String to_set){
    PackedByteArray set_data = to_set.to_utf8_buffer();
    // add null byte at the end
    set_data.append(0);
    return this->write(offset, set_data);
}

RVMemoryDevice::RVMemoryDevice(){
    this->read_only = false;
}

RVMemoryDevice::~RVMemoryDevice(){
}

/**
 * Checks sizing against the underlying data property, and if valid, returns
 * a slice of it. Otherwise return an error. TODO: Change this to our own error
 * code, so it's easier to wrap in a HART.
 */
Variant RVMemoryDevice::_read(int64_t offset, int64_t size){
    if((offset+size) > this->data.size()) return Error::ERR_INVALID_DATA;
    return this->data.slice(offset, offset+size);
}

/**
 * Writes to the underlying data property by doing direct writes instead of relying
 * on built-in functions. Don't remember why.
 */
RVMemResult RVMemoryDevice::_write(int64_t offset, PackedByteArray new_data){
    if(this->read_only) return RVMemResult::BAD;
    if((offset+new_data.size()) > this->data.size()) return RVMemResult::BAD;
    
    for(int64_t i = 0; i < new_data.size(); i++){
        this->data[offset+i] = new_data[i];
    }

    return RVMemResult::MEM_OK;
}

/**
 * Get the size of the data property.
 */
int64_t RVMemoryDevice::_get_size(){
    return this->data.size();
}

/**
 * Try to resize this device. If it's read-only, fail.
 */
RVMemResult RVMemoryDevice::_set_size(int64_t new_size){
    if(this->read_only) return RVMemResult::BAD;
    //TODO: Figure out what this really returns
    this->data.resize(new_size);
    return RVMemResult::MEM_OK;
}

int64_t RVMemoryDevice::_find_byte(int64_t to_find, int64_t offset_to_start){
    return this->data.find(to_find, offset_to_start);
}

/**
 * Get if this device is read-only.
 */
bool RVMemoryDevice::get_read_only(){
    return this->read_only;
}

/**
 * Set if this device is read-only.
 */
void RVMemoryDevice::set_read_only(bool new_mode){
    this->read_only = new_mode;
}
