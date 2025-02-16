#include <gde-virtfs/host_filesys.h>

using namespace godot;

/*
class HostFileSystem : public VirtFileSystem{
    GDCLASS(HostFileSystem, VirtFileSystem)

protected:
    static void _bind_methods();

    virtual Ref<VirtFile> _find_file(String filename) override;
    virtual Ref<VirtFile> _create_file(String filename, FileMode new_file_mode) override;
    virtual VirtFSResult _delete_file(String filename) override;

    virtual Ref<VirtFile> _create_file_from_ref(Ref<VirtFile> new_file, String new_filename, bool duplicate = false) override;

    // The directory that's treated as root
    Ref<DirAccess> host_directory;
public:
    HostFileSystem();
    ~HostFileSystem();

    GET_NOSET(host_directory, Ref<DirAccess>);
    VirtFSResult set_host_directory(Ref<DirAccess> new_host_directory);
};*/

