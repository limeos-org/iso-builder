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
    if (write_os_identity(path, version) != 0)
    {
        return -1;
    }

    // Configure Plymouth splash.
    if (configure_plymouth(path, CONFIG_SPLASH_LOGO_PATH) != 0)
    {
        return -2;
    }

    LOG_INFO("Live rootfs configured successfully");

    return 0;
}
