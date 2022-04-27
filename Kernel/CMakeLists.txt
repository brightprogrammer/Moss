# add all Kernel sources here
# recurisve search may be slower sometimes so just hardcode filenames
set(KERNEL_SRCS "KernelEntry.cpp" "Renderer.cpp" "String.cpp")

# make Kernel as executable
add_executable(Kernel ${KERNEL_SRCS})

# set compile options
target_compile_options(Kernel PRIVATE   -Wall -Wextra -O0 -g
                                        -ffreestanding
                                        -fno-stack-protector
                                        -fno-pic -fpie
                                        -mabi=sysv
                                        -mno-80387
                                        -mno-mmx
                                        -mno-3dnow
                                        -mgeneral-regs-only
                                        -mno-sse
                                        -mno-sse2
                                        -mno-avx
                                        -fno-exceptions
                                        -mno-red-zone)
# set linker options
target_link_options(Kernel PRIVATE  -fno-pic -fpie
                                    # this must be a comma separated list
                                    -Wl,-static,-pie,-ztext--no-dynamic-linker
                                    -static-pie
                                    -nostdlib
                                    -T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
                                    -z max-page-size=0x1000)


# set_target_properties(Kernel PROPERTIES PUBLIC_HEADER "stivale2.h")

# add install rules for this include dir
# install(TARGETS Kernel PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/Kernel)
