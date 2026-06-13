# Contributing

We appreciate your interest in contributing to the **C Games Collection**. Please read through these guidelines to help keep the project consistent and the review process smooth.

## Branching Strategy

| Branch | Purpose |
| ------ | ------- |
| `main` | Contains production-ready code that passes all tests. Do not branch from here. |
| `dev`  | Active development happens here. Branch from `dev` and open a PR back into it. |

## Workflow

Clone → Branch from `dev` → Commit → Pull request

Keep changes small and focused. One logical change per branch makes review faster and easier.

## Commit & PR Prefixes

All commits and PR titles must begin with one of the following prefixes:

| Prefix | Use for |
| ------ | ------- |
| `bug:` | Defect fixes |
| `feature:` | New functionality |
| `docs:` | Documentation updates |
| `refactor:` | Code restructuring |

Use short, descriptive messages. Clearly describe what changed and why in the PR body.

## Code Style

- Follow standard C99/C11 naming conventions (e.g., `snake_case` for variables and functions).
- Match the style of surrounding code.
- Prefer simple, readable code over clever solutions.
- Use only the C standard library and GTK4 / GLib — this project is intentionally self-contained and avoids unnecessary external dependencies.

## Tests & Docs

- Add or update tests in the `tests/` directory for every data/logic change using `g_test`.
- Update relevant documentation alongside code changes.

## Before Submitting

- Verify the project builds cleanly by running `make all` from the main directory.
- Avoid introducing new dependencies.
- Clearly describe what changed and why in the PR.
- Ensure no memory leaks when manually allocating pointers (`g_free` where appropriate).

## Contributors

Once your PR is merged, add your name to [CONTRIBUTORS.md](CONTRIBUTORS.md).
