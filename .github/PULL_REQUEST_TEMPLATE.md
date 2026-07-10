## Description
Describe the changes introduced by this Pull Request. List the specific games or shared components (`src/common/`) that were modified.

Fixes # (issue number)

## Type of Change
- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (modifies persistence schema or shared boundaries)
- [ ] Documentation update
- [ ] CSS/Styling enhancement

## Quality Assurance Checklist
- [ ] My code strictly follows the `snake_case` conventions of this repository.
- [ ] I did not introduce any global variables (state is encapsulated in `AppData`).
- [ ] The code compiles cleanly and produces zero warnings (`-Wall -Wextra -Wpedantic`).
- [ ] I ran the test suite (`make test` or `build.bat`) and all headless GLib tests pass successfully.
- [ ] Any UI styling changes were placed in `assets/css/` rather than hardcoded in C.
- [ ] Memory dynamically allocated with `g_new0` or `malloc` is properly managed and freed to prevent leaks.

## Visual Changes (if applicable)
If this PR alters the GTK4 UI, please attach before/after screenshots here.
