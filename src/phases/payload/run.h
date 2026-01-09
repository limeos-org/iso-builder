#pragma once

/**
 * Runs the payload phase.
 *
 * Creates the payload rootfs (the system to be installed to disk), strips
 * unnecessary files, and packages it as a tarball for embedding in the
 * carrier.
 *
 * @param rootfs_dir The directory for the payload rootfs.
 * @param tarball_path The output path for the packaged tarball.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int run_payload_phase(const char *rootfs_dir, const char *tarball_path);
