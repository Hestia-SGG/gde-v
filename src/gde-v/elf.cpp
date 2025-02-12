#include "gde-v/elf.h"

#include <godot_cpp/core/type_info.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>

using namespace godot;

uint64_t get_value_by_bitsize_unsafe(PackedByteArray data, int64_t offset, int64_t field_size){
    switch(field_size){
        case 1: return data.decode_u8(offset);
        case 2: return data.decode_u16(offset);
        case 4: return data.decode_u32(offset);
        case 8: return data.decode_u64(offset);
        default: return -1;
    }
}

void ElfHeader::_bind_methods(){
    BIND_GET_SET(magic, ElfHeader, PACKED_BYTE_ARRAY, "new_val")
    BIND_GET_SET(bit_size, ElfHeader, INT, "new_val")
    BIND_GET_SET(endianness, ElfHeader, INT, "new_val")
    BIND_GET_SET(version, ElfHeader, INT, "new_val")
    BIND_GET_SET(os_abi, ElfHeader, INT, "new_val")
    BIND_GET_SET(abi_version, ElfHeader, INT, "new_val")
    BIND_GET_SET(type, ElfHeader, INT, "new_val")
    BIND_GET_SET(machine, ElfHeader, INT, "new_val")
    BIND_GET_SET(elf_version, ElfHeader, INT, "new_val")
    BIND_GET_SET(entry_point, ElfHeader, INT, "new_val")
    BIND_GET_SET(program_header_offset, ElfHeader, INT, "new_val")
    BIND_GET_SET(section_header_offset, ElfHeader, INT, "new_val")
    BIND_GET_SET(flags, ElfHeader, INT, "new_val")
    BIND_GET_SET(header_size, ElfHeader, INT, "new_val")
    BIND_GET_SET(program_header_entry_size, ElfHeader, INT, "new_val")
    BIND_GET_SET(program_header_count, ElfHeader, INT, "new_val")
    BIND_GET_SET(section_header_entry_size, ElfHeader, INT, "new_val")
    BIND_GET_SET(section_header_count, ElfHeader, INT, "new_val")
    BIND_GET_SET(section_header_name_index, ElfHeader, INT, "new_val")

    ClassDB::bind_method(D_METHOD("get_pointer_size"), &ElfHeader::get_pointer_size);
    ClassDB::bind_method(D_METHOD("is_valid"), &ElfHeader::is_valid);

    BIND_ENUM_CONSTANT(MAGIC)
    BIND_ENUM_CONSTANT(BIT_SIZE)
    BIND_ENUM_CONSTANT(ENDIANNESS)
    BIND_ENUM_CONSTANT(VERSION)
    BIND_ENUM_CONSTANT(OS_ABI)
    BIND_ENUM_CONSTANT(ABI_VER)
    BIND_ENUM_CONSTANT(PAD_1_BEGIN)
    BIND_ENUM_CONSTANT(TYPE)
    BIND_ENUM_CONSTANT(MACHINE)
    BIND_ENUM_CONSTANT(ELF_VERSION)
    BIND_ENUM_CONSTANT(ENTRY_BASE)
    BIND_ENUM_CONSTANT(PH_OFF_BASE)
    BIND_ENUM_CONSTANT(SH_OFF_BASE)
    BIND_ENUM_CONSTANT(FLAGS_BASE)
    BIND_ENUM_CONSTANT(HEADER_SIZE_BASE)
    BIND_ENUM_CONSTANT(PH_ENT_SIZE_BASE)
    BIND_ENUM_CONSTANT(PH_NUM_BASE)
    BIND_ENUM_CONSTANT(SH_ENT_SIZE_BASE)
    BIND_ENUM_CONSTANT(SH_NUM_BASE)
    BIND_ENUM_CONSTANT(SECTION_HEADER_NAME_INDX_BASE)
    BIND_ENUM_CONSTANT(END_BASE)

    BIND_ENUM_CONSTANT(LITTLE)
    BIND_ENUM_CONSTANT(BIG)
    BIND_ENUM_CONSTANT(INVALID_ENDIANNESS)

    BIND_ENUM_CONSTANT(INVALID_BITSIZE)
    BIND_ENUM_CONSTANT(SIZE_32)
    BIND_ENUM_CONSTANT(SIZE_64)

    BIND_ENUM_CONSTANT(INVALID_OBJTYPE)
    BIND_ENUM_CONSTANT(NONE)
    BIND_ENUM_CONSTANT(RELOC)
    BIND_ENUM_CONSTANT(EXEC)
    BIND_ENUM_CONSTANT(SHARED)
    BIND_ENUM_CONSTANT(DYNAMIC)
    BIND_ENUM_CONSTANT(CORE)
    BIND_ENUM_CONSTANT(LOW_OS)
    BIND_ENUM_CONSTANT(OS_RANGE)
    BIND_ENUM_CONSTANT(LOW_PROC)
    BIND_ENUM_CONSTANT(PROC_RANGE)
    BIND_ENUM_CONSTANT(HIGH_PROC)

    ClassDB::bind_static_method("ElfHeader", D_METHOD("load_header", "data"), &ElfHeader::load_header);
}

void ElfProgramHeader::_bind_methods(){
    BIND_GET_SET(type, ElfProgramHeader, INT, "new_val")
    BIND_GET_SET(flags, ElfProgramHeader, INT, "new_val")
    BIND_GET_SET(file_offset, ElfProgramHeader, INT, "new_val")
    BIND_GET_SET(virtual_address, ElfProgramHeader, INT, "new_val")
    BIND_GET_SET(physical_address, ElfProgramHeader, INT, "new_val")
    BIND_GET_SET(size_in_file, ElfProgramHeader, INT, "new_val")
    BIND_GET_SET(size_in_memory, ElfProgramHeader, INT, "new_val")
    BIND_GET_SET(align, ElfProgramHeader, INT, "new_val")

    ClassDB::bind_method(D_METHOD("is_valid"), &ElfProgramHeader::is_valid);
    
    BIND_ENUM_CONSTANT(TYPE_32)
    BIND_ENUM_CONSTANT(FLAGS_32)
    BIND_ENUM_CONSTANT(OFFSET_32)
    BIND_ENUM_CONSTANT(VADDR_32)
    BIND_ENUM_CONSTANT(PADDR_32)
    BIND_ENUM_CONSTANT(FILESZ_32)
    BIND_ENUM_CONSTANT(MEMSZ_32)
    BIND_ENUM_CONSTANT(ALIGN_32)
    BIND_ENUM_CONSTANT(TYPE_64)
    BIND_ENUM_CONSTANT(FLAGS_64)
    BIND_ENUM_CONSTANT(OFFSET_64)
    BIND_ENUM_CONSTANT(VADDR_64)
    BIND_ENUM_CONSTANT(PADDR_64)
    BIND_ENUM_CONSTANT(FILESZ_64)
    BIND_ENUM_CONSTANT(MEMSZ_64)
    BIND_ENUM_CONSTANT(ALIGN_64)

    BIND_ENUM_CONSTANT(PH_NULL)
    BIND_ENUM_CONSTANT(LOAD)
    BIND_ENUM_CONSTANT(DYNAMIC)
    BIND_ENUM_CONSTANT(INTERP)
    BIND_ENUM_CONSTANT(NOTE)
    BIND_ENUM_CONSTANT(SHLIB)
    BIND_ENUM_CONSTANT(PHDR)
    BIND_ENUM_CONSTANT(TLS)
    BIND_ENUM_CONSTANT(LOW_OS)
    BIND_ENUM_CONSTANT(OS_RANGE)
    BIND_ENUM_CONSTANT(HIGH_OS)
    BIND_ENUM_CONSTANT(LOW_PROC)
    BIND_ENUM_CONSTANT(PROC_RANGE)
    BIND_ENUM_CONSTANT(HIGH_PROC)

    BIND_ENUM_CONSTANT(EXECUTABLE)
    BIND_ENUM_CONSTANT(WRITEABLE)
    BIND_ENUM_CONSTANT(READABLE)
    BIND_ENUM_CONSTANT(NONE)

    ClassDB::bind_static_method("ElfProgramHeader", D_METHOD("load_program_headers", "header", "data"), &ElfProgramHeader::load_program_headers);
}

ElfHeader::ElfHeader(){
    this->magic = PackedByteArray();
    endianness = Endianness::INVALID_ENDIANNESS;
    version = os_abi = abi_version = 0;
    type = ObjectType::INVALID_OBJTYPE;
    machine = elf_version = entry_point = 0;
    program_header_offset = section_header_offset = 0;
    flags = header_size = 0;
    program_header_entry_size = program_header_count = 0;
    section_header_entry_size = section_header_count = 0;
    section_header_name_index = 0;
}

uint64_t ElfHeader::get_expected_header_size(BitSize test_bit_size){
    if(test_bit_size == BitSize::SIZE_32) return 52;
    else if(test_bit_size == BitSize::SIZE_64) return 64;
    else return -1;
}

Ref<ElfHeader> ElfHeader::load_header(PackedByteArray data){
    if(data.size() <= Offsets::BIT_SIZE) return nullptr;
    uint8_t in_bit_size = data.decode_u8(Offsets::BIT_SIZE);
    if(in_bit_size > 2) in_bit_size = BitSize::INVALID_BITSIZE;
    uint8_t ptr_size = in_bit_size == 1 ? 4 : 8;
    uint8_t bit_fixer = in_bit_size == 1 ? 0 : 4; // this will be used to do offset+bit_fixer*i

    if(data.size() < ElfHeader::get_expected_header_size(BitSize(in_bit_size)))
		return nullptr;
    
    Ref<ElfHeader> ret_value;
    ret_value.instantiate();
    ret_value->magic = data.slice(0, 4);
    ret_value->bit_size = BitSize(in_bit_size);

    uint8_t temp_endianness = data.decode_u8(Offsets::ENDIANNESS);
    if(temp_endianness > 2) temp_endianness = Endianness::INVALID_ENDIANNESS;
    ret_value->endianness = Endianness(temp_endianness);

    ret_value->version = get_value_by_bitsize_unsafe(data, Offsets::VERSION, 1);
    ret_value->os_abi = get_value_by_bitsize_unsafe(data, Offsets::OS_ABI, 1);
    ret_value->abi_version = get_value_by_bitsize_unsafe(data, Offsets::ABI_VER, 1);
    
    uint16_t temp_type = data.decode_u16(Offsets::TYPE);
    if(temp_type > ObjectType::LOW_OS && temp_type < ObjectType::HIGH_OS)
        ret_value->type = ObjectType::OS_RANGE;
    else if(temp_type > ObjectType::LOW_PROC && temp_type < ObjectType::HIGH_PROC)
        ret_value->type = ObjectType::PROC_RANGE;
    else if(temp_type > ObjectType::CORE)
        ret_value->type = ObjectType::INVALID_OBJTYPE;
    else ret_value->type = ObjectType(temp_type);

    ret_value->machine = get_value_by_bitsize_unsafe(data, Offsets::MACHINE, 2);
    ret_value->elf_version = get_value_by_bitsize_unsafe(data, Offsets::ELF_VERSION, 4);
    ret_value->entry_point = get_value_by_bitsize_unsafe(data, Offsets::ENTRY_BASE, ptr_size);
    ret_value->program_header_offset = get_value_by_bitsize_unsafe(data, Offsets::PH_OFF_BASE + bit_fixer, ptr_size);
    ret_value->section_header_offset = get_value_by_bitsize_unsafe(data, Offsets::SH_OFF_BASE + (bit_fixer*2), ptr_size);
    //it ends at 3 items to fix for
    bit_fixer *= 3;
    ret_value->flags = get_value_by_bitsize_unsafe(data, Offsets::FLAGS_BASE+bit_fixer, 4);
    ret_value->header_size = get_value_by_bitsize_unsafe(data, Offsets::HEADER_SIZE_BASE+bit_fixer, 2);
    ret_value->program_header_entry_size = get_value_by_bitsize_unsafe(data, Offsets::PH_ENT_SIZE_BASE+bit_fixer, 2);
    ret_value->program_header_count = get_value_by_bitsize_unsafe(data, Offsets::PH_NUM_BASE+bit_fixer, 2);
    ret_value->section_header_entry_size = get_value_by_bitsize_unsafe(data, Offsets::SH_ENT_SIZE_BASE+bit_fixer, 2);
    ret_value->section_header_count = get_value_by_bitsize_unsafe(data, Offsets::SH_NUM_BASE+bit_fixer, 2);
    ret_value->section_header_name_index = get_value_by_bitsize_unsafe(data, Offsets::SECTION_HEADER_NAME_INDX_BASE+bit_fixer, 2);

    return ret_value;
}

ElfProgramHeader::ElfProgramHeader(){
    type = HeaderType::PH_NULL;
    flags = file_offset = 0;
    virtual_address = physical_address = 0;
    size_in_file = size_in_memory = 0;
    align = 0;
}

Dictionary ElfProgramHeader::get_offsets(bool is_64){
    Dictionary ret;
    if(is_64){
        ret["type"] = Offsets_64::TYPE_64;
        ret["flags"] = Offsets_64::FLAGS_64;
        ret["offset"] = Offsets_64::OFFSET_64;
        ret["vaddr"] = Offsets_64::VADDR_64;
        ret["paddr"] = Offsets_64::PADDR_64;
        ret["filesz"] = Offsets_64::FILESZ_64;
        ret["memsz"] = Offsets_64::MEMSZ_64;
        ret["align"] = Offsets_64::ALIGN_64;
    } else{
        ret["type"] = Offsets_32::TYPE_32;
        ret["flags"] = Offsets_32::FLAGS_32;
        ret["offset"] = Offsets_32::OFFSET_32;
        ret["vaddr"] = Offsets_32::VADDR_32;
        ret["paddr"] = Offsets_32::PADDR_32;
        ret["filesz"] = Offsets_32::FILESZ_32;
        ret["memsz"] = Offsets_32::MEMSZ_32;
        ret["align"] = Offsets_32::ALIGN_32;
    }
    return ret;
}

TypedArray<ElfProgramHeader> ElfProgramHeader::load_program_headers(Ref<ElfHeader> header, PackedByteArray data){
    TypedArray<ElfProgramHeader> ret_array;

    uint16_t ph_offset = header->get_program_header_offset(), ph_count = header->get_program_header_count();
    uint16_t ph_entry_size = header->get_program_header_entry_size();

    int64_t offset_end = ph_offset + (ph_count * ph_entry_size);
    if(ph_entry_size <= 0  || ph_count <= 0 || offset_end >= data.size()){
        return ret_array;
    }
    
    PackedByteArray sub_data = data.slice(header->get_program_header_offset(), offset_end);
    uint8_t header_size = header->get_bit_size() == 1 ? 0x20 : 0x38;

    for(int64_t i = 0; i < ph_count; i++){
        Ref<ElfProgramHeader> cur_header;
        cur_header.instantiate();
        Dictionary offsets = ElfProgramHeader::get_offsets(header->get_bit_size() == 2);
        uint64_t cur_offset = i * header_size;
        uint32_t temp_type = get_value_by_bitsize_unsafe(sub_data, (int64_t)offsets["type"]+cur_offset, 4);
        if(temp_type <= HeaderType::TLS || temp_type == HeaderType::LOW_OS || temp_type == HeaderType::HIGH_OS
                || temp_type == HeaderType::LOW_PROC || temp_type == HeaderType::HIGH_PROC)
            cur_header->type = HeaderType(temp_type);
        else if(temp_type > HeaderType::LOW_OS && temp_type < HeaderType::HIGH_OS) cur_header->type = HeaderType::OS_RANGE;
        else if(temp_type > HeaderType::LOW_PROC && temp_type < HeaderType::HIGH_PROC) cur_header->type = HeaderType::PROC_RANGE;
        else temp_type = HeaderType::PH_NULL;
        cur_header->flags = get_value_by_bitsize_unsafe(sub_data, (int64_t)offsets["flags"]+cur_offset, 4);
        cur_header->file_offset = get_value_by_bitsize_unsafe(sub_data, (int64_t)offsets["offset"]+cur_offset, 4);
        cur_header->virtual_address = get_value_by_bitsize_unsafe(sub_data, (int64_t)offsets["vaddr"]+cur_offset, 4);
        cur_header->physical_address = get_value_by_bitsize_unsafe(sub_data, (int64_t)offsets["paddr"]+cur_offset, 4);
        cur_header->size_in_file = get_value_by_bitsize_unsafe(sub_data, (int64_t)offsets["filesz"]+cur_offset, 4);
        cur_header->size_in_memory = get_value_by_bitsize_unsafe(sub_data, (int64_t)offsets["memsz"]+cur_offset, 4);
        cur_header->align = get_value_by_bitsize_unsafe(sub_data, (int64_t)offsets["align"]+cur_offset, 4);
        ret_array.push_back(cur_header);
    }
    
    return ret_array;
}
