#include <gde-virtfs/filesys.h>

using namespace godot;

void VirtFSDispatcher::_bind_methods(){
    BIND_GET_SET_HINT(fs_handlers, VirtFSDispatcher, Variant::ARRAY, PROPERTY_HINT_TYPE_STRING, "VirtFileSystem");
    //ClassDB::bind_method(D_METHOD("add_handler", "new_handler"), &VirtFSDispatcher::add_handler);
    //ClassDB::bind_method(D_METHOD("remove_handler", "handler_to_remove"), &VirtFSDispatcher::remove_handler);
    ClassDB::bind_method(D_METHOD("find_file", "filename"), &VirtFSDispatcher::find_file);
    ClassDB::bind_method(D_METHOD("create_file", "filename", "new_file_mode"), &VirtFSDispatcher::create_file);
    ClassDB::bind_method(D_METHOD("delete_file", "filename"), &VirtFSDispatcher::delete_file);
    ClassDB::bind_method(D_METHOD("move_file", "filename", "move_filename"), &VirtFSDispatcher::move_file);
    ClassDB::bind_method(D_METHOD("copy_file", "filename", "copy_filename"), &VirtFSDispatcher::copy_file);

    ClassDB::bind_static_method("VirtFSDispatcher", D_METHOD("normalize_path", "path"), &VirtFSDispatcher::normalize_path);
}

VirtFSDispatcher::VirtFSDispatcher(){
    this->fs_handlers = {};
}

VirtFSDispatcher::~VirtFSDispatcher(){
    
}

Ref<VirtFile> VirtFSDispatcher::find_file(String filename){
    Ref<VirtFile> to_return = nullptr;
    String filename_normalized = VirtFSDispatcher::normalize_path(filename);
    for(int64_t i = 0; i < this->fs_handlers.size(); i++){
        Ref<VirtFileSystem> cur_fs = this->fs_handlers[i];
        if(cur_fs.is_null()) continue;

        to_return = cur_fs->find_file(filename_normalized);
        if(to_return.is_valid()) break;
    }

    return to_return;
}

Ref<VirtFile> VirtFSDispatcher::create_file(String filename, FileMode new_file_mode){
    Ref<VirtFile> to_return = this->find_file(filename);
    if(to_return.is_valid()) return to_return;

    String filename_normalized = VirtFSDispatcher::normalize_path(filename);

    for(int64_t i = 0; i < this->fs_handlers.size(); i++){
        Ref<VirtFileSystem> cur_fs = this->fs_handlers[i];
        if(cur_fs.is_null()) continue;

        to_return = cur_fs->create_file(filename_normalized, new_file_mode);
        if(to_return.is_valid()) break;
    }

    return to_return;
}

VirtFSResult VirtFSDispatcher::delete_file(String filename){
    String filename_normalized = VirtFSDispatcher::normalize_path(filename);
    VirtFSResult to_return = VFS_GENERIC_ERROR;

    for(int64_t i = 0; i < this->fs_handlers.size(); i++){
        Ref<VirtFileSystem> cur_fs = this->fs_handlers[i];
        if(cur_fs.is_null()) continue;

        to_return = cur_fs->delete_file(filename_normalized);
        if(to_return == VFS_OK) return VFS_OK;
    }

    // Failed to service
    return VFS_GENERIC_ERROR;
}

// TODO: Make this better.
VirtFSResult VirtFSDispatcher::move_copy_file(String filename, String move_filename, bool copy){
    String filename_normalized = VirtFSDispatcher::normalize_path(filename);

    Ref<VirtFile> old_file = this->find_file(filename_normalized);
    if(old_file.is_null()) return VFS_INVALID_PATH;

    String move_filename_normalized = VirtFSDispatcher::normalize_path(move_filename);

    //old_file->set_path(move_filename_normalized.get_base_dir());
    //old_file->set_filename(move_filename_normalized.get_file());

    Ref<VirtFile> new_file = nullptr;

    for(int64_t i = 0; i < this->fs_handlers.size(); i++){
        Ref<VirtFileSystem> cur_fs = this->fs_handlers[i];
        if(cur_fs.is_null()) continue;

        new_file = cur_fs->create_file_from_ref(old_file, move_filename_normalized, copy);
        if(new_file.is_valid()) break;
    }

    if(new_file.is_valid()){
        // If the source and destination filesystems are the same, the underlying fs can choose to keep the same reference
        // but repoint it. If not, and we aren't copying, delete the souce.
        VirtFSResult to_return = VFS_OK;
        if(new_file != old_file && !copy) to_return = this->delete_file(filename_normalized);
        return to_return;
    }
    else return VFS_GENERIC_ERROR;
}

VirtFSResult VirtFSDispatcher::move_file(String filename, String move_filename){
    return this->move_copy_file(filename, move_filename, false);
}

VirtFSResult VirtFSDispatcher::copy_file(String filename, String copy_filename){
    return this->move_copy_file(filename, copy_filename, true);
}

String VirtFSDispatcher::normalize_path(String path){
    String ret = path.begins_with("/") ? path : ("/" + path);
    return ret.simplify_path();
}
