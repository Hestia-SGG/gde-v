#ifndef _GDE_VIRTFS_HOST_FILESYS_H_
#define _GDE_VIRTFS_HOST_FILESYS_H_

#include "gde-virtfs/filesys.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/dir_access.hpp>

namespace godot{

class HostFileHanlde : public VirtFileHandle{
    GDCLASS(HostFileHanlde, VirtFileHandle)

protected:
    static void _bind_methods();

    virtual PackedByteArray _read(int64_t size) override;
    virtual int64_t _write(PackedByteArray data_to_write) override;
    virtual VirtFSResult _seek(int64_t new_position, SeekPosition from) override;
    virtual int64_t _tell() override;
    virtual VirtFSResult _flush() override;
    virtual void _close() override;

    Ref<FileAccess> host_file;
public:
    HostFileHanlde(){ this->host_file = Ref<FileAccess>(); }
    ~HostFileHanlde(){}

    GET_NOSET(host_file, Ref<FileAccess>)
    void set_host_file(Ref<FileAccess> new_host_file){ if(this->host_file.is_null()) this->host_file = new_host_file; }
};

class HostFile : public VirtFile{
    GDCLASS(HostFile, VirtFile)

protected:
    static void _bind_methods();

    virtual Ref<VirtFileHandle> _get_handle(FileAccess::ModeFlags flags) override;

    virtual int64_t _get_size() override;

    String host_mapped_path;
public:
    HostFile() : VirtFile(){}
    ~HostFile(){}

    GET_NOSET(host_mapped_path, String)
    void set_host_mapped_path(String new_path);
};

class HostFileSystem : public VirtFileSystem{
    GDCLASS(HostFileSystem, VirtFileSystem)

protected:
    static void _bind_methods();

    virtual Ref<VirtFile> _find_file(String filename) override;
    virtual Ref<VirtFile> _create_file(String filename, FileMode new_file_mode) override;
    virtual VirtFSResult _delete_file(String filename) override;

    virtual Ref<VirtFile> _create_file_from_ref(Ref<VirtFile> old_file, String new_filename, bool duplicate = false) override;

    // The directory that's treated as root
    Ref<DirAccess> host_directory;
    String host_directory_path;
public:
    HostFileSystem();
    ~HostFileSystem(){}

    GET_NOSET(host_directory_path, String)
    void set_host_directory_path(String new_host_directory_path);
};

}

#endif
