#ifndef _GDE_V_ABI_HART_H_
#define _GDE_V_ABI_HART_H_

#include "gde-v/hart.h"
#include "gde-v/utils.h"

// File from the newlib libgloss repository, riscv folder.
#include <machine/syscall.h>

namespace godot{

/**
 * Extension class for the RVHart. This breaks out the EUECALL, ESECALL, and EMECALL
 * into separate signals for what the syscall is trying to achieve. Conforms to the newlib
 * RISV-V syscall implementation. 
 */
class ABIHart : public RVHart{
    GDCLASS(ABIHart, RVHart)

protected:
    // Standard Godot binding method to let it know about our class.
    static void _bind_methods();

    // Override the parent exception handler
    virtual bool _handle_exception(RVExceptions exception) override;

    // Emit a syscall of the given number. Uses newlib RISC-V numbers.
    bool emit_syscall_signal(int64_t syscall_number);
public:
    ABIHart(){}
    ~ABIHart(){}

    // Mappings for all the syscall numbers so it's easier to use.
    enum SyscallMappings{
        SYS_ENUM_WRAP(getcwd),
        SYS_ENUM_WRAP(dup),
        SYS_ENUM_WRAP(fcntl),
        SYS_ENUM_WRAP(faccessat),
        SYS_ENUM_WRAP(chdir),
        SYS_ENUM_WRAP(openat),
        SYS_ENUM_WRAP(close),
        SYS_ENUM_WRAP(getdents),
        SYS_ENUM_WRAP(lseek),
        SYS_ENUM_WRAP(read),
        SYS_ENUM_WRAP(write),
        SYS_ENUM_WRAP(writev),
        SYS_ENUM_WRAP(pread),
        SYS_ENUM_WRAP(pwrite),
        SYS_ENUM_WRAP(fstatat),
        SYS_ENUM_WRAP(fstat),
        SYS_ENUM_WRAP(exit),
        SYS_ENUM_WRAP(exit_group),
        SYS_ENUM_WRAP(kill),
        SYS_ENUM_WRAP(rt_sigaction),
        SYS_ENUM_WRAP(times),
        SYS_ENUM_WRAP(uname),
        SYS_ENUM_WRAP(gettimeofday),
        SYS_ENUM_WRAP(getpid),
        SYS_ENUM_WRAP(getuid),
        SYS_ENUM_WRAP(geteuid),
        SYS_ENUM_WRAP(getgid),
        SYS_ENUM_WRAP(getegid),
        SYS_ENUM_WRAP(brk),
        SYS_ENUM_WRAP(munmap),
        SYS_ENUM_WRAP(mremap),
        SYS_ENUM_WRAP(mmap),
        SYS_ENUM_WRAP(clock_gettime64),
        SYS_ENUM_WRAP(open),
        SYS_ENUM_WRAP(link),
        SYS_ENUM_WRAP(unlink),
        SYS_ENUM_WRAP(mkdir),
        SYS_ENUM_WRAP(access),
        SYS_ENUM_WRAP(stat),
        SYS_ENUM_WRAP(lstat),
        SYS_ENUM_WRAP(time),
        SYS_ENUM_WRAP(getmainvars)
    };
};

}

VARIANT_ENUM_CAST(ABIHart::SyscallMappings);

#endif