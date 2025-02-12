#ifndef _GDE_V_ELF_H_
#define _GDE_V_ELF_H_

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <gde-v/utils.h>
#include <gde-v/bus_device.h>

namespace godot{

/**
 * Definition for an ELF header. Implemented as a Resource so it can be saved off if wanted.
 */
class ElfHeader : public Resource{
    GDCLASS(ElfHeader, Resource)

protected:
    // Standard Godot binding method to let it know about our class.
    static void _bind_methods();

public:
    // Gives all of the offsets for the header. Ones that end in _BASE define the 32bit version
    // from which the 64 bit version is calculated.
    enum Offsets{ MAGIC = 0, BIT_SIZE = 4, ENDIANNESS = 5, VERSION = 6, OS_ABI = 7, ABI_VER = 8,
        PAD_1_BEGIN = 9, TYPE = 0x10, MACHINE = 0x12, ELF_VERSION = 0x14, ENTRY_BASE = 0x18,
        PH_OFF_BASE = 0x1C, SH_OFF_BASE = 0x20, FLAGS_BASE = 0x24, HEADER_SIZE_BASE = 0x28,
        PH_ENT_SIZE_BASE = 0x2A, PH_NUM_BASE = 0x2C, SH_ENT_SIZE_BASE = 0x2E, SH_NUM_BASE = 0x30,
        SECTION_HEADER_NAME_INDX_BASE = 0x32, END_BASE = 0x34 };
    // Definitions for types of endianness.
    enum Endianness{ LITTLE = 1, BIG = 2, INVALID_ENDIANNESS = 0 };
    // Definitions for bit size. Try to use these instead of 32 or 64.
    enum BitSize{ SIZE_32 = 1, SIZE_64 = 2, INVALID_BITSIZE = 0 };
    // The type of object this is. The _RANGE values are used to indicate the loaded thing is a value
    // in there. This isn't really good, but TODO fix it.
    enum ObjectType{ INVALID_OBJTYPE = -1, NONE = 0, RELOC = 1, EXEC = 2, SHARED = 3,
        DYNAMIC = 3, CORE = 4, LOW_OS = 0xFE00, OS_RANGE = 0xFE01, HIGH_OS = 0xFEFF,
        LOW_PROC = 0xFF00, PROC_RANGE = 0xFF01, HIGH_PROC = 0xFFFF };

protected:
    Variant magic;
    BitSize bit_size;
    Endianness endianness;
    int64_t version, os_abi, abi_version;
    ObjectType type;
    int64_t machine, elf_version, entry_point;
    int64_t program_header_offset, section_header_offset, flags, header_size;
    int64_t program_header_entry_size, program_header_count;
    int64_t section_header_entry_size, section_header_count;
    uint64_t section_header_name_index;

    /**
     * Gets this object as a dictionary of parameters. Used for the _to_string method.
     */
    Dictionary get_str_params(){
        Dictionary ret;
        ret["magic"] = magic;
        ret["bit_size"] = bit_size;
        ret["endianness"] = endianness;
        ret["version"] = version;
        ret["os_abi"] = os_abi;
        ret["abi_version"] = abi_version;
        ret["type"] = type;
        ret["machine"] = machine;
        ret["elf_version"] = elf_version;
        ret["entry_point"] = String("%x") % entry_point;
        ret["program_header_offset"] = String("%x") % program_header_offset;
        ret["section_header_offset"] = String("%x") % section_header_offset;
        ret["flags"] = flags;
        ret["header_size"] = header_size;
        ret["program_header_entry_size"] = program_header_entry_size;
        ret["program_header_count"] = program_header_count;
        ret["section_header_entry_size"] = section_header_entry_size;
        ret["section_header_count"] = section_header_count;
        ret["section_header_name_index"] = section_header_name_index;
        ret["valid"] = is_valid();
        return ret;
    }

public:
    ElfHeader();
    ~ElfHeader(){}

    /**
     * Get/set for the fields.
     */
    GET_SET(magic, PackedByteArray)
    GET_SET(bit_size, BitSize)
    GET_SET(endianness, Endianness)
    GET_SET(version, int64_t)
    GET_SET(os_abi, int64_t)
    GET_SET(abi_version, int64_t)
    GET_SET(type, ObjectType)
    GET_SET(machine, int64_t)
    GET_SET(elf_version, int64_t)
    GET_SET(entry_point, int64_t)
    GET_SET(program_header_offset, int64_t)
    GET_SET(section_header_offset, int64_t)
    GET_SET(flags, int64_t)
    GET_SET(header_size, int64_t)
    GET_SET(program_header_entry_size, int64_t)
    GET_SET(program_header_count, int64_t)
    GET_SET(section_header_entry_size, int64_t)
    GET_SET(section_header_count, int64_t)
    GET_SET(section_header_name_index, int64_t)

    // Use the static version
    int64_t get_pointer_size(){
        return ElfHeader::get_pointer_size_s(this->bit_size);
    }

    // Returns how big a pointer is, so we can try not to hardcode the check everywhere.
    static int64_t get_pointer_size_s(BitSize test_bit_size){
        if(test_bit_size == BitSize::SIZE_32) return 4;
		else if(test_bit_size == BitSize::SIZE_64) return 8;
		else return 0;
    }

    // Use the static version
    int64_t get_expected_header_size(){
        return ElfHeader::get_expected_header_size_s(this->bit_size);
    }

    // Returns how big the header should be based on bit size.
    static uint64_t get_expected_header_size_s(BitSize test_bit_size){
        if(test_bit_size == BitSize::SIZE_32) return 52;
        else if(test_bit_size == BitSize::SIZE_64) return 64;
        else return -1;
    }

    // Returns if this object is a valid ELF header. Mainly just checks if they're not -1, other than header.
    bool is_valid(){
        // WHY CAN'T WE DO STATIC CONST PACKEDBYTEARRAY?!?!
        PackedByteArray _good_magic = PackedByteArray({0x7F, 0x45, 0x4C, 0x46});

        return magic.get_type() == Variant::PACKED_BYTE_ARRAY && ((PackedByteArray)magic) == _good_magic
            && bit_size != BitSize::INVALID_BITSIZE && endianness != Endianness::INVALID_ENDIANNESS
			&& version != -1 && abi_version != -1 && type != ObjectType::INVALID_OBJTYPE
			&& machine != -1 && elf_version != -1 && entry_point != -1
			&& program_header_offset != -1 && section_header_offset != -1
			&& flags != -1 && header_size != -1 && program_header_entry_size != -1
			&& program_header_count != -1 && section_header_entry_size != -1
			&& section_header_count != -1 && section_header_name_index != -1;
    }

    // Convert this into a string.
    String _to_string(){
        String header_print_formatter = "Magic: {magic}\nBit Size: {bit_size}\nEndianness: {endianness}\nVersion: {version}\nOS ABI: {os_abi}\nABI Version: {abi_version}\nType: {type}\nMachine: {machine}\nElf Version : {elf_version}\nEntry Point: 0x{entry_point}\nProgram Header Offset: 0x{program_header_offset}\nSection Header Offset: 0x{section_header_offset}\nFlags: {flags}\nHeader Size: {header_size}\nProgram Header Entry Size: {program_header_entry_size}\nProgram Header Count: {program_header_count}\nSection Header Entry Size: {section_header_entry_size}\nSection Header Count: {section_header_count}\nSection Header Name Index: {section_header_name_index}\nValid: {valid}";
        return header_print_formatter.format(this->get_str_params());
    }

    // Load a header from the given data slice.
    static Ref<ElfHeader> load_header(PackedByteArray data);
};

/**
 * Base class for a Program Header in an ELF.
 */
class ElfProgramHeader : public Resource{
    GDCLASS(ElfProgramHeader, Resource)

public:
    // Because the offsets are so different, easier to just define them for 32 and 64 bits.
    enum Offsets_32 { TYPE_32 = 0x00, FLAGS_32 = 0x18, OFFSET_32 = 0x04, VADDR_32 = 0x08, PADDR_32 = 0x0C, FILESZ_32 = 0x10, MEMSZ_32 = 0x14, ALIGN_32 = 0x1C };
	enum Offsets_64 { TYPE_64 = 0x00, FLAGS_64 = 0x04, OFFSET_64 = 0x08, VADDR_64 = 0x10, PADDR_64 = 0x18, FILESZ_64 = 0x20, MEMSZ_64 = 0x28, ALIGN_64 = 0x30 };
	// The type of header this is. The _RANGE values are used to indicate the loaded thing is a value
    // in there. This isn't really good, but TODO fix it.
    enum HeaderType { PH_NULL = 0, LOAD = 1, DYNAMIC = 2, INTERP = 3, NOTE = 4,
		SHLIB = 5, PHDR = 6, TLS = 7, LOW_OS = 0x60000000, OS_RANGE = 0x60000001,
		HIGH_OS = 0x6FFFFFFF, LOW_PROC = 0x70000000, PROC_RANGE = 0x70000001,
		HIGH_PROC = 0x7FFFFFFF };
	enum Flags { EXECUTABLE = 1, WRITEABLE = 2, READABLE = 4, NONE = 0 };

protected:
    static void _bind_methods();
	
	HeaderType type;
	int64_t flags;
	int64_t file_offset;
	int64_t virtual_address;
	int64_t physical_address;
	int64_t size_in_file;
	int64_t size_in_memory;
	int64_t align;

public:
    // Gets the offsets, for either 32 or 64 bit, since we can't static const allocate these.
    static Dictionary get_offsets(bool is_64);

    ElfProgramHeader();
    ~ElfProgramHeader(){}

    // Get/set for the properties.
    GET_SET(type, HeaderType)
    GET_SET(flags, int64_t)
    GET_SET(file_offset, int64_t)
    GET_SET(virtual_address, int64_t)
    GET_SET(physical_address, int64_t)
    GET_SET(size_in_file, int64_t)
    GET_SET(size_in_memory, int64_t)
    GET_SET(align, int64_t)

    // Determines if this header is valid.
    bool is_valid(){
		bool test_align = false;
		if(align == 0 || align == 1) test_align = true;
		else test_align = ((file_offset % align) == (virtual_address % align));
		return (type <= HeaderType::TLS || (type >= HeaderType::LOW_OS && type <= HeaderType::HIGH_OS) || (type >= HeaderType::LOW_PROC && type <= HeaderType::HIGH_PROC))
			&& (flags >= 0 && flags <= 7) && file_offset != -1 && virtual_address != -1
			&& physical_address != -1 && size_in_file != -1 && size_in_memory != -1
			&& test_align;
    }

    // Loads all the arrays in the data slice. Use the header to know the offset to start and bit size. MUST NOT BE SLICE,
    // must be whole file.
    static TypedArray<ElfProgramHeader> load_program_headers(Ref<ElfHeader> header, PackedByteArray data);
};

/**
 * Represents a full ELF file.
 * 
 * TODO: Implement reading segments and handling the full RISC-V extras.
 */
class ElfFile : public Resource{
    GDCLASS(ElfFile, Resource)
protected:
    static void _bind_methods();

    Ref<ElfHeader> header;
    TypedArray<ElfProgramHeader> program_headers;

    PackedByteArray raw_data;
    bool loaded;
public:
    ElfFile(){}
    ~ElfFile(){}

    GET_NOSET(header, Ref<ElfHeader>)
    GET_NOSET(program_headers, TypedArray<ElfProgramHeader>)
    GET_NOSET(loaded, bool)

    // Loads a header from the given data. Broken out so that it can be reset with new info
    // during runtime.
    void load_data(Variant data);

    // Returns this turned into a printable string.
    // TODO: Add the rest of the info.
    String _to_string(){ return "Header:\n\n" + header->to_string(); }

    // Generates the list of memory areas needed to be created. The Dictionary will be of the format:
    // {address_1: Ref<RVBusDevice>, address_2: Ref<RVBusDevice>, ..., address_n: Ref<RVBusDevice>}
    // where address_n is an int whose value is 0 <= address_n <= 0xFFFFFFFF
    Dictionary generate_memory_areas();

    // Load an ELF file by creating a new instance, calling load_data, and returning.
    // On error, returns nullptr
    static Ref<ElfFile> load_elf_data(Variant data);
};

}

//All of the enum casting
VARIANT_ENUM_CAST(ElfHeader::Offsets);
VARIANT_ENUM_CAST(ElfHeader::Endianness);
VARIANT_ENUM_CAST(ElfHeader::BitSize);
VARIANT_ENUM_CAST(ElfHeader::ObjectType);

VARIANT_ENUM_CAST(ElfProgramHeader::Offsets_32);
VARIANT_ENUM_CAST(ElfProgramHeader::Offsets_64);
VARIANT_ENUM_CAST(ElfProgramHeader::HeaderType);
VARIANT_ENUM_CAST(ElfProgramHeader::Flags);

#endif