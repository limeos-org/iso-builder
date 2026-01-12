/**
 * This code is responsible for packaging the payload rootfs into a tarball.
 */

#include "all.h"

int package_payload_rootfs(const char *rootfs_path, const char *output_path)
{
    char command[COMMAND_MAX_LENGTH];
    char quoted_rootfs[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_output[COMMAND_QUOTED_MAX_LENGTH];

    LOG_INFO("Packaging payload rootfs to %s", output_path);

    // Quote paths to prevent shell injection.
    if (shell_quote_path(rootfs_path, quoted_rootfs, sizeof(quoted_rootfs)) != 0)
    {
        LOG_ERROR("Failed to quote rootfs path");
        return -1;
    }
    if (shell_quote_path(output_path, quoted_output, sizeof(quoted_output)) != 0)
    {
        LOG_ERROR("Failed to quote output path");
        return -1;
    }

    // Create a compressed tarball of the rootfs.
    // Use --numeric-owner to preserve UIDs/GIDs without mapping to names.
    // Use -C to change to the rootfs directory so paths are relative.
    snprintf(
        command, sizeof(command),
        "tar --numeric-owner -czf %s -C %s .",
        quoted_output, quoted_rootfs
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create rootfs tarball");
        return -1;
    }

    LOG_INFO("Payload rootfs packaged successfully");

    return 0;
}
