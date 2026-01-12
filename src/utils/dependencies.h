#pragma once
#include "../all.h"

/**
 * Validates that all required dependencies are available.
 *
 * Checks for required files (isolinux, syslinux modules, assets) and
 * required commands (debootstrap, mksquashfs, xorriso, etc.) before
 * starting the build process.
 *
 * @return - `0` - All dependencies are satisfied.
 * @return - `1` - One or more dependencies are missing.
 */
int validate_dependencies(void);

/**
 * Checks if a command is available in PATH.
 *
 * @param name The command name (e.g., "debootstrap").
 *
 * @return - `1` - The command is available.
 * @return - `0` - The command is not available.
 */
int is_command_available(const char *name);
