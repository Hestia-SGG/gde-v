#!/usr/bin/env python
import os
import sys

env = SConscript("extern/godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["include/","extern/rv/tools/linux/","extern/rv/","extern/newlib/libgloss/riscv/"])
sources = Glob("src/*.cpp")
sources = sources + Glob("src/gde-v/*.cpp")
sources = sources + Glob("extern/rv/rv.c")

if env["platform"] == "windows":
    env.Prepend(LINKFLAGS = ['/OPT:NOICF'])

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "bin/libgde-v.{}.{}.framework/libgde-v.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
elif env["platform"] == "ios":
    if env["ios_simulator"]:
        library = env.StaticLibrary(
            "bin/libgde-v.{}.{}.simulator.a".format(env["platform"], env["target"]),
            source=sources,
        )
    else:
        library = env.StaticLibrary(
            "bin/libgde-v.{}.{}.a".format(env["platform"], env["target"]),
            source=sources,
        )
else:
    library = env.SharedLibrary(
        "bin/libgde-v{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
