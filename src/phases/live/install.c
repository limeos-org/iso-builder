/**
 * This code is responsible for installing LimeOS components into the
 * live rootfs.
 */

#include "all.h"

int install_live_components(const char *rootfs_path, const char *components_path)
{
    char src_path[COMMON_MAX_PATH_LENGTH];
    char dst_path[COMMON_MAX_PATH_LENGTH];
    char bin_dir[COMMON_MAX_PATH_LENGTH];

    LOG_INFO("Installing components into live rootfs...");

    // Create the target directory for binaries.
    snprintf(bin_dir, sizeof(bin_dir), "%s" CONFIG_INSTALL_BIN_PATH, rootfs_path);
    if (mkdir_p(bin_dir) != 0)
    {
        return -1;
    }

    // Install required components.
    for (int i = 0; i < CONFIG_REQUIRED_COMPONENTS_COUNT; i++)
    {
        const Component *component = &CONFIG_REQUIRED_COMPONENTS[i];

        // Copy the component binary.
        snprintf(src_path, sizeof(src_path), "%s/%s", components_path, component->repo_name);
        snprintf(dst_path, sizeof(dst_path), "%s/%s", bin_dir, component->binary_name);

        if (copy_file(src_path, dst_path) != 0)
        {
            LOG_ERROR("Failed to install required component: %s", component->repo_name);
            return -2;
        }

        // Make the binary executable.
        if (chmod_file("+x", dst_path) != 0)
        {
            LOG_ERROR("Failed to make component executable: %s", component->binary_name);
            return -3;
        }

        LOG_INFO("Installed %s", component->binary_name);
    }

    // Install optional components if they exist.
    for (int i = 0; i < CONFIG_OPTIONAL_COMPONENTS_COUNT; i++)
    {
        const Component *component = &CONFIG_OPTIONAL_COMPONENTS[i];

        // Check if the optional component binary exists.
        snprintf(src_path, sizeof(src_path), "%s/%s", components_path, component->repo_name);
        if (!file_exists(src_path))
        {
            LOG_INFO("Skipping optional component: %s", component->repo_name);
            continue;
        }

        // Copy the optional component binary.
        snprintf(dst_path, sizeof(dst_path), "%s/%s", bin_dir, component->binary_name);
        if (copy_file(src_path, dst_path) != 0)
        {
            LOG_WARNING("Failed to install optional component: %s", component->repo_name);
            continue;
        }

        // Make the binary executable.
        if (chmod_file("+x", dst_path) != 0)
        {
            LOG_WARNING("Failed to make optional component executable: %s", component->binary_name);
            continue;
        }

        LOG_INFO("Installed %s", component->binary_name);
    }

    LOG_INFO("All required components installed successfully");

    return 0;
}
