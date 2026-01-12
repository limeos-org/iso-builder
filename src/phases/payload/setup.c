/**
 * This code is responsible for system setup in the payload rootfs.
 *
 * Handles user creation and other initialization tasks that aren't
 * related to branding or package installation.
 */

#include "all.h"

int create_default_user(const char *rootfs_path)
{
    LOG_INFO("Creating default user...");

    // Create user with home directory and bash shell.
    if (run_chroot(rootfs_path, "useradd -m -s /bin/bash user") != 0)
    {
        LOG_ERROR("Failed to create default user");
        return -1;
    }

    // Set password.
    if (run_chroot(rootfs_path, "echo 'user:password' | chpasswd") != 0)
    {
        LOG_ERROR("Failed to set user password");
        return -1;
    }

    // Add to sudo group.
    if (run_chroot(rootfs_path, "usermod -aG sudo user") != 0)
    {
        LOG_ERROR("Failed to add user to sudo group");
        return -1;
    }

    LOG_INFO("Default user created");
    return 0;
}
