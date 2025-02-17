#include <gde-virtfs/host_filesys.h>

#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <sys/stat.h>

using namespace godot;

void HostFileSystem::_bind_methods(){
    BIND_GET_SET(host_directory_path, HostFileSystem, STRING, "new_host_directory_path")
}

HostFileSystem::HostFileSystem(){
    
}

Ref<VirtFile> HostFileSystem::_find_file(String filename){
    if(this->host_directory.is_null()) return nullptr;

    if(!filename.begins_with("/")) return nullptr;

    String normalized_filename = VirtFSDispatcher::normalize_path(filename);
    // Sanity check
    if(filename.contains("/./") || filename.contains("/../")) return nullptr;

    normalized_filename = normalized_filename.right(-1); // remove the leading "/";

    FileMode ret_filemode = FM_INVALID;
    //first try to find it as a folder
    if(this->host_directory->dir_exists(normalized_filename)){
        ret_filemode = FM_DIR;
    } else if(this->host_directory->file_exists(normalized_filename)){
        ret_filemode = FM_REG;
    }

    if(ret_filemode == FM_INVALID) return nullptr;

    String links_to = "";
    String current_dir = this->host_directory->get_current_dir();

    if(this->host_directory->is_link(normalized_filename)){
        ret_filemode = FM_LINK;
        links_to = this->host_directory->read_link(normalized_filename);

        if(!links_to.begins_with(current_dir)) return nullptr;
    }

    Ref<HostFile> ret;
    ret.instantiate();
    ret->set_file_mode(ret_filemode);
    ret->set_path("/" + normalized_filename.get_base_dir());
    ret->set_filename(normalized_filename.get_file());
    ret->set_links_to(links_to);
    ret->set_host_mapped_path(current_dir);

    //TODO: Fix this so works in all OSs.
    struct stat filestats;
    String full_path = current_dir + "/" + normalized_filename;
    CharString full_path_cstr = ret_filemode == FM_LINK ? links_to.ascii() : full_path.ascii();
    int stat_ret = stat(full_path_cstr.get_data(), &filestats);

    // Initialize defaults if can't read
    if(stat_ret != 0){
        return ret;
    }

    ret->set_access_time(filestats.st_atime);
    ret->set_modified_time(filestats.st_mtime);
    ret->set_created_time(filestats.st_ctime);

    // TODO: add mapping from host to virtual IDs
    ret->set_owner_uid(filestats.st_uid);
    ret->set_group_uid(filestats.st_gid);
    return ret;
}

Ref<VirtFile> HostFileSystem::_create_file(String filename, FileMode new_file_mode){
    if(this->host_directory.is_null()) return nullptr;

    if(!filename.begins_with("/")) return nullptr;
    String normalized_filename = VirtFSDispatcher::normalize_path(filename);
    // Sanity check
    if(filename.contains("/./") || filename.contains("/../")) return nullptr;
    normalized_filename = normalized_filename.right(-1); // remove the leading "/";

    Error res = Error::OK;
    if(new_file_mode == FM_DIR){
        res = this->host_directory->make_dir(normalized_filename);
    } else if(new_file_mode == FM_REG){
        Ref<FileAccess> test_create = FileAccess::open(this->host_directory_path.path_join(normalized_filename), FileAccess::ModeFlags::WRITE);
        if(test_create.is_null()) res = FileAccess::get_open_error();
    }

    if(res != Error::OK) return nullptr;
    else return this->find_file(filename);
}

VirtFSResult HostFileSystem::_delete_file(String filename){
    if(this->host_directory.is_null()) return VirtFSResult::VFS_GENERIC_ERROR;

    if(!filename.begins_with("/")) return VirtFSResult::VFS_INVALID_PATH;
    String normalized_filename = VirtFSDispatcher::normalize_path(filename);
    // Sanity check
    if(filename.contains("/./") || filename.contains("/../")) return VirtFSResult::VFS_INVALID_PATH;
    normalized_filename = normalized_filename.right(-1); // remove the leading "/";

    Error remove_ret = this->host_directory->remove(normalized_filename);
    
    if(remove_ret != Error::OK) return VFS_GENERIC_ERROR;
    else return VFS_OK;
}

Ref<VirtFile> HostFileSystem::_create_file_from_ref(Ref<VirtFile> old_file, String new_filename, bool duplicate){
    return nullptr;
}

void HostFileSystem::set_host_directory_path(String new_host_directory_path){
    String user_dir = OS::get_singleton()->get_user_data_dir();
    //this->host_directory = DirAccess::open(user_dir);
    if(new_host_directory_path.begins_with(user_dir)){
        Ref<DirAccess> test_dir = DirAccess::open(new_host_directory_path);
        if(test_dir.is_valid()){
            this->host_directory_path = new_host_directory_path;
            this->host_directory = test_dir;
        }
    }
}