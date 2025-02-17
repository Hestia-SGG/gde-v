#include "register_types.h"

#include "gde-v.h"
#include "gde-virtfs/filesys.h"
#include "gde-virtfs/host_filesys.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

// Standard initialization function for Godot.
void initialize_gdev_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_VIRTUAL_CLASS(RVBusDevice);
    GDREGISTER_CLASS(RVMemoryDevice);
	GDREGISTER_CLASS(RVHeapDevice);
	GDREGISTER_CLASS(RVHartState);
	GDREGISTER_CLASS(RVHart);
	GDREGISTER_CLASS(ABIHart);
	GDREGISTER_CLASS(ElfHeader);
	GDREGISTER_CLASS(ElfProgramHeader);
	GDREGISTER_CLASS(ElfFile);

	GDREGISTER_VIRTUAL_CLASS(VirtFileHandle);
	GDREGISTER_VIRTUAL_CLASS(VirtFile);
	GDREGISTER_VIRTUAL_CLASS(VirtFileSystem);
	GDREGISTER_CLASS(VirtFSDispatcher);

	GDREGISTER_CLASS(HostFileHanlde);
	GDREGISTER_CLASS(HostFile);
	GDREGISTER_CLASS(HostFileSystem);
}

// Standard uninitialization function for Godot.
void uninitialize_gdev_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT gdev_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_gdev_module);
	init_obj.register_terminator(uninitialize_gdev_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
