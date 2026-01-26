/**
 * Configures the target rootfs.
 *
 * Applies OS identity, Plymouth splash, GRUB silent boot settings,
 * TTY policy, and XDM configuration.
 */

#include "all.h"

/**
 * Configures GRUB for silent boot on the installed system.
 *
 * Writes a drop-in config to /etc/default/grub.d/ with settings
 * for hidden menu, zero timeout, and quiet kernel boot.
 */
static int configure_grub(const char *rootfs_path)
{
    LOG_INFO("Configuring GRUB for silent boot...");

    // Use drop-in directory to override GRUB settings without replacing
    // the package-managed /etc/default/grub file.
    char dir_path[COMMAND_PATH_MAX_LENGTH];
    snprintf(dir_path, sizeof(dir_path), "%s/etc/default/grub.d", rootfs_path);
    if (mkdir_p(dir_path) != 0)
    {
        LOG_ERROR("Failed to create /etc/default/grub.d");
        return -1;
    }

    // Write the GRUB config file.
    char file_path[COMMAND_PATH_MAX_LENGTH];
    const char *content =
        "GRUB_DISTRIBUTOR=\"" CONFIG_OS_NAME "\"\n"
        "GRUB_TIMEOUT=0\n"
        "GRUB_TIMEOUT_STYLE=hidden\n"
        "GRUB_RECORDFAIL_TIMEOUT=0\n"
        "GRUB_GFXMODE=auto\n"
        "GRUB_GFXPAYLOAD_LINUX=keep\n"
        "GRUB_CMDLINE_LINUX_DEFAULT=\"" CONFIG_TARGET_KERNEL_PARAMS "\"\n";
    snprintf(file_path, sizeof(file_path), "%s/distributor.cfg", dir_path);
    if (write_file(file_path, content) != 0)
    {
        LOG_ERROR("Failed to write GRUB config");
        return -2;
    }

    LOG_INFO("GRUB configured successfully");

    return 0;
}

/**
 * Enforces the LimeOS TTY policy.
 *
 * The policy requires that tty1 is graphical-only and tty2 is diagnostics-only.
 * This function masks getty on tty1 and clears issue files to prevent any text
 * from appearing on the graphical terminal.
 */
static int configure_tty_policy(const char *rootfs_path)
{
    char path[COMMAND_PATH_MAX_LENGTH];

    LOG_INFO("Enforcing TTY policy...");

    // Create the systemd system directory if it doesn't exist.
    snprintf(path, sizeof(path), "%s/etc/systemd/system", rootfs_path);
    if (mkdir_p(path) != 0)
    {
        LOG_ERROR("Failed to create /etc/systemd/system");
        return -1;
    }

    // Mask getty@tty1.service to prevent any login prompt on tty1.
    // This is the primary enforcement of the "tty1 is graphical-only" rule.
    snprintf(path, sizeof(path), "%s/etc/systemd/system/getty@tty1.service", rootfs_path);
    if (symlink_file("/dev/null", path) != 0)
    {
        LOG_ERROR("Failed to mask getty@tty1.service");
        return -2;
    }

    // Clear /etc/issue as a safety measure.
    // If getty somehow runs on tty1, it won't display any text.
    snprintf(path, sizeof(path), "%s/etc/issue", rootfs_path);
    if (write_file(path, "") != 0)
    {
        LOG_ERROR("Failed to clear /etc/issue");
        return -3;
    }

    // Clear /etc/issue.net as well.
    snprintf(path, sizeof(path), "%s/etc/issue.net", rootfs_path);
    if (write_file(path, "") != 0)
    {
        LOG_ERROR("Failed to clear /etc/issue.net");
        return -4;
    }

    LOG_INFO("TTY policy enforced successfully");

    return 0;
}

/**
 * Configures XDM display manager appearance.
 *
 * Removes the default Debian logo, sets LimeOS branding, and configures
 * X to run on vt1 for graphical continuity from Plymouth splash to desktop.
 */
static int configure_xdm(const char *rootfs_path)
{
    char path[COMMAND_PATH_MAX_LENGTH];

    LOG_INFO("Configuring XDM appearance...");

    // Create the XDM directory if it doesn't exist.
    snprintf(path, sizeof(path), "%s/etc/X11/xdm", rootfs_path);
    if (mkdir_p(path) != 0)
    {
        LOG_ERROR("Failed to create /etc/X11/xdm directory");
        return -1;
    }

    // Configure XDM: minimal changes, keep defaults for styling.
    snprintf(path, sizeof(path), "%s/etc/X11/xdm/Xresources", rootfs_path);
    const char *xresources =
        "! XDM configuration for " CONFIG_OS_NAME "\n"
        "\n"
        "! Remove logo, set greeting\n"
        "xlogin*logoFileName:\n"
        "xlogin*greeting: " CONFIG_OS_NAME " (Temporary DM / Greeter)\n";
    if (write_file(path, xresources) != 0)
    {
        LOG_ERROR("Failed to configure XDM Xresources");
        return -2;
    }

    // Configure XDM to run X on vt1 instead of the default vt7.
    // This provides graphical continuity from Plymouth splash to desktop.
    snprintf(path, sizeof(path), "%s/etc/X11/xdm/Xservers", rootfs_path);
    if (write_file(path, ":0 local /usr/bin/X :0 vt1\n") != 0)
    {
        LOG_ERROR("Failed to configure XDM Xservers");
        return -3;
    }

    LOG_INFO("XDM configured successfully");

    return 0;
}

int configure_target_rootfs(const char *path, const char *version)
{
    LOG_INFO("Configuring target rootfs...");

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

    // Configure GRUB for silent boot.
    if (configure_grub(path) != 0)
    {
        return -3;
    }

    // Enforce TTY policy (tty1 graphical, tty2 diagnostics).
    // Must run after write_os_identity to override /etc/issue files.
    if (configure_tty_policy(path) != 0)
    {
        return -4;
    }

    // Configure XDM appearance (temporary display manager).
    if (configure_xdm(path) != 0)
    {
        return -5;
    }

    LOG_INFO("Target rootfs configured successfully");

    return 0;
}
