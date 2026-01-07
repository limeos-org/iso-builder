/**
 * This code is responsible for installing component binaries into the rootfs.
 */

#include "all.h"

int install_components(const char *rootfs_path, const char *components_path)
{
    char src_path[MAX_PATH_LENGTH];
    char dst_path[MAX_PATH_LENGTH];
    char bin_dir[MAX_PATH_LENGTH];

    LOG_INFO("Installing components into rootfs...");

    // Create the target directory for binaries.
    snprintf(bin_dir, sizeof(bin_dir), "%s" CONFIG_INSTALL_BIN_PATH, rootfs_path);
    if (mkdir_p(bin_dir) != 0)
    {
        return -1;
    }

    // Install required components.
    for (int i = 0; i < REQUIRED_COMPONENTS_COUNT; i++)
    {
        // Copy the required component binary.
        snprintf(src_path, sizeof(src_path), "%s/%s", components_path, REQUIRED_COMPONENTS[i]);
        snprintf(dst_path, sizeof(dst_path), "%s/%s", bin_dir, REQUIRED_COMPONENTS[i]);

        if (copy_file(src_path, dst_path) != 0)
        {
            LOG_ERROR("Failed to install required component: %s", REQUIRED_COMPONENTS[i]);
            return -1;
        }

        // Make the binary executable.
        chmod_file("+x", dst_path);

        LOG_INFO("Installed %s", REQUIRED_COMPONENTS[i]);
    }

    // Install optional components if they exist.
    for (int i = 0; i < OPTIONAL_COMPONENTS_COUNT; i++)
    {
        // Check if the optional component binary exists.
        snprintf(src_path, sizeof(src_path), "%s/%s", components_path, OPTIONAL_COMPONENTS[i]);
        if (!file_exists(src_path))
        {
            LOG_INFO("Skipping optional component: %s", OPTIONAL_COMPONENTS[i]);
            continue;
        }

        // Copy the optional component binary.
        snprintf(dst_path, sizeof(dst_path), "%s/%s", bin_dir, OPTIONAL_COMPONENTS[i]);
        if (copy_file(src_path, dst_path) != 0)
        {
            LOG_WARNING("Failed to install optional component: %s", OPTIONAL_COMPONENTS[i]);
            continue;
        }

        // Make the binary executable.
        chmod_file("+x", dst_path);

        LOG_INFO("Installed %s", OPTIONAL_COMPONENTS[i]);
    }

    LOG_INFO("All required components installed successfully");

    return 0;
}
