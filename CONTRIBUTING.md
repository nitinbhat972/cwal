# Contributing to cwal

Thank you for your interest in contributing to cwal! To maintain a clean and readable history, we follow specific guidelines for commit messages.

---

## Getting Started

1. **Fork** the repository and clone your fork locally.
2. Create a **new branch** for your change:
   ```text
   git checkout -b <type>/<short-description>
   ```
   Examples: `feat/template-command-links`, `fix/default-config-path`
3. Make your changes, commit using the format below, and open a **pull request** against `main`.

---

## Commit Message Format

We follow the [Conventional Commits](https://www.conventionalcommits.org/) specification:

```text
<type>(<scope>): <subject>

[optional body]

[optional footer(s)]
```

For documentation changes, the scope is typically omitted:

```text
docs: <subject>
```

### Full Example

```text
feat(backend): add support for parallel extraction

Previously, extraction ran sequentially which caused slowdowns on large
inputs. This change introduces a worker pool to run jobs concurrently.

Closes #42
```

---

### Breaking Changes

For changes that introduce breaking modifications, append a `!` after the type or scope:

```text
feat(backend)!: rewrite extraction engine
```

or

```text
fix!: change default configuration path
```

You should also describe the breaking change in the **footer** using the `BREAKING CHANGE:` token:

```text
feat(config)!: rename output_dir to output_path

BREAKING CHANGE: the `output_dir` config key has been renamed to
`output_path`. Update your configuration files accordingly.
```

---

### Types

| Type | When to use | Example |
|------|-------------|---------|
| `feat` | A new feature | `feat(template): support command only links` |
| `fix` | A bug fix | `fix(template): remove redundant braces` |
| `docs` | Documentation changes | `docs: update link configuration examples` |
| `refactor` | Code change that neither fixes a bug nor adds a feature | `refactor(template): refine status bar item hover colors` |
| `style` | Whitespace, formatting, or other non-semantic changes | `style: normalize indentation in config parser` |
| `perf` | A code change that improves performance | `perf(cache): reduce redundant disk reads` |
| `chore` | Build process or tooling changes | `chore(release): update version v0.8.5` |
| `revert` | Reverts a previous commit | `revert: feat(backend): add parallel extraction` |

---

### Language & Style

- **Use the imperative mood:** Write "add", "remove", "update", "fix" — not "added", "removed", "updated", "fixed".
- **Keep it short:** The subject line should be 72 characters or fewer.
- **Lowercase:** The subject should start with a lowercase letter and not end with a period.
- **Body (optional):** Use the body to explain *what* changed and *why*, not *how*. Wrap lines at 72 characters.
- **Footer (optional):** Reference issues (`Closes #123`, `Fixes #456`) or document breaking changes here.

#### Good vs. Bad Examples

| ✅ Good | ❌ Bad |
|--------|--------|
| `fix(cache): prevent stale reads on cold start` | `Fixed bug` |
| `feat(cli): add --verbose flag` | `Added new feature for CLI` |
| `docs: update link configuration examples` | `docs: Updated docs.` |
| `refactor(utils): simplify path resolution logic` | `WIP` |

---

### Common Scopes

- `cli`
- `backend`
- `template` (For changes to the core template engine in `src/`)
- `template/<app>` (For adding or modifying specific template files, e.g., `feat(template/hyprland): ...`)
- `config`
- `cache`
- `utils`

Scopes should reflect the area of the codebase affected. If a change genuinely spans multiple scopes, consider splitting it into separate commits. For small changes that touch multiple areas, scopes may be comma-separated (e.g., `refactor(backend,color): ...`).
