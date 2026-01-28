#pragma once
#include "../all.h"

/** Required files that must exist on the host system. */
extern const char *const REQUIRED_FILES[];
/** Number of entries in REQUIRED_FILES. */
extern const int REQUIRED_FILES_COUNT;

/** Required commands that must be available in PATH. */
extern const char *const REQUIRED_COMMANDS[];
/** Number of entries in REQUIRED_COMMANDS. */
extern const int REQUIRED_COMMANDS_COUNT;

/**
 * Validates that all required dependencies are available.
 *
 * Checks for required files (assets) and required commands
 * (debootstrap, mksquashfs, grub-mkrescue, etc.) before starting
 * the build process.
 *
 * @return - `0` - All dependencies are satisfied.
 * @return - `-1` - Missing required file(s).
 * @return - `-2` - Missing required command(s).
 * @return - `-3` - Missing both files and commands.
 */
int validate_dependencies(void);

