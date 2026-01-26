#pragma once

// ---
// Build Configuration
// ---

/** The path to the splash logo image. */
#define CONFIG_SPLASH_LOGO_PATH "./assets/splash.png"

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

/**
 * Kernel command line parameters for live boot (ISO).
 *
 * TTY policy: tty1 is graphical-only, tty2 is diagnostics-only.
 * Kernel and systemd output is suppressed on tty1 via quiet/loglevel.
 */
#define CONFIG_LIVE_KERNEL_PARAMS \
    "boot=live "                        /* Boot from live system (live-boot)*/ \
    "quiet "                            /* Minimize boot messages.          */ \
    "splash "                           /* Show Plymouth splash screen.     */ \
    "loglevel=0 "                       /* Suppress kernel log messages.    */ \
    "vt.global_cursor_default=0 "       /* Hide the blinking cursor.        */ \
    "systemd.show_status=false "        /* Hide systemd [OK]/[FAILED] logs. */ \
    "rd.systemd.show_status=false "     /* Hide initramfs systemd status.   */ \
    "rd.udev.log_level=3 "              /* Hide udev messages in initramfs. */ \
    "amdgpu.modeset=1 "                 /* Force amdgpu KMS early.          */ \
    "radeon.modeset=1 "                 /* Force radeon KMS early.          */ \
    "nouveau.modeset=1 "                /* Force nouveau KMS early.         */ \
    "i915.modeset=1"                    /* Force i915 KMS early.            */

/**
 * Kernel command line parameters for target system (installed).
 *
 * Same TTY policy as live, minus the live-boot specific param.
 */
#define CONFIG_TARGET_KERNEL_PARAMS \
    "quiet "                            /* Minimize boot messages.          */ \
    "splash "                           /* Show Plymouth splash screen.     */ \
    "loglevel=0 "                       /* Suppress kernel log messages.    */ \
    "vt.global_cursor_default=0 "       /* Hide the blinking cursor.        */ \
    "systemd.show_status=false "        /* Hide systemd [OK]/[FAILED] logs. */ \
    "rd.systemd.show_status=false "     /* Hide initramfs systemd status.   */ \
    "rd.udev.log_level=3 "              /* Hide udev messages in initramfs. */ \
    "amdgpu.modeset=1 "                 /* Force amdgpu KMS early.          */ \
    "radeon.modeset=1 "                 /* Force radeon KMS early.          */ \
    "nouveau.modeset=1 "                /* Force nouveau KMS early.         */ \
    "i915.modeset=1"                    /* Force i915 KMS early.            */

/** The default kernel image path within the ISO. */
#define CONFIG_BOOT_KERNEL_PATH "/boot/vmlinuz"

/** The default initrd image path within the ISO. */
#define CONFIG_BOOT_INITRD_PATH "/boot/initrd.img"

/** The GRUB menu entry name displayed during boot. */
#define CONFIG_GRUB_MENU_ENTRY_NAME "LimeOS Installer"

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

/** The path where the target tarball is stored in the live rootfs. */
#define CONFIG_TARGET_ROOTFS_PATH "/usr/share/limeos/rootfs.tar.gz"

/** The APT cache directory where bootloader packages are pre-populated. */
#define CONFIG_APT_CACHE_DIR "/var/cache/apt/archives"

/**
 * Packages for the live rootfs (boots from ISO, runs installer).
 * Minimal environment to run the installation wizard.
 */
#define CONFIG_LIVE_PACKAGES \
    "linux-image-amd64 "      /* Kernel                                     */ \
    "firmware-amd-graphics "  /* AMD GPU firmware (amdgpu, radeon)          */ \
    "firmware-misc-nonfree "  /* Intel/misc GPU firmware (i915, etc)        */ \
    "systemd-sysv "           /* Init system (SysV compat)                  */ \
    "live-boot "              /* Live system boot scripts                   */ \
    "plymouth "               /* Boot splash screen                         */ \
    "plymouth-themes "        /* Splash screen themes                       */ \
    "libncurses6 "            /* Terminal UI library (installer TUI)        */ \
    "parted "                 /* Disk partitioning (installer)              */ \
    "dosfstools "             /* FAT filesystem tools (EFI partition)       */ \
    "e2fsprogs"               /* ext4 filesystem tools (root partition)     */

/**
 * Packages for the target rootfs (installed to disk).
 *
 * GRUB bootloader packages (CONFIG_BIOS_PACKAGES / CONFIG_EFI_PACKAGES) are
 * excluded here because they conflict with each other. They are bundled in the
 * ISO's APT cache and installed at runtime based on detected boot mode.
 */
#define CONFIG_TARGET_PACKAGES \
    "linux-image-amd64 "       /* Kernel                                    */ \
    "firmware-amd-graphics "   /* AMD GPU firmware (amdgpu, radeon)         */ \
    "firmware-misc-nonfree "   /* Intel/misc GPU firmware (i915, etc)       */ \
    "systemd "                 /* Init system and service manager           */ \
    "systemd-sysv "            /* SysV init compatibility layer             */ \
    "dbus "                    /* IPC message bus                           */ \
    "libpam-systemd "          /* User session management                   */ \
    "plymouth "                /* Boot splash screen                        */ \
    "plymouth-themes "         /* Splash screen themes                      */ \
    "locales "                 /* Language and locale support               */ \
    "console-setup "           /* Console font and keymap setup             */ \
    "keyboard-configuration "  /* Keyboard layout configuration             */ \
    "sudo "                    /* Privilege escalation                      */ \
    "grub2-common "            /* Shared GRUB files (BIOS/EFI agnostic)     */ \
    "grub-common "             /* More shared GRUB files                    */ \
    "ucf "                     /* Config file update management (grub dep)  */ \
    "sensible-utils "          /* Default editor/browser (grub dep)         */ \
    "libefiboot1 "             /* EFI boot manager library (grub-efi dep)   */ \
    "libefivar1 "              /* EFI variable library (grub-efi dep)       */ \
    "xserver-xorg-core "       /* X server (WM dep)                         */ \
    "xserver-xorg-input-all "  /* Input drivers (WM dep)                    */ \
    "xserver-xorg-video-all "  /* Video drivers (WM dep)                    */ \
    "xinit "                   /* startx command (WM dep)                   */ \
    "xterm "                   /* Default terminal emulator (WM dep)        */ \
    "libx11-6 "                /* X11 client library (WM dep)               */ \
    "libxcomposite1 "          /* X Composite extension (WM dep)            */ \
    "libxi6 "                  /* X Input extension (WM dep)                */ \
    "libxrandr2 "              /* X RandR extension (WM dep)                */ \
    "libxfixes3 "              /* X Fixes extension (WM dep)                */ \
    "libcairo2 "               /* Cairo graphics library (WM dep)           */ \
    "xdm "                     /* Display manager (temporary)               */

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
