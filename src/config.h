#pragma once

// ---
// Build Configuration
// ---

/** The path to the splash logo image. */
#define CONFIG_SPLASH_LOGO_PATH "./assets/splash.png"

/** The path to the black background image for clean boot. */
#define CONFIG_BLACK_PNG_PATH "./assets/black.png"

/**
 * The prefix for output ISO filenames.
 *
 * Example: "limeos" produces "limeos-1.0.0.iso".
 */
#define CONFIG_ISO_FILENAME_PREFIX "limeos"

/** The directory to search for local component binaries before downloading. */
#define CONFIG_LOCAL_BIN_DIR "./bin"

/** The prefix for temporary build directories. */
#define CONFIG_TMPDIR_PREFIX "/tmp/limeos-build-"

// ---
// Github Configuration
// ---

/** The GitHub organization hosting LimeOS component repositories. */
#define CONFIG_GITHUB_ORG "limeos-org"

/** The HTTP User-Agent string for API requests. */
#define CONFIG_USER_AGENT "limeos-iso-builder/1.0"

/** The GitHub API base URL for releases. */
#define CONFIG_GITHUB_API_BASE "https://api.github.com/repos"

/** The GitHub API version for request headers. */
#define CONFIG_GITHUB_API_VERSION "2022-11-28"

/** The filename for release checksums. */
#define CONFIG_CHECKSUMS_FILENAME "SHA256SUMS"

// ---
// Boot Configuration
// ---

/** The kernel command line parameters for boot. */
#define CONFIG_BOOT_KERNEL_PARAMS "boot=live quiet splash loglevel=0"

/** The default kernel image path within the ISO. */
#define CONFIG_BOOT_KERNEL_PATH "/boot/vmlinuz"

/** The default initrd image path within the ISO. */
#define CONFIG_BOOT_INITRD_PATH "/boot/initrd.img"

/** The GRUB menu entry name displayed during boot. */
#define CONFIG_GRUB_MENU_ENTRY_NAME "LimeOS Installer"

// ---
// System Paths (Host Dependencies)
// ---

/** The path to isolinux.bin on the host system. */
#define CONFIG_ISOLINUX_BIN_PATH "/usr/lib/ISOLINUX/isolinux.bin"

/** The path to the MBR template for hybrid ISO. */
#define CONFIG_ISOLINUX_MBR_PATH "/usr/lib/ISOLINUX/isohdpfx.bin"

/** The path to ldlinux.c32 on the host system. */
#define CONFIG_LDLINUX_PATH "/usr/lib/syslinux/modules/bios/ldlinux.c32"

/** The path to vesamenu.c32 on the host system. */
#define CONFIG_VESAMENU_C32_PATH "/usr/lib/syslinux/modules/bios/vesamenu.c32"

/** The path to libutil.c32 on the host system. */
#define CONFIG_LIBUTIL_C32_PATH "/usr/lib/syslinux/modules/bios/libutil.c32"

/** The path to libcom32.c32 on the host system. */
#define CONFIG_LIBCOM32_C32_PATH "/usr/lib/syslinux/modules/bios/libcom32.c32"

/** The path to the GRUB EFI binary on the host system. */
#define CONFIG_GRUB_EFI_PATH "/usr/lib/grub/x86_64-efi/monolithic/grubx64.efi"

// ---
// Plymouth Configuration
// ---

/** The Plymouth theme name. */
#define CONFIG_PLYMOUTH_THEME_NAME "limeos"

/** The Plymouth theme display name shown in theme metadata. */
#define CONFIG_PLYMOUTH_DISPLAY_NAME "LimeOS"

/** The Plymouth theme description shown in theme metadata. */
#define CONFIG_PLYMOUTH_DESCRIPTION "LimeOS boot splash"

/** The Plymouth themes directory path (relative to rootfs). */
#define CONFIG_PLYMOUTH_THEMES_DIR "/usr/share/plymouth/themes"

// ---
// OS Branding Configuration
// ---

/** The display name of the operating system. */
#define CONFIG_OS_NAME "LimeOS"

/** The lowercase identifier for the operating system. */
#define CONFIG_OS_ID "limeos"

/** The OS home page URL. */
#define CONFIG_OS_HOME_URL "https://limeos.org"

/** The ID of the upstream distribution this OS is based on. */
#define CONFIG_OS_BASE_ID "debian"

// ---
// Root Filesystem Configuration
// ---

/** The Debian release to use for the base rootfs. */
#define CONFIG_DEBIAN_RELEASE "bookworm"

/** The installation path for component binaries (relative to rootfs). */
#define CONFIG_INSTALL_BIN_PATH "/usr/local/bin"

/** The path where the payload tarball is stored in the carrier rootfs. */
#define CONFIG_PAYLOAD_ROOTFS_PATH "/usr/share/limeos/rootfs.tar.gz"

/**
 * The directory where bundled .deb packages are stored in the carrier rootfs.
 */
#define CONFIG_PACKAGES_DIR "/usr/share/limeos/packages"

/** The directory for BIOS-specific bootloader packages. */
#define CONFIG_PACKAGES_BIOS_DIR CONFIG_PACKAGES_DIR "/bios"

/** The directory for EFI-specific bootloader packages. */
#define CONFIG_PACKAGES_EFI_DIR CONFIG_PACKAGES_DIR "/efi"

/**
 * Packages for the carrier rootfs (boots from ISO, runs installer).
 * Minimal environment to run the installation wizard.
 */
#define CONFIG_CARRIER_PACKAGES \
    "linux-image-amd64 systemd-sysv live-boot " \
    "plymouth plymouth-themes " \
    "libncurses6 parted dosfstools e2fsprogs"

/**
 * Packages for the payload rootfs (installed to disk).
 * Full system with networking and user tools.
 *
 * Includes common grub dependencies that don't conflict between BIOS/EFI.
 * The boot-mode-specific packages are bundled separately.
 */
#define CONFIG_PAYLOAD_PACKAGES \
    "linux-image-amd64 systemd-sysv dbus " \
    "libpam-systemd policykit-1 " \
    "locales console-setup keyboard-configuration " \
    "sudo network-manager " \
    "grub2-common grub-common ucf sensible-utils " \
    "libefiboot1 libefivar1 libfuse3-3 os-prober"

/**
 * BIOS-specific bootloader packages to bundle.
 * These conflict with EFI packages and must be installed by the installer.
 */
#define CONFIG_BIOS_PACKAGES "grub-pc grub-pc-bin"

/**
 * EFI-specific bootloader packages to bundle.
 * These conflict with BIOS packages and must be installed by the installer.
 */
#define CONFIG_EFI_PACKAGES "grub-efi-amd64 grub-efi-amd64-bin"

// ---
// Component Configuration
// ---

/**
 * A type representing a LimeOS component with its repository and binary names.
 */
typedef struct {
    const char *repo_name;
    const char *binary_name;
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
