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
    printf("  --no-cache      Disable rootfs caching\n");
}

int main(int argc, char *argv[])
{
    const char *version = NULL;
    char build_dir[COMMAND_PATH_MAX_LENGTH];
    char components_dir[COMMAND_PATH_MAX_LENGTH];
    char base_rootfs_dir[COMMAND_PATH_MAX_LENGTH];
    char target_rootfs_dir[COMMAND_PATH_MAX_LENGTH];
    char target_tarball_path[COMMAND_PATH_MAX_LENGTH];
    char carrier_rootfs_dir[COMMAND_PATH_MAX_LENGTH];
    int exit_code = 0;
    int use_cache = 1;

    // Verify the program is running as root.
    if (geteuid() != 0)
    {
        LOG_ERROR("This program must be run as root");
        return 1;
    }

    // Validate all required dependencies are available.
    if (validate_dependencies() != 0)
    {
        LOG_ERROR("Missing dependencies, cannot continue");
        return 1;
    }

    // Parse command-line arguments.
    int option;
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"no-cache", no_argument, 0, 'n'},
        {0, 0, 0, 0}
    };
    while ((option = getopt_long(argc, argv, "hn", long_options, NULL)) != -1)
    {
        switch (option)
        {
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'n':
                use_cache = 0;
                break;
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

    // Extract and validate the version.
    version = argv[optind];
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

    // Install signal handlers for graceful shutdown.
    install_signal_handlers(build_dir);

    // Construct derived paths.
    snprintf(components_dir, sizeof(components_dir), "%s/components", build_dir);
    snprintf(base_rootfs_dir, sizeof(base_rootfs_dir), "%s/base-rootfs", build_dir);
    snprintf(target_rootfs_dir, sizeof(target_rootfs_dir), "%s/target-rootfs", build_dir);
    snprintf(target_tarball_path, sizeof(target_tarball_path), "%s/rootfs.tar.gz", build_dir);
    snprintf(carrier_rootfs_dir, sizeof(carrier_rootfs_dir), "%s/carrier-rootfs", build_dir);

    LOG_INFO("Building ISO for version %s", version);

    // Phase 1: Preparation - fetch components from GitHub.
    if (run_preparation_phase(version, components_dir) != 0)
    {
        exit_code = 1;
        goto cleanup;
    }
    if (check_interrupted()) return 130;

    // Phase 2: Base - create and strip base rootfs.
    if (run_base_phase(base_rootfs_dir, use_cache) != 0)
    {
        exit_code = 1;
        goto cleanup;
    }
    if (check_interrupted()) return 130;

    // Phase 3: Target - copy base, install packages, brand, package.
    if (run_target_phase(base_rootfs_dir, target_rootfs_dir, target_tarball_path, version, use_cache) != 0)
    {
        exit_code = 1;
        goto cleanup;
    }
    if (check_interrupted()) return 130;

    // Phase 4: Carrier - copy base, install packages, embed target.
    if (run_carrier_phase(base_rootfs_dir, carrier_rootfs_dir, target_tarball_path, components_dir, version, use_cache) != 0)
    {
        exit_code = 1;
        goto cleanup;
    }
    if (check_interrupted()) return 130;

    // Base rootfs no longer needed after target and carrier are created.
    rm_rf(base_rootfs_dir);

    // Phase 5: Assembly - configure bootloaders and create ISO.
    if (run_assembly_phase(carrier_rootfs_dir, version) != 0)
    {
        exit_code = 1;
        goto cleanup;
    }

cleanup:
    rm_rf(build_dir);
    clear_cleanup_dir();
    return exit_code;
}
