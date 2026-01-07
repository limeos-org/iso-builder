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
    char build_dir[COMMAND_PATH_MAX_LENGTH];
    char components_dir[COMMAND_PATH_MAX_LENGTH];
    char rootfs_dir[COMMAND_PATH_MAX_LENGTH];
    int exit_code = 0;

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

    // Create a secure temporary build directory.
    if (create_secure_tmpdir(build_dir, sizeof(build_dir)) != 0)
    {
        LOG_ERROR("Failed to create secure build directory");
        return 1;
    }

    // Construct derived paths.
    snprintf(components_dir, sizeof(components_dir), "%s/components", build_dir);
    snprintf(rootfs_dir, sizeof(rootfs_dir), "%s/rootfs", build_dir);

    // Initialize the fetch module.
    if (init_fetch() != 0)
    {
        LOG_ERROR("Failed to initialize fetch module");
        rm_rf(build_dir);
        return 1;
    }

    // Log the build operation.
    LOG_INFO("Building ISO for version %s", version);

    // Fetch all required components.
    if (fetch_all_components(version, components_dir) != 0)
    {
        LOG_ERROR("Failed to fetch components");
        cleanup_fetch();
        rm_rf(build_dir);
        return 1;
    }

    LOG_INFO("Phase 1 complete: Components fetched");

    // Clean up the fetch module.
    cleanup_fetch();

    // Create the root filesystem.
    if (create_rootfs(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create rootfs");
        exit_code = 1;
        goto cleanup;
    }

    // Strip unnecessary files from the rootfs.
    if (strip_rootfs(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to strip rootfs");
        exit_code = 1;
        goto cleanup;
    }

    // Install component binaries into the rootfs.
    if (install_components(rootfs_dir, components_dir) != 0)
    {
        LOG_ERROR("Failed to install components");
        exit_code = 1;
        goto cleanup;
    }

    // Configure the init system.
    if (configure_init(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to configure init");
        exit_code = 1;
        goto cleanup;
    }

    LOG_INFO("Phase 2 complete: Rootfs created");

    // Configure GRUB for UEFI boot.
    if (setup_grub(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to configure GRUB");
        exit_code = 1;
        goto cleanup;
    }

    // Configure isolinux for BIOS boot.
    if (setup_isolinux(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to configure isolinux");
        exit_code = 1;
        goto cleanup;
    }

    // Configure Plymouth splash screen.
    if (setup_splash(rootfs_dir, CONFIG_SPLASH_LOGO_PATH) != 0)
    {
        LOG_ERROR("Failed to configure splash screen");
        exit_code = 1;
        goto cleanup;
    }

    LOG_INFO("Phase 3 complete: Boot configured");

    // Construct the ISO path with version in current directory.
    char iso_output_path[256];
    snprintf(iso_output_path, sizeof(iso_output_path), "limeos-%s.iso", version);

    // Create the final ISO image.
    if (create_iso(rootfs_dir, iso_output_path) != 0)
    {
        LOG_ERROR("Failed to create ISO image");
        exit_code = 1;
        goto cleanup;
    }

    LOG_INFO("Phase 4 complete: ISO assembled");
    LOG_INFO("Build complete! ISO available at: %s", iso_output_path);

cleanup:
    // Clean up the secure build directory.
    rm_rf(build_dir);
    return exit_code;
}
