/**
 * This code is responsible for packaging the target rootfs into a tarball.
 */

#include "all.h"

int package_target_rootfs(const char *rootfs_path, const char *output_path)
{
    LOG_INFO("Packaging target rootfs to %s", output_path);

    // Quote the rootfs path for shell safety.
    char quoted_rootfs[COMMON_MAX_QUOTED_LENGTH];
    if (common.shell_escape_path(rootfs_path, quoted_rootfs, sizeof(quoted_rootfs)) != 0)
    {
        LOG_ERROR("Failed to quote rootfs path");
        return -1;
    }

    // Quote the output path for shell safety.
    char quoted_output[COMMON_MAX_QUOTED_LENGTH];
    if (common.shell_escape_path(output_path, quoted_output, sizeof(quoted_output)) != 0)
    {
        LOG_ERROR("Failed to quote output path");
        return -2;
    }

    // Create a compressed tarball of the rootfs.
    // Use --numeric-owner to preserve UIDs/GIDs without mapping to names.
    // Use -C to change to the rootfs directory so paths are relative.
    char command[COMMON_MAX_COMMAND_LENGTH];
    snprintf(
        command, sizeof(command),
        "tar --numeric-owner -czf %s -C %s .",
        quoted_output, quoted_rootfs
    );
    if (common.run_command_indented(command) != 0)
    {
        LOG_ERROR("Failed to create rootfs tarball");
        return -3;
    }

    LOG_INFO("Target rootfs packaged successfully");

    return 0;
}
