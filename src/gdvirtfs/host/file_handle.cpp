#include <gde-virtfs/host_filesys.h>

using namespace godot;

void HostFileHanlde::_bind_methods(){
    BIND_GET_SET_HINT(host_file, HostFileHanlde, OBJECT, PROPERTY_HINT_TYPE_STRING, "FileAccess")
}

PackedByteArray HostFileHanlde::_read(int64_t size){
    if(!this->host_file->is_open()) return PackedByteArray();
    else return this->host_file->get_buffer(size);
}

int64_t HostFileHanlde::_write(PackedByteArray data_to_write){
    if(!this->host_file->is_open()) return 0;

    this->host_file->store_buffer(data_to_write);
    // This isn't necessarily accurate, but TODO: Fix sizing
    return data_to_write.size();
}

VirtFSResult HostFileHanlde::_seek(int64_t new_position, SeekPosition from){
    if(!this->host_file->is_open()) return VFS_GENERIC_ERROR;

    int64_t true_new_position = new_position;
    if(from == VFS_SEEK_CUR){
        //Call the underlying cause we want to bypass the virtual calling and ask the host.
        true_new_position += this->_tell();
        from = VFS_SEEK_SET;
    }

    if(from == VFS_SEEK_SET){
        this->host_file->seek(true_new_position);
    } else if(from == VFS_SEEK_END){
        this->host_file->seek_end(true_new_position);
    } else{
        return VFS_GENERIC_ERROR;
    }

    return VFS_OK;
}

int64_t HostFileHanlde::_tell(){
    if(!this->host_file->is_open()) return -1;
    else return this->host_file->get_position();
}

VirtFSResult HostFileHanlde::_flush(){
    if(!this->host_file->is_open()) return VFS_GENERIC_ERROR;

    this->host_file->flush();
    return VFS_OK;
}

void HostFileHanlde::_close(){
    if(!this->host_file->is_open()) return;

    this->host_file->close();
}
