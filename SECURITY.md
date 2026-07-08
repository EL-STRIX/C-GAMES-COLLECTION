# Security Policy

## Supported Versions

Only the current major version of the C-GAMES-COLLECTION is actively supported with security updates. 

| Version | Supported          |
| ------- | ------------------ |
| 1.x.x   | :white_check_mark: |
| 0.x.x   | :x:                |

## Reporting a Vulnerability

Security is a top priority, especially given that this project involves local disk I/O parsing (`GKeyFile`) and dynamic native execution (`g_spawn_async`).

If you discover a security vulnerability within the repository—such as a buffer overflow, string format vulnerability, or arbitrary code execution via manipulated INI files—please do **not** report it through public GitHub issues.

Instead, please send an email to the repository maintainer directly or use the GitHub Security Advisory private reporting tool.

### What to include in your report:
- A detailed description of the vulnerability.
- Steps to reproduce the issue (including any maliciously crafted `data/` `.ini` files).
- The operating system and GTK4 version where the bug was discovered.
- Potential impact and, if possible, a suggested mitigation.

### Response Timeline
You can expect an initial acknowledgment of your report within 48 hours. If the vulnerability is verified, a patch will be developed, and a security advisory will be issued attributing credit to the reporter.
