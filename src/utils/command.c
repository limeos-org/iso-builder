/**
 * This code is responsible for shell quoting, command execution, and
 * filesystem operations.
 */

#include "all.h"

int shell_quote(const char *input, char *out_quoted, size_t buffer_length)
{
    size_t out_pos = 0;
    const char *p;

    // Validate inputs.
    if (!input || !out_quoted)
    {
        return -2;
    }

    // Start with opening single quote.
    if (out_pos >= buffer_length - 1)
    {
        return -1;
    }
    out_quoted[out_pos++] = '\'';

    // Process each character.
    for (p = input; *p; p++)
    {
        if (*p == '\'')
        {
            // Escape single quote: end quote, escaped quote, start quote.
            // Need 4 characters: '\''
            if (out_pos + 4 >= buffer_length)
            {
                return -1;
            }
            out_quoted[out_pos++] = '\'';
            out_quoted[out_pos++] = '\\';
            out_quoted[out_pos++] = '\'';
            out_quoted[out_pos++] = '\'';
        }
        else
        {
            // Copy character as-is (safe inside single quotes).
            if (out_pos >= buffer_length - 2)
            {
                return -1;
            }
            out_quoted[out_pos++] = *p;
        }
    }

    // Close with ending single quote.
    if (out_pos >= buffer_length - 1)
    {
        return -1;
    }
    out_quoted[out_pos++] = '\'';
    out_quoted[out_pos] = '\0';

    return 0;
}

int shell_quote_path(const char *path, char *out_quoted, size_t buffer_length)
{
    // Validate path is non-empty.
    if (!path || path[0] == '\0')
    {
        return -2;
    }

    return shell_quote(path, out_quoted, buffer_length);
}

/** The maximum length for a single line of command output. */
#define COMMAND_OUTPUT_LINE_MAX 4096

/** ANSI escape code for gray (bright black) text. */
#define ANSI_GRAY "\033[90m"

/** ANSI escape code to reset text formatting. */
#define ANSI_RESET "\033[0m"

int run_command(const char *command)
{
    char line[COMMAND_OUTPUT_LINE_MAX];
    char full_command[COMMAND_MAX_LENGTH];
    FILE *pipe;
    int status;

    // Redirect stderr to stdout so we capture all output.
    snprintf(full_command, sizeof(full_command), "%s 2>&1", command);

    // Open a pipe to capture command output.
    pipe = popen(full_command, "r");
    if (pipe == NULL)
    {
        return -1;
    }

    // Print each line with gray color and gutter bar.
    while (fgets(line, sizeof(line), pipe) != NULL)
    {
        printf(ANSI_GRAY "  | %s" ANSI_RESET, line);
        fflush(stdout);
    }

    // Close the pipe and extract the exit status.
    status = pclose(pipe);
    if (status == -1)
    {
        return -1;
    }

    // Return the command's exit code.
    return WEXITSTATUS(status);
}

int run_chroot(const char *rootfs_path, const char *command)
{
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_cmd[COMMAND_QUOTED_MAX_LENGTH];
    char full_command[COMMAND_MAX_LENGTH];

    // Quote the rootfs path to prevent command injection.
    if (shell_quote_path(rootfs_path, quoted_path, sizeof(quoted_path)) != 0)
    {
        return -2;
    }

    // Quote the command to prevent injection.
    if (shell_quote(command, quoted_cmd, sizeof(quoted_cmd)) != 0)
    {
        return -2;
    }

    // Execute the command inside the chroot environment via sh -c.
    snprintf(
        full_command, sizeof(full_command),
        "chroot %s /bin/sh -c %s",
        quoted_path, quoted_cmd
    );
    if (run_command(full_command) != 0)
    {
        return -1;
    }

    return 0;
}

int mkdir_p(const char *path)
{
    char command[COMMAND_MAX_LENGTH];
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];

    // Quote the path to prevent command injection.
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        LOG_ERROR("Failed to quote path: %s", path);
        return -1;
    }

    // Construct the mkdir command with parent directory creation.
    snprintf(command, sizeof(command), "mkdir -p %s", quoted_path);

    // Execute the command and check for errors.
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: mkdir -p %s", path);
        return -1;
    }

    return 0;
}

int copy_file(const char *src, const char *dst)
{
    char command[COMMAND_MAX_LENGTH];
    char quoted_src[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_dst[COMMAND_QUOTED_MAX_LENGTH];

    // Quote paths to prevent command injection.
    if (shell_quote_path(src, quoted_src, sizeof(quoted_src)) != 0)
    {
        LOG_ERROR("Failed to quote source path: %s", src);
        return -1;
    }
    if (shell_quote_path(dst, quoted_dst, sizeof(quoted_dst)) != 0)
    {
        LOG_ERROR("Failed to quote destination path: %s", dst);
        return -1;
    }

    // Construct the copy command.
    snprintf(command, sizeof(command), "cp %s %s", quoted_src, quoted_dst);

    // Execute the command and check for errors.
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: cp %s %s", src, dst);
        return -1;
    }

    return 0;
}

int rm_rf(const char *path)
{
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];

    // Quote the path to prevent command injection.
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        return -2;
    }

    // Execute the recursive remove command.
    snprintf(command, sizeof(command), "rm -rf %s", quoted_path);
    if (run_command(command) != 0)
    {
        return -1;
    }

    return 0;
}

int rm_file(const char *path)
{
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];

    // Quote the path to prevent command injection.
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        return -2;
    }

    // Execute the remove command.
    snprintf(command, sizeof(command), "rm -f %s", quoted_path);
    if (run_command(command) != 0)
    {
        return -1;
    }

    return 0;
}

int chmod_file(const char *mode, const char *path)
{
    struct stat st;

    // Validate inputs.
    if (!mode || !path)
    {
        return -2;
    }

    // Handle "+x" mode: add executable permission.
    if (strcmp(mode, "+x") == 0)
    {
        // Get current file permissions.
        if (stat(path, &st) != 0)
        {
            LOG_ERROR("Failed to stat file for chmod: %s", path);
            return -1;
        }

        // Add executable bit for user, group, and others.
        mode_t new_mode = st.st_mode | S_IXUSR | S_IXGRP | S_IXOTH;
        if (chmod(path, new_mode) != 0)
        {
            LOG_ERROR("Failed to chmod file: %s", path);
            return -1;
        }

        return 0;
    }

    // Handle numeric modes (e.g., "755", "644").
    if (mode[0] >= '0' && mode[0] <= '7')
    {
        char *endptr;
        long numeric_mode = strtol(mode, &endptr, 8);

        // Validate the mode string was fully parsed and in valid range.
        if (*endptr != '\0' || numeric_mode < 0 || numeric_mode > 07777)
        {
            LOG_ERROR("Invalid chmod mode: %s", mode);
            return -2;
        }

        if (chmod(path, (mode_t)numeric_mode) != 0)
        {
            LOG_ERROR("Failed to chmod file: %s", path);
            return -1;
        }

        return 0;
    }

    // Reject unknown mode formats to prevent injection.
    LOG_ERROR("Unsupported chmod mode format: %s", mode);
    return -2;
}

int symlink_file(const char *target, const char *link_path)
{
    char quoted_target[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_link[COMMAND_QUOTED_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];

    // Quote the target and link paths.
    if (shell_quote(target, quoted_target, sizeof(quoted_target)) != 0)
    {
        return -2;
    }
    if (shell_quote_path(link_path, quoted_link, sizeof(quoted_link)) != 0)
    {
        return -2;
    }

    // Execute the symlink command.
    snprintf(command, sizeof(command), "ln -sf %s %s", quoted_target, quoted_link);
    if (run_command(command) != 0)
    {
        return -1;
    }

    return 0;
}

int file_exists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

int write_file(const char *path, const char *content)
{
    // Open the file for writing.
    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        LOG_ERROR("Failed to create file %s: %s", path, strerror(errno));
        return -1;
    }

    // Write the content to the file.
    if (fputs(content, file) == EOF)
    {
        LOG_ERROR("Failed to write to file %s", path);
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int find_first_glob(const char *pattern, char *out_path, size_t buffer_length)
{
    glob_t glob_result;
    int ret;

    // Perform glob expansion without sorting results.
    ret = glob(pattern, GLOB_NOSORT, NULL, &glob_result);
    if (ret != 0 || glob_result.gl_pathc == 0)
    {
        globfree(&glob_result);
        return -1;
    }

    // Copy the first matched path to the output buffer.
    strncpy(out_path, glob_result.gl_pathv[0], buffer_length - 1);
    out_path[buffer_length - 1] = '\0';

    // Free glob resources.
    globfree(&glob_result);
    return 0;
}

int create_secure_tmpdir(char *out_path, size_t buffer_length)
{
    // Template for mkdtemp - X's will be replaced with unique characters.
    char template[] = CONFIG_TMPDIR_PREFIX "XXXXXX";

    // Validate buffer size.
    if (buffer_length < sizeof(template))
    {
        LOG_ERROR("Buffer too small for temporary directory path");
        return -1;
    }

    // Create the secure temporary directory.
    char *result = mkdtemp(template);
    if (result == NULL)
    {
        LOG_ERROR("Failed to create secure temporary directory: %s", strerror(errno));
        return -1;
    }

    // Copy the path to the output buffer.
    strncpy(out_path, result, buffer_length - 1);
    out_path[buffer_length - 1] = '\0';

    LOG_INFO("Created secure build directory: %s", out_path);
    return 0;
}

int cleanup_apt_directories(const char *rootfs_path)
{
    char dir_path[COMMAND_PATH_MAX_LENGTH];

    // Remove apt cache.
    snprintf(dir_path, sizeof(dir_path), "%s/var/cache/apt", rootfs_path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove apt cache");
        return -1;
    }

    // Recreate apt cache directory.
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to recreate apt cache directory");
    }

    // Remove apt lists.
    snprintf(dir_path, sizeof(dir_path), "%s/var/lib/apt/lists", rootfs_path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove apt lists");
        return -2;
    }

    // Recreate apt lists directory.
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to recreate apt lists directory");
    }

    return 0;
}

int copy_kernel_and_initrd(const char *rootfs_path)
{
    char pattern[COMMAND_PATH_MAX_LENGTH];
    char src[COMMAND_PATH_MAX_LENGTH];
    char dst[COMMAND_PATH_MAX_LENGTH];

    // Copy kernel to standard path.
    snprintf(pattern, sizeof(pattern), "%s/boot/vmlinuz-*", rootfs_path);
    if (find_first_glob(pattern, src, sizeof(src)) != 0)
    {
        LOG_ERROR("Failed to find kernel");
        return -1;
    }
    snprintf(dst, sizeof(dst), "%s/boot/vmlinuz", rootfs_path);
    if (copy_file(src, dst) != 0)
    {
        LOG_ERROR("Failed to copy kernel");
        return -1;
    }

    // Copy initrd to standard path.
    snprintf(pattern, sizeof(pattern), "%s/boot/initrd.img-*", rootfs_path);
    if (find_first_glob(pattern, src, sizeof(src)) != 0)
    {
        LOG_ERROR("Failed to find initrd");
        return -2;
    }
    snprintf(dst, sizeof(dst), "%s/boot/initrd.img", rootfs_path);
    if (copy_file(src, dst) != 0)
    {
        LOG_ERROR("Failed to copy initrd");
        return -2;
    }

    return 0;
}

/** Firmware directories to remove (relative to /usr/lib/firmware). */
static const char *FIRMWARE_TO_REMOVE[] = {
    // WiFi firmware.
    "iwlwifi",
    "ath9k_htc",
    "ath10k",
    "ath11k",
    "ath12k",
    "rtlwifi",
    "rtw88",
    "rtw89",
    "mediatek",
    "mrvl",
    // Bluetooth firmware.
    "qca",
    // Server/datacenter NICs (not needed for desktop).
    "rtl_nic",
    "cxgb4",
    "liquidio",
    "mellanox",
    "netronome",
    "dpaa2",
    "bnx2",
    "bnx2x",
    // Audio DSP firmware.
    "cirrus",
};

void cleanup_unnecessary_firmware(const char *rootfs_path)
{
    char fw_base[COMMAND_PATH_MAX_LENGTH];
    char dir_path[COMMAND_PATH_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];

    snprintf(fw_base, sizeof(fw_base), "%s/usr/lib/firmware", rootfs_path);

    // Remove firmware directories.
    int count = sizeof(FIRMWARE_TO_REMOVE) / sizeof(FIRMWARE_TO_REMOVE[0]);
    for (int i = 0; i < count; i++)
    {
        snprintf(dir_path, sizeof(dir_path), "%s/%s", fw_base, FIRMWARE_TO_REMOVE[i]);
        rm_rf(dir_path);
    }

    // Remove Intel Bluetooth firmware (files matching *bt* in intel/).
    snprintf(dir_path, sizeof(dir_path), "%s/intel", fw_base);
    if (shell_quote_path(dir_path, quoted_path, sizeof(quoted_path)) == 0)
    {
        snprintf(command, sizeof(command),
            "find %s -name '*bt*' -type f -delete 2>/dev/null", quoted_path);
        run_command(command);
    }

    // Remove Intel Sound Open Firmware.
    snprintf(dir_path, sizeof(dir_path), "%s/intel/sof", fw_base);
    rm_rf(dir_path);
    snprintf(dir_path, sizeof(dir_path), "%s/intel/sof-tplg", fw_base);
    rm_rf(dir_path);

    // Remove Broadcom Bluetooth firmware (.hcd files).
    snprintf(dir_path, sizeof(dir_path), "%s/brcm", fw_base);
    if (shell_quote_path(dir_path, quoted_path, sizeof(quoted_path)) == 0)
    {
        snprintf(command, sizeof(command),
            "find %s -name '*.hcd' -type f -delete 2>/dev/null", quoted_path);
        run_command(command);
    }

    // Remove Broadcom WiFi firmware (pcie/sdio files).
    if (shell_quote_path(dir_path, quoted_path, sizeof(quoted_path)) == 0)
    {
        snprintf(command, sizeof(command),
            "find %s \\( -name '*-pcie.*' -o -name '*-sdio.*' \\) -type f -delete 2>/dev/null",
            quoted_path);
        run_command(command);
    }

    // Also check /lib/firmware (legacy path).
    snprintf(fw_base, sizeof(fw_base), "%s/lib/firmware", rootfs_path);
    for (int i = 0; i < count; i++)
    {
        snprintf(dir_path, sizeof(dir_path), "%s/%s", fw_base, FIRMWARE_TO_REMOVE[i]);
        rm_rf(dir_path);
    }
}

void blacklist_wireless_modules(const char *rootfs_path)
{
    char dir_path[COMMAND_PATH_MAX_LENGTH];
    char file_path[COMMAND_PATH_MAX_LENGTH];

    // Create modprobe.d directory if it doesn't exist.
    snprintf(dir_path, sizeof(dir_path), "%s/etc/modprobe.d", rootfs_path);
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to create /etc/modprobe.d");
        return;
    }

    // Blacklist wireless and bluetooth modules since we removed their firmware.
    const char *content =
        "# Blacklist wireless modules (firmware removed)\n"
        "blacklist iwlwifi\n"
        "blacklist iwlmvm\n"
        "blacklist iwldvm\n"
        "blacklist ath9k\n"
        "blacklist ath9k_htc\n"
        "blacklist ath10k_pci\n"
        "blacklist ath10k_core\n"
        "blacklist ath11k\n"
        "blacklist ath11k_pci\n"
        "blacklist rtlwifi\n"
        "blacklist rtl8192ce\n"
        "blacklist rtl8192cu\n"
        "blacklist rtl8192de\n"
        "blacklist rtl8192se\n"
        "blacklist rtl8723ae\n"
        "blacklist rtl8723be\n"
        "blacklist rtw88_pci\n"
        "blacklist rtw89_pci\n"
        "blacklist brcmfmac\n"
        "blacklist brcmsmac\n"
        "blacklist mwifiex\n"
        "blacklist mwifiex_pcie\n"
        "\n"
        "# Blacklist bluetooth modules (firmware removed)\n"
        "blacklist btusb\n"
        "blacklist btrtl\n"
        "blacklist btbcm\n"
        "blacklist btintel\n"
        "blacklist bluetooth\n";

    snprintf(file_path, sizeof(file_path), "%s/blacklist-wireless.conf", dir_path);
    if (write_file(file_path, content) != 0)
    {
        LOG_WARNING("Failed to write module blacklist");
    }
}

void mask_rfkill_service(const char *rootfs_path)
{
    char dir_path[COMMAND_PATH_MAX_LENGTH];
    char mask_path[COMMAND_PATH_MAX_LENGTH];

    // Create systemd system directory if needed.
    snprintf(dir_path, sizeof(dir_path), "%s/etc/systemd/system", rootfs_path);
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to create systemd system directory");
        return;
    }

    // Mask rfkill service (no RF devices to manage).
    snprintf(mask_path, sizeof(mask_path),
        "%s/systemd-rfkill.service", dir_path);
    if (symlink_file("/dev/null", mask_path) != 0)
    {
        LOG_WARNING("Failed to mask systemd-rfkill.service");
    }

    // Mask rfkill socket.
    snprintf(mask_path, sizeof(mask_path),
        "%s/systemd-rfkill.socket", dir_path);
    if (symlink_file("/dev/null", mask_path) != 0)
    {
        LOG_WARNING("Failed to mask systemd-rfkill.socket");
    }
}
