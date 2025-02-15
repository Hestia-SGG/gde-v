#include "gde-virtfs/filesys.h"

using namespace godot;

void VirtFileHandle::_bind_methods(){
    BIND_ENUM_CONSTANT(VFS_SEEK_SET)
    BIND_ENUM_CONSTANT(VFS_SEEK_CUR)
    BIND_ENUM_CONSTANT(VFS_SEEK_END)

    GDVIRTUAL_BIND(_read, "size");
    GDVIRTUAL_BIND(_write, "data_to_write");
    GDVIRTUAL_BIND(_seek, "new_position", "from");
    GDVIRTUAL_BIND(_tell);
    GDVIRTUAL_BIND(_flush);
    GDVIRTUAL_BIND(_close);
    ClassDB::bind_method(D_METHOD("read", "size"), &VirtFileHandle::read);
    ClassDB::bind_method(D_METHOD("write", "data_to_write"), &VirtFileHandle::write);
    ClassDB::bind_method(D_METHOD("seek", "new_position", "from"), &VirtFileHandle::seek);
    ClassDB::bind_method(D_METHOD("tell"), &VirtFileHandle::tell);
    ClassDB::bind_method(D_METHOD("flush"), &VirtFileHandle::flush);
    ClassDB::bind_method(D_METHOD("close"), &VirtFileHandle::close);

    BIND_GET_SET(handle_id, VirtFileHandle, INT, "new_val")
}

VIRT_CALL_WRAPPER1R(PackedByteArray, VirtFileHandle, read, int64_t, size)
VIRT_CALL_WRAPPER1R(int64_t, VirtFileHandle, write, PackedByteArray, data_to_write)
VIRT_CALL_WRAPPER2R(VirtFSResult, VirtFileHandle, seek, int64_t, new_position, SeekPosition, from)
VIRT_CALL_WRAPPER0R(int64_t, VirtFileHandle, tell)
VIRT_CALL_WRAPPER0R(VirtFSResult, VirtFileHandle, flush)
VIRT_CALL_WRAPPER0(VirtFileHandle, close)

void VirtFile::_bind_methods(){
    BIND_ENUM_CONSTANT(FM_INVALID)
    BIND_ENUM_CONSTANT(FM_REG)
    BIND_ENUM_CONSTANT(FM_DIR)
    BIND_ENUM_CONSTANT(FM_CHAR_DEVICE)
    BIND_ENUM_CONSTANT(FM_FIFO)
    BIND_ENUM_CONSTANT(FM_LINK)

    BIND_GET_SET(owner_uid, VirtFile, INT, "new_val")
    BIND_GET_SET(group_uid, VirtFile, INT, "new_val")
    BIND_GET_SET(access_time, VirtFile, INT, "new_val")
    BIND_GET_SET(modified_time, VirtFile, INT, "new_val")
    BIND_GET_SET(created_time, VirtFile, INT, "new_val")
    BIND_GET_SET(file_mode, VirtFile, INT, "new_val")
    
    ClassDB::bind_method(D_METHOD("is_invalid"), &VirtFile::is_invalid);
    ClassDB::bind_method(D_METHOD("is_file"), &VirtFile::is_file);
    ClassDB::bind_method(D_METHOD("is_directory"), &VirtFile::is_directory);
    ClassDB::bind_method(D_METHOD("is_character_device"), &VirtFile::is_character_device);
    ClassDB::bind_method(D_METHOD("is_fifo"), &VirtFile::is_fifo);
    ClassDB::bind_method(D_METHOD("is_link"), &VirtFile::is_link);

    BIND_GET_SET(path, VirtFile, STRING, "new_val")
    BIND_GET_SET(filename, VirtFile, STRING, "new_val")
    BIND_GET_SET(links_to, VirtFile, STRING, "new_links_to")

    GDVIRTUAL_BIND(_get_handle);
    ClassDB::bind_method(D_METHOD("get_handle"), &VirtFile::get_handle);
}

VIRT_CALL_WRAPPER0R(Ref<VirtFileHandle>, VirtFile, get_handle)

VirtFile::VirtFile(){
    this->owner_uid = this->group_uid = -1;
    this->access_time = this->modified_time = this->created_time = 0;
    this->file_mode = FileMode::FM_INVALID;
    path = "";
    filename = "";
    links_to = "";
}

void VirtFileSystem::_bind_methods(){
    BIND_ENUM_CONSTANT(VFS_OK)
    BIND_ENUM_CONSTANT(VFS_GENERIC_ERROR)
    BIND_ENUM_CONSTANT(VFS_INVALID_PATH)
    BIND_ENUM_CONSTANT(VFS_INVALID_PERMS)

    GDVIRTUAL_BIND(_find_file, "filename");
    GDVIRTUAL_BIND(_create_file, "filename", "new_file_mode");
    GDVIRTUAL_BIND(_delete_file, "filename");
    GDVIRTUAL_BIND(_create_file_from_ref, "new_file", "duplicate");
    ClassDB::bind_method(D_METHOD("find_file", "filename"), &VirtFileSystem::find_file);
    ClassDB::bind_method(D_METHOD("create_file", "filename", "new_file_mode"), &VirtFileSystem::create_file);
    ClassDB::bind_method(D_METHOD("delete_file", "filename"), &VirtFileSystem::delete_file);
    ClassDB::bind_method(D_METHOD("create_file_from_ref", "new_file", "duplicate"), &VirtFileSystem::create_file_from_ref);
}

VIRT_CALL_WRAPPER1R(Ref<VirtFile>, VirtFileSystem, find_file, String, filename)
VIRT_CALL_WRAPPER2R(Ref<VirtFile>, VirtFileSystem, create_file, String, filename, FileMode, new_file_mode)
VIRT_CALL_WRAPPER1R(VirtFSResult, VirtFileSystem, delete_file, String, filename)
VIRT_CALL_WRAPPER2R(Ref<VirtFile>, VirtFileSystem, create_file_from_ref, Ref<VirtFile>, new_file, bool, duplicate)
