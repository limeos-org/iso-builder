/**
 * Configures the live rootfs.
 *
 * Applies OS identity and Plymouth splash for the live environment.
 */

#include "all.h"

int configure_live_rootfs(const char *path, const char *version)
{
    LOG_INFO("Configuring live rootfs...");

    // Write OS identity files.
    LOG_INFO("Writing OS identity files...");
    int identity_result = write_os_identity(path, version);
    if (identity_result != 0)
    {
        switch (identity_result)
        {
            case -1:
                LOG_ERROR("Failed to write /etc/os-release");
                break;
            case -2:
                LOG_ERROR("Failed to write /etc/issue");
                break;
            case -3:
                LOG_ERROR("Failed to write /etc/issue.net");
                break;
            case -4:
                LOG_ERROR("Failed to clear /etc/machine-id");
                break;
        }
        return -1;
    }

    // Configure Plymouth splash.
    LOG_INFO("Configuring Plymouth splash screen...");
    int plymouth_result = configure_plymouth(path, CONFIG_SPLASH_LOGO_PATH);
    if (plymouth_result != 0)
    {
        switch (plymouth_result)
        {
            case -1:
                LOG_ERROR("Splash logo not found: %s", CONFIG_SPLASH_LOGO_PATH);
                break;
            case -2:
                LOG_ERROR("Failed to create Plymouth theme directory");
                break;
            case -3:
                LOG_ERROR("Failed to copy splash logo");
                break;
            case -4:
                LOG_ERROR("Failed to write Plymouth theme config");
                break;
            case -5:
                LOG_ERROR("Failed to write Plymouth theme script");
                break;
        }
        return -2;
    }

    LOG_INFO("Live rootfs configured successfully");

    return 0;
}
