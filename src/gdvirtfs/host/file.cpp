#include <gde-virtfs/host_filesys.h>

using namespace godot;

void HostFile::_bind_methods(){
    BIND_GET_SET(host_mapped_path, HostFile, STRING)
}

Ref<VirtFileHandle> HostFile::_get_handle(FileAccess::ModeFlags flags){
    String path_to_access = this->path.path_join(this->filename);
    path_to_access = this->host_mapped_path.path_join(path_to_access);
    Ref<FileAccess> host_file = FileAccess::open(path_to_access, flags);
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

void HostFile::set_host_mapped_path(String new_path){
    if(this->host_mapped_path == ""){
        if(DirAccess::dir_exists_absolute(new_path) || FileAccess::file_exists(new_path)){
            this->host_mapped_path = new_path;
        }
    }
}
