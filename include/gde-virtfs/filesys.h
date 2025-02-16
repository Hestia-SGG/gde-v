#ifndef _GDVIRTFS_FILESYS_H_
#define _GDVIRTFS_FILESYS_H_

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>
#include <godot_cpp/classes/file_access.hpp>

#include <gde-v/utils.h>

#include <sys/types.h>

enum VirtFSResult : int64_t{
    VFS_OK = 0,
    VFS_GENERIC_ERROR = 1,
    VFS_INVALID_PATH = 2,
    VFS_INVALID_PERMS = 3
};

enum SeekPosition : int64_t{
    VFS_SEEK_SET = 0,
    VFS_SEEK_CUR = 1,
    VFS_SEEK_END = 2
};

enum FileMode{
    FM_INVALID = 0,
    FM_REG = 1,
    FM_DIR = 2,
    FM_CHAR_DEVICE = 3,
    FM_FIFO = 4,
    FM_LINK = 5
};

VARIANT_ENUM_CAST(VirtFSResult);
VARIANT_ENUM_CAST(SeekPosition);
VARIANT_ENUM_CAST(FileMode);

namespace godot{

class VirtFileHandle : public RefCounted{
    GDCLASS(VirtFileHandle, RefCounted)

protected:
    static void _bind_methods();

    virtual PackedByteArray _read(int64_t size){ return PackedByteArray(); }
    virtual int64_t _write(PackedByteArray data_to_write){ return 0; }
    virtual VirtFSResult _seek(int64_t new_position, SeekPosition from){ return VFS_GENERIC_ERROR; }
    virtual int64_t _tell(){ return 0; }
    virtual VirtFSResult _flush(){ return VFS_GENERIC_ERROR; }
    virtual void _close(){}

    int64_t handle_id;
public:
    VirtFileHandle(){ this->handle_id = -1; }
    ~VirtFileHandle(){}

    GDVIRTUAL1R(PackedByteArray, _read, int64_t)
    GDVIRTUAL1R(int64_t, _write, PackedByteArray)
    GDVIRTUAL2R(VirtFSResult, _seek, int64_t, SeekPosition)
    GDVIRTUAL0R(int64_t, _tell)
    GDVIRTUAL0R(VirtFSResult, _flush)
    GDVIRTUAL0(_close)
    PackedByteArray read(int64_t size);
    int64_t write(PackedByteArray data_to_write);
    VirtFSResult seek(int64_t new_position, SeekPosition from);
    int64_t tell();
    VirtFSResult flush();
    void close();

    GET_NOSET(handle_id, int64_t)
    void set_handle_id(int64_t new_val){ if(this->handle_id == -1) this->handle_id = new_val; }
};

class VirtFile : public Resource{
    GDCLASS(VirtFile, Resource)

protected:
    static void _bind_methods();

    int64_t owner_uid, group_uid;
    int64_t access_time, modified_time, created_time;
    FileMode file_mode;

    String path, filename;
    String links_to;

    virtual Ref<VirtFileHandle> _get_handle(FileAccess::ModeFlags flags){ return nullptr; }

    virtual int64_t _get_size(){ return -1; }
public:
    VirtFile();
    ~VirtFile(){}

    GET_SET(owner_uid, int64_t)
    GET_SET(group_uid, int64_t)
    GET_SET(access_time, int64_t)
    GET_SET(modified_time, int64_t)
    GET_SET(created_time, int64_t)

    FileMode get_file_mode(){ return this->file_mode; }
    void set_file_mode(FileMode new_file_mode){ if(this->is_invalid() && new_file_mode != FileMode::FM_INVALID) this->file_mode = new_file_mode; }
    
    bool is_invalid(){ return this->file_mode == FileMode::FM_INVALID; }
    bool is_file(){ return this->file_mode == FileMode::FM_REG; }
    bool is_directory(){ return this->file_mode == FileMode::FM_DIR; }
    bool is_character_device(){ return this->file_mode == FileMode::FM_CHAR_DEVICE; }
    bool is_fifo(){ return this->file_mode == FileMode::FM_FIFO; }
    bool is_link(){ return this->file_mode == FileMode::FM_LINK; }

    GET_SET(path, String)
    GET_SET(filename, String)
    String get_links_to(){ if(this->is_link()) return this->links_to; else return ""; }
    void set_links_to(String new_links_to){ if(this->is_link()) this->links_to = new_links_to;}

    GDVIRTUAL1R(Ref<VirtFileHandle>, _get_handle, FileAccess::ModeFlags)
    Ref<VirtFileHandle> get_handle(FileAccess::ModeFlags flags);

    GDVIRTUAL0R(int64_t, _get_size)
    int64_t get_size();
};

class VirtFSDispatcher;

class VirtFileSystem : public Resource{
    GDCLASS(VirtFileSystem, Resource)

protected:
    static void _bind_methods();

    virtual Ref<VirtFile> _find_file(String filename){ return nullptr; }
    virtual Ref<VirtFile> _create_file(String filename, FileMode new_file_mode){ return nullptr; }
    virtual VirtFSResult _delete_file(String filename){ return VFS_GENERIC_ERROR; }

    virtual Ref<VirtFile> _create_file_from_ref(Ref<VirtFile> file_ref, String new_filename, bool duplicate = false){ return nullptr; }
public:
    VirtFileSystem(){}
    ~VirtFileSystem(){}

    GDVIRTUAL1R(Ref<VirtFile>, _find_file, String)
    GDVIRTUAL2R(Ref<VirtFile>, _create_file, String, FileMode)
    GDVIRTUAL1R(VirtFSResult, _delete_file, String)
    Ref<VirtFile> find_file(String filename);
    Ref<VirtFile> create_file(String filename, FileMode new_file_mode);
    VirtFSResult delete_file(String filename);

    //Specifically for copy and move support
    GDVIRTUAL3R(Ref<VirtFile>, _create_file_from_ref, Ref<VirtFile>, String, bool)
    Ref<VirtFile> create_file_from_ref(Ref<VirtFile> file_ref, String new_filename, bool duplicate = false);
};

class VirtFSDispatcher : public Resource{
    GDCLASS(VirtFSDispatcher, Resource)

protected:
    static void _bind_methods();

    TypedArray<VirtFileSystem> fs_handlers;

    VirtFSResult move_copy_file(String filename, String move_copy_filename, bool copy);
public:
    VirtFSDispatcher();
    ~VirtFSDispatcher();

    bool add_handler(Ref<VirtFileSystem> new_handler);
    bool remove_handler(Ref<VirtFileSystem> handler_to_remove);

    Ref<VirtFile> find_file(String filename);
    Ref<VirtFile> create_file(String filename, FileMode new_file_mode);
    VirtFSResult delete_file(String filename);
    VirtFSResult move_file(String filename, String move_filename);
    VirtFSResult copy_file(String filename, String copy_filename);

    static String normalize_path(String path);
};

}

#endif