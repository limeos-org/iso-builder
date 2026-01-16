/**
 * This code is responsible for aggressively stripping noncritical files
 * from the base rootfs to minimize size for both target and carrier.
 */

#include "all.h"

/**
 * Mapping of firmware directories to their kernel modules.
 *
 * When firmware is excluded via dpkg, the corresponding modules should be
 * blacklisted to prevent them from loading and failing to find firmware.
 */
struct FirmwareModuleEntry
{
    const char *firmware_dir;  // Directory relative to /usr/lib/firmware.
    const char *modules;       // Space-separated list of kernel modules.
};

/** Firmware directories and their corresponding kernel modules. */
static const struct FirmwareModuleEntry FIRMWARE_MODULES[] = {
    // Intel WiFi.
    {"iwlwifi", "iwlwifi iwlmvm iwldvm"},
    // Atheros WiFi.
    {"ath9k_htc", "ath9k ath9k_htc"},
    {"ath10k", "ath10k_pci ath10k_core"},
    {"ath11k", "ath11k ath11k_pci"},
    {"ath12k", "ath12k"},
    // Realtek WiFi.
    {"rtlwifi", "rtlwifi rtl8192ce rtl8192cu rtl8192de rtl8192se rtl8723ae rtl8723be"},
    {"rtw88", "rtw88_pci rtw88_core"},
    {"rtw89", "rtw89_pci rtw89_core"},
    // MediaTek WiFi.
    {"mediatek", "mt7601u mt7921e mt7921s"},
    // Marvell WiFi.
    {"mrvl", "mwifiex mwifiex_pcie mwifiex_sdio"},
    // Qualcomm Bluetooth.
    {"qca", "btqca"},
    // Realtek NICs (not needed).
    {"rtl_nic", "r8169"},
    // Server/datacenter NICs (not needed for desktop).
    {"cxgb4", "cxgb4"},
    {"liquidio", "liquidio"},
    {"mellanox", "mlx4_core mlx5_core"},
    {"netronome", "nfp"},
    {"dpaa2", "fsl_dpaa2_eth"},
    {"bnx2", "bnx2"},
    {"bnx2x", "bnx2x"},
    // Cirrus audio DSP.
    {"cirrus", "snd_hda_codec_cirrus"},
};

/** Bluetooth modules to blacklist (firmware from multiple sources). */
static const char *BLUETOOTH_MODULES[] = {
    "btusb", "btrtl", "btbcm", "btintel", "bluetooth",
};

/** Intel Sound Open Firmware modules to blacklist. */
static const char *INTEL_SOF_MODULES[] = {
    "snd_sof", "snd_sof_pci", "snd_sof_intel_hda_common",
};

static int write_dpkg_firmware_exclusions(const char *rootfs_path)
{
    char dir_path[COMMAND_PATH_MAX_LENGTH];
    char file_path[COMMAND_PATH_MAX_LENGTH];

    // Create dpkg config directory if it doesn't exist.
    snprintf(dir_path, sizeof(dir_path), "%s/etc/dpkg/dpkg.cfg.d", rootfs_path);
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to create /etc/dpkg/dpkg.cfg.d");
        return -1;
    }

    char content[8192];
    size_t pos = 0;
    int count;

    // Write header comment.
    pos += snprintf(
        content + pos, sizeof(content) - pos,
        "# Exclude noncritical firmware from package installation.\n"
        "# This prevents firmware from being installed in the first place,\n"
        "# rather than removing it after installation.\n\n"
    );

    // Exclude firmware directories (both modern and legacy paths).
    count = sizeof(FIRMWARE_MODULES) / sizeof(FIRMWARE_MODULES[0]);
    for (int i = 0; i < count; i++)
    {
        pos += snprintf(
            content + pos, sizeof(content) - pos,
            "path-exclude=/usr/lib/firmware/%s/*\n",
            FIRMWARE_MODULES[i].firmware_dir
        );
        pos += snprintf(
            content + pos, sizeof(content) - pos,
            "path-exclude=/lib/firmware/%s/*\n",
            FIRMWARE_MODULES[i].firmware_dir
        );
    }

    // Exclude Intel Bluetooth firmware (*bt* pattern in intel/).
    pos += snprintf(
        content + pos, sizeof(content) - pos,
        "\n# Intel Bluetooth firmware.\n"
        "path-exclude=/usr/lib/firmware/intel/*bt*\n"
        "path-exclude=/lib/firmware/intel/*bt*\n"
    );

    // Exclude Intel Sound Open Firmware.
    pos += snprintf(
        content + pos, sizeof(content) - pos,
        "\n# Intel Sound Open Firmware.\n"
        "path-exclude=/usr/lib/firmware/intel/sof/*\n"
        "path-exclude=/usr/lib/firmware/intel/sof-tplg/*\n"
        "path-exclude=/lib/firmware/intel/sof/*\n"
        "path-exclude=/lib/firmware/intel/sof-tplg/*\n"
    );

    // Exclude Broadcom Bluetooth firmware (.hcd files).
    pos += snprintf(
        content + pos, sizeof(content) - pos,
        "\n# Broadcom Bluetooth firmware.\n"
        "path-exclude=/usr/lib/firmware/brcm/*.hcd\n"
        "path-exclude=/lib/firmware/brcm/*.hcd\n"
    );

    // Exclude Broadcom WiFi firmware (pcie/sdio).
    pos += snprintf(
        content + pos, sizeof(content) - pos,
        "\n# Broadcom WiFi firmware.\n"
        "path-exclude=/usr/lib/firmware/brcm/*-pcie.*\n"
        "path-exclude=/usr/lib/firmware/brcm/*-sdio.*\n"
        "path-exclude=/lib/firmware/brcm/*-pcie.*\n"
        "path-exclude=/lib/firmware/brcm/*-sdio.*\n"
    );

    // Write the exclusion file.
    snprintf(file_path, sizeof(file_path), "%s/exclude-firmware", dir_path);
    if (write_file(file_path, content) != 0)
    {
        LOG_WARNING("Failed to write dpkg firmware exclusions");
        return -1;
    }

    return 0;
}

static int write_module_blacklist(const char *rootfs_path)
{
    char dir_path[COMMAND_PATH_MAX_LENGTH];
    char file_path[COMMAND_PATH_MAX_LENGTH];

    // Create modprobe.d directory if it doesn't exist.
    snprintf(dir_path, sizeof(dir_path), "%s/etc/modprobe.d", rootfs_path);
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to create /etc/modprobe.d");
        return -1;
    }

    char content[8192];
    size_t pos = 0;
    int count;

    // Write header comment.
    pos += snprintf(
        content + pos, sizeof(content) - pos,
        "# Modules blacklisted because their firmware is excluded.\n"
        "# See /etc/dpkg/dpkg.cfg.d/exclude-firmware for exclusions.\n\n"
    );

    // Blacklist modules from firmware mapping.
    count = sizeof(FIRMWARE_MODULES) / sizeof(FIRMWARE_MODULES[0]);
    for (int i = 0; i < count; i++)
    {
        // Parse space-separated modules.
        const char *modules = FIRMWARE_MODULES[i].modules;
        char module[64];
        int j = 0;

        while (*modules)
        {
            // Skip spaces.
            while (*modules == ' ')
            {
                modules++;
            }
            if (!*modules)
            {
                break;
            }

            // Copy module name.
            j = 0;
            while (*modules && *modules != ' ' && j < (int)sizeof(module) - 1)
            {
                module[j++] = *modules++;
            }
            module[j] = '\0';

            // Blacklist the module.
            if (j > 0)
            {
                pos += snprintf(
                    content + pos, sizeof(content) - pos,
                    "blacklist %s\n", module
                );
            }
        }
    }

    // Blacklist Bluetooth modules.
    pos += snprintf(
        content + pos, sizeof(content) - pos,
        "\n# Bluetooth modules (firmware excluded from multiple sources).\n"
    );
    count = sizeof(BLUETOOTH_MODULES) / sizeof(BLUETOOTH_MODULES[0]);
    for (int i = 0; i < count; i++)
    {
        pos += snprintf(
            content + pos, sizeof(content) - pos,
            "blacklist %s\n", BLUETOOTH_MODULES[i]
        );
    }

    // Blacklist Intel SOF modules.
    pos += snprintf(
        content + pos, sizeof(content) - pos,
        "\n# Intel Sound Open Firmware modules.\n"
    );
    count = sizeof(INTEL_SOF_MODULES) / sizeof(INTEL_SOF_MODULES[0]);
    for (int i = 0; i < count; i++)
    {
        pos += snprintf(
            content + pos, sizeof(content) - pos,
            "blacklist %s\n", INTEL_SOF_MODULES[i]
        );
    }

    // Write the blacklist file.
    snprintf(file_path, sizeof(file_path), "%s/blacklist-excluded-firmware.conf", dir_path);
    if (write_file(file_path, content) != 0)
    {
        LOG_WARNING("Failed to write module blacklist");
        return -1;
    }

    return 0;
}

static int exclude_firmware_and_modules(const char *rootfs_path)
{
    int result = 0;

    // Write dpkg exclusions to prevent firmware installation.
    if (write_dpkg_firmware_exclusions(rootfs_path) != 0)
    {
        result = -1;
    }

    // Write blacklist for corresponding kernel modules.
    if (write_module_blacklist(rootfs_path) != 0)
    {
        result = -1;
    }

    return result;
}

static void mask_rfkill_service(const char *rootfs_path)
{
    char dir_path[COMMAND_PATH_MAX_LENGTH];
    char mask_path[COMMAND_PATH_MAX_LENGTH];

    // Construct the systemd system directory path.
    snprintf(dir_path, sizeof(dir_path), "%s/etc/systemd/system", rootfs_path);

    // Create systemd system directory if needed.
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to create systemd system directory");
        return;
    }

    // Mask rfkill service (no RF devices to manage).
    snprintf(mask_path, sizeof(mask_path), "%s/systemd-rfkill.service", dir_path);
    if (symlink_file("/dev/null", mask_path) != 0)
    {
        LOG_WARNING("Failed to mask systemd-rfkill.service");
    }

    // Mask rfkill socket.
    snprintf(mask_path, sizeof(mask_path), "%s/systemd-rfkill.socket", dir_path);
    if (symlink_file("/dev/null", mask_path) != 0)
    {
        LOG_WARNING("Failed to mask systemd-rfkill.socket");
    }
}

int strip_base_rootfs(const char *path)
{
    char dir_path[COMMAND_PATH_MAX_LENGTH];

    LOG_INFO("Stripping base rootfs at %s", path);

    // Remove documentation files.
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/doc", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove doc directory");
        return -1;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/man", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove man directory");
        return -1;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/info", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove info directory");
        return -1;
    }

    // Remove non-English locales.
    char command[COMMAND_MAX_LENGTH];
    char quoted_locale_dir[COMMAND_QUOTED_MAX_LENGTH];
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/locale", path);
    if (shell_quote_path(dir_path, quoted_locale_dir, sizeof(quoted_locale_dir)) != 0)
    {
        LOG_ERROR("Failed to quote locale directory");
        return -2;
    }
    snprintf(
        command, sizeof(command),
        "find %s -mindepth 1 -maxdepth 1 ! -name 'en*' -exec rm -rf {} +",
        quoted_locale_dir
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to remove non-English locales");
        return -2;
    }

    // Exclude noncritical firmware and blacklist corresponding kernel modules.
    // This prevents firmware from being installed as a dependency by any future
    // package installs.
    if (exclude_firmware_and_modules(path) != 0)
    {
        LOG_WARNING("Failed to configure firmware exclusions (continuing anyway)");
    }

    // Mask rfkill service since there's no RF hardware to manage.
    mask_rfkill_service(path);

    // Clear MOTD files that display Debian messages on login.
    snprintf(dir_path, sizeof(dir_path), "%s/etc/motd", path);
    if (write_file(dir_path, "") != 0)
    {
        LOG_ERROR("Failed to clear /etc/motd");
        return -3;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/etc/update-motd.d", path);
    rm_rf(dir_path);  // OK if it doesn't exist.

    LOG_INFO("Base rootfs stripped successfully");

    return 0;
}
