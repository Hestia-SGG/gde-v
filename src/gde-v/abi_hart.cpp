#include "gde-v/abi_hart.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

#define BIND_SYSCALL_STATIC(x) ADD_SIGNAL(MethodInfo("pending_" #x, PropertyInfo(Variant::OBJECT, "hart", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT, "ABIHart"))); BIND_ENUM_CONSTANT(x)

void ABIHart::_bind_methods(){
    BIND_SYSCALL_STATIC(getcwd)
    BIND_SYSCALL_STATIC(dup)
    BIND_SYSCALL_STATIC(fcntl)
    BIND_SYSCALL_STATIC(faccessat)
    BIND_SYSCALL_STATIC(chdir)
    BIND_SYSCALL_STATIC(openat)
    BIND_SYSCALL_STATIC(close)
    BIND_SYSCALL_STATIC(getdents)
    BIND_SYSCALL_STATIC(lseek)
    BIND_SYSCALL_STATIC(read)
    BIND_SYSCALL_STATIC(write)
    BIND_SYSCALL_STATIC(writev)
    BIND_SYSCALL_STATIC(pread)
    BIND_SYSCALL_STATIC(pwrite)
    BIND_SYSCALL_STATIC(fstatat)
    BIND_SYSCALL_STATIC(fstat)
    BIND_SYSCALL_STATIC(exit)
    BIND_SYSCALL_STATIC(exit_group)
    BIND_SYSCALL_STATIC(kill)
    BIND_SYSCALL_STATIC(rt_sigaction)
    BIND_SYSCALL_STATIC(times)
    BIND_SYSCALL_STATIC(uname)
    BIND_SYSCALL_STATIC(gettimeofday)
    BIND_SYSCALL_STATIC(getpid)
    BIND_SYSCALL_STATIC(getuid)
    BIND_SYSCALL_STATIC(geteuid)
    BIND_SYSCALL_STATIC(getgid)
    BIND_SYSCALL_STATIC(getegid)
    BIND_SYSCALL_STATIC(brk)
    BIND_SYSCALL_STATIC(munmap)
    BIND_SYSCALL_STATIC(mremap)
    BIND_SYSCALL_STATIC(mmap)
    BIND_SYSCALL_STATIC(clock_gettime64)
    BIND_SYSCALL_STATIC(open)
    BIND_SYSCALL_STATIC(link)
    BIND_SYSCALL_STATIC(unlink)
    BIND_SYSCALL_STATIC(mkdir)
    BIND_SYSCALL_STATIC(access)
    BIND_SYSCALL_STATIC(stat)
    BIND_SYSCALL_STATIC(lstat)
    BIND_SYSCALL_STATIC(time)
    BIND_SYSCALL_STATIC(getmainvars)
}

bool ABIHart::_handle_exception(RVExceptions exception){
    if(exception != RVExceptions::ETRAP_NONE){
		switch(exception){
			case EUECALL:
			case ESECALL:
			case EMECALL:
				//this->emit_signal("machine_system_call", this);
                //Register 17 is the one that maps to a7
                this->emit_syscall_signal(this->hart_state->get_register_by_num(17));
				break;
			default:
				break;
		}

		//Exception wasn't cleared, return whatever the parent does
		if(this->hart_state->get_pending_exception() != RVExceptions::ETRAP_NONE) return this->RVHart::_handle_exception(exception);
		else return true;
	}

    return true;
}

#define CASE_SYSCALL_TO_STR(x, y) case SyscallMappings::x : y = "pending_" #x; break;

bool ABIHart::emit_syscall_signal(int64_t syscall_number){
    String signal_to_call = "";
    switch(syscall_number){
        CASE_SYSCALL_TO_STR(getcwd, signal_to_call)
        CASE_SYSCALL_TO_STR(dup, signal_to_call)
        CASE_SYSCALL_TO_STR(fcntl, signal_to_call)
        CASE_SYSCALL_TO_STR(faccessat, signal_to_call)
        CASE_SYSCALL_TO_STR(chdir, signal_to_call)
        CASE_SYSCALL_TO_STR(openat, signal_to_call)
        CASE_SYSCALL_TO_STR(close, signal_to_call)
        CASE_SYSCALL_TO_STR(getdents, signal_to_call)
        CASE_SYSCALL_TO_STR(lseek, signal_to_call)
        CASE_SYSCALL_TO_STR(read, signal_to_call)
        CASE_SYSCALL_TO_STR(write, signal_to_call)
        CASE_SYSCALL_TO_STR(writev, signal_to_call)
        CASE_SYSCALL_TO_STR(pread, signal_to_call)
        CASE_SYSCALL_TO_STR(pwrite, signal_to_call)
        CASE_SYSCALL_TO_STR(fstatat, signal_to_call)
        CASE_SYSCALL_TO_STR(fstat, signal_to_call)
        CASE_SYSCALL_TO_STR(exit, signal_to_call)
        CASE_SYSCALL_TO_STR(exit_group, signal_to_call)
        CASE_SYSCALL_TO_STR(kill, signal_to_call)
        CASE_SYSCALL_TO_STR(rt_sigaction, signal_to_call)
        CASE_SYSCALL_TO_STR(times, signal_to_call)
        CASE_SYSCALL_TO_STR(uname, signal_to_call)
        CASE_SYSCALL_TO_STR(gettimeofday, signal_to_call)
        CASE_SYSCALL_TO_STR(getpid, signal_to_call)
        CASE_SYSCALL_TO_STR(getuid, signal_to_call)
        CASE_SYSCALL_TO_STR(geteuid, signal_to_call)
        CASE_SYSCALL_TO_STR(getgid, signal_to_call)
        CASE_SYSCALL_TO_STR(getegid, signal_to_call)
        CASE_SYSCALL_TO_STR(brk, signal_to_call)
        CASE_SYSCALL_TO_STR(munmap, signal_to_call)
        CASE_SYSCALL_TO_STR(mremap, signal_to_call)
        CASE_SYSCALL_TO_STR(mmap, signal_to_call)
        CASE_SYSCALL_TO_STR(clock_gettime64, signal_to_call)
        CASE_SYSCALL_TO_STR(open, signal_to_call)
        CASE_SYSCALL_TO_STR(link, signal_to_call)
        CASE_SYSCALL_TO_STR(unlink, signal_to_call)
        CASE_SYSCALL_TO_STR(mkdir, signal_to_call)
        CASE_SYSCALL_TO_STR(access, signal_to_call)
        CASE_SYSCALL_TO_STR(stat, signal_to_call)
        CASE_SYSCALL_TO_STR(lstat, signal_to_call)
        CASE_SYSCALL_TO_STR(time, signal_to_call)
        CASE_SYSCALL_TO_STR(getmainvars, signal_to_call)
        default:
            return false;
    }

    this->emit_signal(signal_to_call, this);
    return true;
}
