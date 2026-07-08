# Security Policy

## Supported Versions

Only the current major version of the C-GAMES-COLLECTION is actively supported with security updates. 

| Version | Supported          |
| ------- | ------------------ |
| 1.x.x   | :white_check_mark: |
| 0.x.x   | :x:                |

## Architecture & Local Security Bounds

This desktop application does not expose network attack surfaces (no remote APIs or open ports). However, the following local security practices are observed to ensure safe execution:

1. **Bounds Checking**: String operations and file parsing limit buffer sizes to prevent overflow vulnerabilities when reading configuration files.
2. **Path Traversal Protection**: The storage engine sanitizes filename inputs against directory traversal characters (`/`, `\`, `.`) during `GKeyFile` operations.
3. **Privilege Segregation**: Storage directory initialization limits permissions on the configuration directory (`data/`) to the owner, preventing concurrent system users from modifying local progress files.

## Reporting a Vulnerability

If you discover a vulnerability bypassing any local bounds—such as a buffer overflow or unauthorized pointer leak—please do **not** report it through public GitHub issues.

Instead, please use the GitHub Security Advisory private reporting tool or contact the repository maintainer directly.

### What to include in your report:
- A detailed description of the vulnerability.
- Steps to reproduce the issue.
- The operating system and GTK4 version where the bug was discovered.
- Potential impact and, if possible, a suggested mitigation.

### Response Timeline
You can expect an initial acknowledgment of your report within 48 hours. If the vulnerability is verified, a patch will be developed, and a security advisory will be issued attributing credit to the reporter.
