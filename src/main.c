/**
 * This code is responsible for providing the main entry point and command-line
 * interface for the LimeOS ISO builder.
 */

#include "all.h"

static void print_usage(const char *program_name)
{
    printf("Usage: %s <version> [options]\n", program_name);
    printf("\n");
    printf("Arguments:\n");
    printf("  <version>       Version tag to build (e.g., 1.0.0)\n");
    printf("\n");
    printf("Options:\n");
    printf("  --help          Show this help message\n");
}

int main(int argc, char *argv[])
{
    const char *version = NULL;

    // Verify the program is running as root.
    if (geteuid() != 0)
    {
        LOG_ERROR("This program must be run as root");
        return 1;
    }

    // Parse command-line arguments.
    int option;
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    while ((option = getopt_long(argc, argv, "h", long_options, NULL)) != -1)
    {
        switch (option)
        {
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // Validate that a version argument was provided.
    if (optind >= argc)
    {
        LOG_ERROR("Missing required argument: version");
        print_usage(argv[0]);
        return 1;
    }

    // Extract the version from positional arguments.
    version = argv[optind];

    // Validate the version format.
    if (validate_version(version) != 0)
    {
        LOG_ERROR("Invalid version format: %s (expected: X.Y.Z or vX.Y.Z)", version);
        return 1;
    }

    // Clean up old build files.
    LOG_INFO("Cleaning old build files...");
    rm_rf(CONFIG_BUILD_DIRECTORY);

    // Initialize the fetch module.
    if (init_fetch() != 0)
    {
        LOG_ERROR("Failed to initialize fetch module");
        return 1;
    }

    // Log the build operation.
    LOG_INFO("Building ISO for version %s", version);

    // Fetch all required components.
    if (fetch_all_components(version, CONFIG_COMPONENTS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to fetch components");
        cleanup_fetch();
        return 1;
    }

    LOG_INFO("Phase 1 complete: Components fetched");

    // Clean up the fetch module.
    cleanup_fetch();

    // Create the root filesystem.
    if (create_rootfs(CONFIG_ROOTFS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to create rootfs");
        return 1;
    }

    // Strip unnecessary files from the rootfs.
    if (strip_rootfs(CONFIG_ROOTFS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to strip rootfs");
        return 1;
    }

    // Install component binaries into the rootfs.
    if (install_components(CONFIG_ROOTFS_DIRECTORY, CONFIG_COMPONENTS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to install components");
        return 1;
    }

    // Configure the init system.
    if (configure_init(CONFIG_ROOTFS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to configure init");
        return 1;
    }

    LOG_INFO("Phase 2 complete: Rootfs created");

    // Configure GRUB for UEFI boot.
    if (setup_grub(CONFIG_ROOTFS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to configure GRUB");
        return 1;
    }

    // Configure isolinux for BIOS boot.
    if (setup_isolinux(CONFIG_ROOTFS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to configure isolinux");
        return 1;
    }

    // Configure Plymouth splash screen.
    if (setup_splash(CONFIG_ROOTFS_DIRECTORY, CONFIG_SPLASH_LOGO_PATH) != 0)
    {
        LOG_ERROR("Failed to configure splash screen");
        return 1;
    }

    LOG_INFO("Phase 3 complete: Boot configured");

    // Create the final ISO image.
    if (create_iso(CONFIG_ROOTFS_DIRECTORY, CONFIG_OUTPUT_ISO_PATH) != 0)
    {
        LOG_ERROR("Failed to create ISO image");
        return 1;
    }

    LOG_INFO("Phase 4 complete: ISO assembled");
    LOG_INFO("Build complete! ISO available at: %s", CONFIG_OUTPUT_ISO_PATH);

    return 0;
}
