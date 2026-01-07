#pragma once

// ---
// Build Configuration
// ---

/** The base directory for all build files. */
#define CONFIG_BUILD_DIRECTORY "/tmp/limeos-build"

/** The directory where component binaries are downloaded. */
#define CONFIG_COMPONENTS_DIRECTORY CONFIG_BUILD_DIRECTORY "/components"

/** The directory where the root filesystem is created. */
#define CONFIG_ROOTFS_DIRECTORY CONFIG_BUILD_DIRECTORY "/rootfs"

/** The path to the splash logo image. */
#define CONFIG_SPLASH_LOGO_PATH "./assets/splash.png"

// ---
// Github Configuration
// ---

/** The GitHub organization hosting LimeOS component repositories. */
#define CONFIG_GITHUB_ORG "limeos-org"

/** The HTTP User-Agent string for API requests. */
#define CONFIG_USER_AGENT "limeos-iso-builder/1.0"

/** The GitHub API base URL for releases. */
#define CONFIG_GITHUB_API_BASE "https://api.github.com/repos"

// ---
// Boot Configuration
// ---

/** The kernel command line parameters for boot. */
#define CONFIG_BOOT_KERNEL_PARAMS "boot=live quiet splash loglevel=0"

/** The default kernel image path within the ISO. */
#define CONFIG_BOOT_KERNEL_PATH "/boot/vmlinuz"

/** The default initrd image path within the ISO. */
#define CONFIG_BOOT_INITRD_PATH "/boot/initrd.img"

/**
 * The size of the EFI boot image in megabytes.
 *
 * 4MB provides sufficient space for the GRUB EFI binary (~1MB) plus
 * headroom for FAT filesystem overhead and future additions.
 */
#define CONFIG_EFI_IMAGE_SIZE_MB 4

// ---
// System Paths (Host Dependencies)
// ---

/** The path to isolinux.bin on the host system. */
#define CONFIG_ISOLINUX_BIN_PATH "/usr/lib/ISOLINUX/isolinux.bin"

/** The path to the MBR template for hybrid ISO. */
#define CONFIG_ISOLINUX_MBR_PATH "/usr/lib/ISOLINUX/isohdpfx.bin"

/** The path to ldlinux.c32 on the host system. */
#define CONFIG_LDLINUX_PATH "/usr/lib/syslinux/modules/bios/ldlinux.c32"

/** The path to the GRUB EFI binary on the host system. */
#define CONFIG_GRUB_EFI_PATH "/usr/lib/grub/x86_64-efi/monolithic/grubx64.efi"

// ---
// Plymouth Configuration
// ---

/** The Plymouth theme name. */
#define CONFIG_PLYMOUTH_THEME_NAME "limeos"

/** The Plymouth themes directory path (relative to rootfs). */
#define CONFIG_PLYMOUTH_THEMES_DIR "/usr/share/plymouth/themes"

// ---
// Root Filesystem Configuration
// ---

/** The Debian release to use for the base rootfs. */
#define CONFIG_DEBIAN_RELEASE "bookworm"

/** The installation path for component binaries (relative to rootfs). */
#define CONFIG_INSTALL_BIN_PATH "/usr/local/bin"

// ---
// Component Configuration
// ---

/** Component definition with repository and binary names. */
typedef struct {
    const char *repo_name;    // GitHub repository name
    const char *binary_name;  // Installed binary name
} Component;

/** Required LimeOS components. */
static const Component CONFIG_REQUIRED_COMPONENTS[] = {
    { "installation-wizard", "limeos-installation-wizard" }
};

/** The number of required components. */
#define CONFIG_REQUIRED_COMPONENTS_COUNT \
    (int)(sizeof(CONFIG_REQUIRED_COMPONENTS) / sizeof(CONFIG_REQUIRED_COMPONENTS[0]))

/** Optional LimeOS components. */
static const Component CONFIG_OPTIONAL_COMPONENTS[] = {
    { "window-manager", "limeos-window-manager" },
    { "display-manager", "limeos-display-manager" }
};

/** The number of optional components. */
#define CONFIG_OPTIONAL_COMPONENTS_COUNT \
    (int)(sizeof(CONFIG_OPTIONAL_COMPONENTS) / sizeof(CONFIG_OPTIONAL_COMPONENTS[0]))

// ---
// Service Configuration
// ---

/** The systemd service name for the installer. */
#define CONFIG_INSTALLER_SERVICE_NAME "limeos-installation-wizard"
