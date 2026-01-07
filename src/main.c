/**
 * This code is responsible for providing the main entry point and command-line
 * interface for the LimeOS ISO builder.
 */

#include "all.h"

/** The base directory for all build files. */
#define BUILD_DIRECTORY "./build"

/** The directory where component binaries are downloaded. */
#define COMPONENTS_DIRECTORY BUILD_DIRECTORY "/components"

/** The directory where the root filesystem is created. */
#define ROOTFS_DIRECTORY BUILD_DIRECTORY "/rootfs"

/** The path to the splash logo image. */
#define SPLASH_LOGO_PATH "./assets/splash.png"

/** The path where the final ISO will be created. */
#define OUTPUT_ISO_PATH BUILD_DIRECTORY "/limeos.iso"

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
    rm_rf(BUILD_DIRECTORY);

    // Initialize the collector module.
    if (init_collector() != 0)
    {
        LOG_ERROR("Failed to initialize collector module");
        return 1;
    }

    // Log the build operation.
    LOG_INFO("Building ISO for version %s", version);

    // Fetch all required components.
    if (fetch_all_components(version, COMPONENTS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to fetch components");
        cleanup_collector();
        return 1;
    }

    LOG_INFO("Phase 1 complete: Components fetched");

    // Clean up the collector module.
    cleanup_collector();

    // Create the root filesystem.
    if (create_rootfs(ROOTFS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to create rootfs");
        return 1;
    }

    // Strip unnecessary files from the rootfs.
    if (strip_rootfs(ROOTFS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to strip rootfs");
        return 1;
    }

    // Install component binaries into the rootfs.
    if (install_components(ROOTFS_DIRECTORY, COMPONENTS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to install components");
        return 1;
    }

    // Configure the init system.
    if (configure_init(ROOTFS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to configure init");
        return 1;
    }

    LOG_INFO("Phase 2 complete: Rootfs created");

    // Configure GRUB for UEFI boot.
    if (setup_grub(ROOTFS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to configure GRUB");
        return 1;
    }

    // Configure isolinux for BIOS boot.
    if (setup_isolinux(ROOTFS_DIRECTORY) != 0)
    {
        LOG_ERROR("Failed to configure isolinux");
        return 1;
    }

    // Configure Plymouth splash screen.
    if (setup_splash(ROOTFS_DIRECTORY, SPLASH_LOGO_PATH) != 0)
    {
        LOG_ERROR("Failed to configure splash screen");
        return 1;
    }

    LOG_INFO("Phase 3 complete: Boot configured");

    // Create the final ISO image.
    if (create_iso(ROOTFS_DIRECTORY, OUTPUT_ISO_PATH) != 0)
    {
        LOG_ERROR("Failed to create ISO image");
        return 1;
    }

    LOG_INFO("Phase 4 complete: ISO assembled");
    LOG_INFO("Build complete! ISO available at: %s", OUTPUT_ISO_PATH);

    return 0;
}
