#include <gde-virtfs/host_filesys.h>

using namespace godot;

Ref<VirtFileHandle> HostFile::_get_handle(FileAccess::ModeFlags flags){
    Ref<FileAccess> host_file = FileAccess::open(this->path.path_join(this->filename), flags);
    if(host_file.is_null()) return nullptr;

    Ref<HostFileHanlde> ret;
    ret.instantiate();
    ret->set_host_file(host_file);

    return ret;
}

// Because windows compatability, we have to use Godot wrapping instead of lstat
int64_t HostFile::_get_size(){
    Ref<HostFileHanlde> handle = this->_get_handle(FileAccess::ModeFlags::READ);
    if(handle.is_null()) return -1;

    //seek to the end
    handle->seek(0, VFS_SEEK_END);

    int64_t ret = handle->tell();
    handle->close();
    return ret;
}
