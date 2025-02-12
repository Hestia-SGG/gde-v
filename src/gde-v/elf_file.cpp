#include "gde-v/elf.h"

using namespace godot;

void ElfFile::_bind_methods(){
    BIND_GET_NOSET_HINT(header, ElfFile, Variant::OBJECT, PROPERTY_HINT_TYPE_STRING, "ElfHeader")
    BIND_GET_NOSET_HINT(program_headers, ElfFile, Variant::ARRAY, PROPERTY_HINT_TYPE_STRING, "ElfProgramHeader")
    /*BIND_GET_NOSET_HINT(program_headers, ElfFile, Variant::ARRAY, PROPERTY_HINT_TYPE_STRING, 
        String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_RESOURCE_TYPE) + ":ElfProgramHeader")*/
    BIND_GET_NOSET(loaded, ElfFile, Variant::BOOL)

    ClassDB::bind_method(D_METHOD("load_data"), &ElfFile::load_data);
    ClassDB::bind_method(D_METHOD("generate_memory_areas"), &ElfFile::generate_memory_areas);

    ClassDB::bind_static_method("ElfFile", D_METHOD("load_elf_data", "data"), &ElfFile::load_elf_data);
}

Ref<ElfFile> ElfFile::load_elf_data(Variant data){
    Ref<ElfFile> ret;
    //We are double checking here and in the sub function, but better safe
    if(data.get_type() != Variant::PACKED_BYTE_ARRAY) return ret;

    ret.instantiate();
    ret->load_data(data);
    return ret;
}

ElfFile::ElfFile(){
    
}

ElfFile::~ElfFile(){

}

void ElfFile::load_data(Variant data){
    if(data.get_type() != Variant::PACKED_BYTE_ARRAY) return;

    this->raw_data = data.duplicate(true);
    this->header = ElfHeader::load_header(data);
    if(this->header.is_null()) return;

    this->program_headers = ElfProgramHeader::load_program_headers(this->header, data);

    this->loaded = true;
}

Dictionary ElfFile::generate_memory_areas(){
    Dictionary ret_dict;
    
    for(int64_t i = 0; i < this->program_headers.size(); i++){
        Ref<ElfProgramHeader> cur_ph = this->program_headers[i];
        if(!cur_ph.is_valid() || cur_ph.is_null() || !cur_ph->is_valid()) continue;
        if(cur_ph->get_size_in_memory() <= 0) continue;
        uint64_t start = cur_ph->get_file_offset();
        uint64_t end = start + cur_ph->get_size_in_file();
        PackedByteArray mem_copy = this->raw_data.slice(start, end);

        Ref<RVMemoryDevice> new_device = RVMemoryDevice::create_from_buffer(mem_copy, cur_ph->get_size_in_memory());

        bool read_only = ((cur_ph->get_flags() & ElfProgramHeader::Flags::WRITEABLE) == 0);
        new_device->set_read_only(read_only);
        ret_dict[cur_ph->get_virtual_address()] = new_device;
    }

    return ret_dict;
}
