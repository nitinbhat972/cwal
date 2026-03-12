## [0.6.0] - 2026-03-12

### 🚀 Features

- *(core)* Implement XDG compliance and variadic path builder
- *(core)* Implement full XDG compliance and variadic path builder

### 📚 Documentation

- Update readme
- Update paths and configuration details for XDG compliance
- Update README for XDG information

### ⚙️ Miscellaneous Tasks

- Update changelog for v0.5.0
- Fix previous tag detection in release workflow
- Update flake.nix for v0.5.0
## [0.5.0] - 2026-03-11

### 🚀 Features

- *(config)* Restructure INI file and add persistence for more options
- *(cli)* Make --random argument optional and improve flag handling
- *(core)* Implement theme-aware cols16 logic and config sync

### 🚜 Refactor

- Remove reload logic for nvim-colo-reload

### 📚 Documentation

- Add copyright headers and project atrribution

### 🎨 Styling

- Removed unnecessary header and blank line

### ⚙️ Miscellaneous Tasks

- Update changelog
- Remove 'Release' prefix from release title
## [0.4.1] - 2026-03-03

### 🚀 Features

- Add Neovim, Quickshell, and Dunst templates
- Add Cava template

### 🐛 Bug Fixes

- Preserve random image filename casing

### 📚 Documentation

- Add lua dependency info

### ⚡ Performance

- Reduce palette generation overhead

### 🧪 Testing

- Release workflow

### ⚙️ Miscellaneous Tasks

- Add automated changelog and release workflow
## [0.4.0] - 2025-09-04

### 🚀 Features

- Add support for custom backends using lua scripting
- Add gif support for random image path

### 🐛 Bug Fixes

- Improved GIF reading to load only first frame
- Ensure config directory and subdirectories are created
- Update the CMakeList.txt for new structure
- Replace strtok with strtok_r for thread safety
- Replace strcpy with snprintf for buffer safety
- Replace unsafe functions with safer alternatives and define constants for magic numbers

### 📚 Documentation

- Add Lua scripting support section and example to README.md

### 🎨 Styling

- Format code according to cpplint

### ⚙️ Miscellaneous Tasks

- Restructure the project
## [0.3.0] - 2025-08-02

### 🚀 Features

- Add alpha value to cache and config
- Implemented theme loading and minor changes

### 🐛 Bug Fixes

- Initialize list_theme flag in CliArgs
- Improve template directory handling and memory management

### ⚙️ Miscellaneous Tasks

- Nix flake
## [0.2.0] - 2025-07-29

### 🚀 Features

- Remove asserts folder and add to .gitignore

### 🐛 Bug Fixes

- Properly remove asserts folder from Git tracking

### 🚜 Refactor

- Replace system() with fork/exec for hook execution
## [0.1.0] - 2025-07-26
