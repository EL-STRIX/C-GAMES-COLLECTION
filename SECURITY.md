# Security Policy

## Supported Versions

Only the current major version of the C-GAMES-COLLECTION is actively supported with security updates. 

| Version | Supported          |
| ------- | ------------------ |
| 1.x.x   | :white_check_mark: |
| 0.x.x   | :x:                |

## Architecture & Local Security Bounds

While this desktop application has no remote attack surfaces (no network servers, open ports, or remote API dependencies), the following strict local security bounds are enforced natively in C to defend against malicious local input or compromised multi-user environments:

1. **Memory Safety / Buffer Bounds**: Strict bounds checking (`size_t` limits) is mandated across all string operations. All internal `persistence.c` string accesses limit copying to explicitly provided buffer boundaries instead of hardcoded lengths to prevent buffer overflow vulnerabilities.
2. **Path Traversal Protection**: The high score storage engine dynamically filters all filename inputs against directory traversal characters (`/`, `\`, `.`) before performing IO operations (`GKeyFile`). 
3. **Privilege Segregation**: Storage directory initialization (`data/`) is locked strictly to `0700` (owner read/write/execute) to prevent concurrent system users from maliciously reading or overwriting local progress files.

## Reporting a Vulnerability

If you discover a vulnerability bypassing any of the aforementioned local bounds—such as a buffer overflow that can trigger arbitrary code execution via manipulated INI files or pointer leaks—please do **not** report it through public GitHub issues.

Instead, please send an email to the repository maintainer directly or use the GitHub Security Advisory private reporting tool.

### What to include in your report:
- A detailed description of the vulnerability.
- Steps to reproduce the issue (including any maliciously crafted `data/` `.ini` files).
- The operating system and GTK4 version where the bug was discovered.
- Potential impact and, if possible, a suggested mitigation.

### Response Timeline
You can expect an initial acknowledgment of your report within 48 hours. If the vulnerability is verified, a patch will be developed, and a security advisory will be issued attributing credit to the reporter.
