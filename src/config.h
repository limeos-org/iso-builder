#pragma once

/**
 * This file consolidates all project-wide configuration constants.
 *
 * Constants are organized by category for easy navigation.
 */

/* ============================================================================
 * Buffer Size Limits
 * ============================================================================ */

/** The maximum length for constructed shell command strings. */
#define CONFIG_MAX_COMMAND_LENGTH 512

/** The maximum length for file path strings. */
#define CONFIG_MAX_PATH_LENGTH 256

/** The maximum length for URL strings. */
#define CONFIG_MAX_URL_LENGTH 512

/** The maximum length for version strings. */
#define CONFIG_VERSION_MAX_LENGTH 32

/**
 * The initial buffer size for API response data.
 *
 * 8KB is sufficient for typical GitHub releases API responses containing
 * 10-20 releases. The buffer grows dynamically if needed.
 */
#define CONFIG_API_BUFFER_SIZE 8192

/* ============================================================================
 * GitHub Configuration
 * ============================================================================ */

/** The GitHub organization hosting LimeOS component repositories. */
#define CONFIG_GITHUB_ORG "limeos-org"

/** The HTTP User-Agent string for API requests. */
#define CONFIG_USER_AGENT "limeos-iso-builder/1.0"

/** The GitHub API base URL for releases. */
#define CONFIG_GITHUB_API_BASE "https://api.github.com/repos"

/* ============================================================================
 * Boot Configuration
 * ============================================================================ */

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

/* ============================================================================
 * System Paths (Host Dependencies)
 * ============================================================================ */

/** The path to isolinux.bin on the host system. */
#define CONFIG_ISOLINUX_BIN_PATH "/usr/lib/ISOLINUX/isolinux.bin"

/** The path to the MBR template for hybrid ISO. */
#define CONFIG_ISOLINUX_MBR_PATH "/usr/lib/ISOLINUX/isohdpfx.bin"

/** The path to ldlinux.c32 on the host system. */
#define CONFIG_LDLINUX_PATH "/usr/lib/syslinux/modules/bios/ldlinux.c32"

/** The path to the GRUB EFI binary on the host system. */
#define CONFIG_GRUB_EFI_PATH "/usr/lib/grub/x86_64-efi/monolithic/grubx64.efi"

/* ============================================================================
 * Plymouth/Splash Configuration
 * ============================================================================ */

/** The Plymouth theme name. */
#define CONFIG_PLYMOUTH_THEME_NAME "limeos"

/** The Plymouth themes directory path (relative to rootfs). */
#define CONFIG_PLYMOUTH_THEMES_DIR "/usr/share/plymouth/themes"

/* ============================================================================
 * Rootfs Configuration
 * ============================================================================ */

/** The Debian release to use for the base rootfs. */
#define CONFIG_DEBIAN_RELEASE "bookworm"

/** The installation path for component binaries (relative to rootfs). */
#define CONFIG_INSTALL_BIN_PATH "/usr/local/bin"

/* ============================================================================
 * Service Configuration
 * ============================================================================ */

/** The systemd service name for the installer. */
#define CONFIG_INSTALLER_SERVICE_NAME "limeos-installer"

/** The installer binary name. */
#define CONFIG_INSTALLER_BINARY_NAME "installation-wizard"
