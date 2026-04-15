## Introduction

<p align="center">
  <img src="https://github.com/user-attachments/assets/c35fe6fa-d919-4831-98c5-640a6bbc4637"
alt="cwal showcase" width="700"/>
</p>

`cwal` is a fast and lightweight command-line tool for generating dynamic color schemes from images. It extracts dominant colors from your chosen image and applies them to your terminal, applications, and other system components, providing a cohesive and visually appealing desktop experience.


## ✨ Features

- **Dynamic Color Generation**: Extracts a vibrant 16-color palette from any image
- **Surgical Config Injection**: Update specific sections of your existing configuration files without losing manual edits
- **Persistent Configuration**: Remembers your preferred settings (alpha, mode, saturation, etc.) across sessions using a structured, XDG-compliant INI file
- **XDG Compliant**: Follows the XDG Base Directory Specification for config, cache, and data
- **Advanced Backend Support**: Utilizes ImageMagick or `libimagequant` for efficient color quantization
- **Lua Scripting Support**: Create custom backends using Lua scripts for advanced color quantization
- **Extensive Customization**: Fine-tune saturation, contrast, alpha transparency, and theme mode (dark/light)
- **Smart Template Engine**: Generates color schemes for various applications with intelligent shade generation
- **Automatic Application Reloading**: Seamlessly integrates with your system to apply changes instantly
- **Palette Preview**: View the generated color palette directly in your terminal
- **Random Image Selection**: Automatically pick a random image from a directory (remembers your last directory)
- **Theme Management**: Load predefined themes or select random themes with automatic generation bypass
- **Shell Completions**: Smart completion scripts for Bash, Zsh, and Fish


## 🖼️ Showcase


- Dark mode

<p align="center">
  <img src="https://github.com/user-attachments/assets/ddc1b78b-a4fd-4607-8919-071f1ef4785f"" width="260"/>
  <img src="https://github.com/user-attachments/assets/3f0a2d95-10a2-4713-bf29-d7e270e7f15c" width="260"/>
  <img src="https://github.com/user-attachments/assets/f41fd575-fde2-4fca-8445-9640d4f45476" width="260"/>
</p>
<p align="center">
  <img src="https://github.com/user-attachments/assets/45cebfd0-ba5c-4b91-a8fd-61fb44c1cbcd" width="260"/>
  <img src="https://github.com/user-attachments/assets/dc831594-36b5-4856-89d4-d248139d2949" width="260"/>
  <img src="https://github.com/user-attachments/assets/0098d56e-7620-4719-b6d0-90816ec1a40a" width="260"/>
</p>

- Light mode
<p align="center">
  <img src="https://github.com/user-attachments/assets/3a14f92f-64fe-411a-abb8-3b247907b594" width="260"/>
  <img src="https://github.com/user-attachments/assets/2c1406e2-737b-43d5-9855-4648948f1ef0" width="260"/>
  <img src="https://github.com/user-attachments/assets/2884d220-5c6c-4e57-b14b-a9be54d8fa9c" width="260"/>
</p>
<p align="center">
  <img src="https://github.com/user-attachments/assets/229f2491-25f7-4e3e-86fa-278c6207e59e" width="260"/>
  <img src="https://github.com/user-attachments/assets/2b585d8c-9fd3-4382-81e9-f25ebe5d39ab" width="260"/>
  <img src="https://github.com/user-attachments/assets/2ff60780-328d-4dbd-ba8e-4b4f776b510a" width="260"/>
</p>


## 🚀 Installation


`cwal` requires ImageMagick 6 or 7, `libimagequant`, and LuaJIT as dependencies.

### Prerequisites

Ensure the following libraries are installed on your system:

- `ImageMagick 6 or 7` (`MagickWand`)
- `libimagequant`
- `LuaJIT`

**Ubuntu/Debian**

```bash
sudo apt install libmagickwand-dev libimagequant-dev libluajit-5.1-dev
```

**Arch Linux**

```bash
sudo pacman -S imagemagick libimagequant luajit
```

**Fedora/RHEL**

```bash
sudo dnf install ImageMagick-devel libimagequant-devel luajit-devel
```

**macOS**
```bash
brew install imagemagick libimagequant luajit
```

## Package Manager

**Arch Linux (AUR)**

```bash
yay -S cwal
# or
paru -S cwal
```

**Homebrew**

```bash
brew tap nitinbhat972/cwal
brew install cwal
```

## Nix

### Run without installing
```bash
nix run github:nitinbhat972/cwal
```

### Install (user profile)
```bash
nix profile install nixpkgs#cwal
```

### NixOS (system-wide)
```nix
environment.systemPackages = with pkgs; [
  cwal
];
```

### Building from Source

1. **Clone the repository:**

```bash
git clone https://github.com/nitinbhat972/cwal.git
cd cwal
```

2. **Build and install:**

*User-specific:*

```bash
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local ..
make
make install
```

*System-wide:*

```bash
mkdir build && cd build
cmake ..
make
sudo make install
```

## 💡 Usage

```bash
Usage: cwal [OPTIONS] --img <image_path>
```

- `--img <image_path>`                 Specify the image path (required)
- `--mode <dark|light>`                 Set theme mode
- `--cols16-mode <darken|lighten>`      Set 16-color mode
- `--saturation <float>`                Overall saturation
- `--contrast <float>`                  Contrast ratio
- `--alpha <float>`                     Alpha transparency (0.0-1.0)
- `--out-dir <path>`                    Output directory for generated files
- `--backend <name>`                    Set image processing backend
- `--script <script_path>`              Run custom script after processing
- `--no-reload`                         Disable reloading
- `--list-backends`                     List available backends
- `--list-themes`                       List all available themes
- `--quiet`                             Suppress all output
- `--random [directory]`                Select random image (uses config default if directory omitted)
- `--theme <theme_name|random_all>`     Select a theme or a random one
- `--preview`                           Preview palette
- `--version`                           Show version number
- `--help`                              Help

**Examples:**

```bash
cwal --img /path/to/image.jpg
cwal --random ~/Pictures/wallpapers    # Use specific path and save it
cwal --random                          # Use the directory saved in config
cwal --theme random_all                # Pick a random predefined theme
cwal --img /path/to/image.jpg --alpha 0.8 --saturation 0.1
```

## ⚙️ Configuration

`cwal` follows the **XDG Base Directory Specification**.

The structured INI file is located at `${XDG_CONFIG_HOME:-~/.config}/cwal/cwal.ini`. Most CLI flags are automatically saved to this file, so you don't have to provide them every time.

```ini
[general]
out_dir = /home/user/.cache/cwal/
backend = cwal
script_path = /home/user/.local/bin/post-cwal.sh

[options]
alpha = 1.00
saturation = 0.00
contrast = 1.00
mode = dark
cols16_mode = darken

[random]
random_dir = /home/user/Pictures/Wallpapers

[links]
# format: template_name = destination_path | reload_command
colors-waybar.css = ~/.config/waybar/colors.css | pkill -USR2 waybar
colors-sway = ~/.config/sway/config | swaymsg reload
```

### Surgical Injection (Placeholders)

Instead of overwriting an entire configuration file, you can add markers to your existing files. `cwal` will only replace the text between these markers:

```toml
# ~/.config/alacritty/alacritty.toml
[window]
padding = { x = 5, y = 5 }

# --- $CWAL_START ---
# (cwal will inject colors here)
# --- $CWAL_END ---

[font]
size = 12
```


## 🎨 Templates

`cwal` searches for templates in the following order:

1. `${XDG_DATA_DIRS:-/usr/local/share:/usr/share}/cwal/templates` (system-wide)
2. `${XDG_DATA_HOME:-~/.local/share}/cwal/templates` (user local)
3. `${XDG_CONFIG_HOME:-~/.config}/cwal/templates` (user config)

**Supported apps:**
Terminal emulators (Alacritty, Kitty, Wezterm, Ghostty, Foot), window managers (i3, bspwm, Hyprland, Sway), system monitors (Btop), text editors (Vim, Neovim, VS Code), and more.

### Color Formatting in Templates

`cwal` templates support various color formatting options. You can use these formats within your templates to customize the output for different applications.

| Format Specifier | Description                                     | Example Output (for color with R=255, G=128, B=0, Alpha=0.8) |
| :--------------- | :---------------------------------------------- | :----------------------------------------------------------- |
| `hex`            | Hexadecimal color code (e.g., `#RRGGBB`)       | `#ff8000`                                                    |
| `xhex`           | Hexadecimal color code with `0x` prefix         | `0xff8000`                                                   |
| `strip`          | Hexadecimal color code without prefix           | `ff8000`                                                     |
| `rgb`            | RGB format (e.g., `rgb(R,G,B)`)                 | `rgb(255,128,0)`                                             |
| `rgba`           | RGBA format (e.g., `rgba(R,G,B,A)`)             | `rgba(255,128,0,0.8)`                                        |
| `red`            | Red component value (0-255)                     | `255`                                                        |
| `green`          | Green component value (0-255)                   | `128`                                                        |
| `blue`           | Blue component value (0-255)                    | `0`                                                          |


In addition to color-specific formats, you can use these global placeholders anywhere in your template:

| Placeholder   | Description                                      | Example Output |
| :------------ | :----------------------------------------------- | :------------- |
| `{mode}`      | Current theme mode (`dark` or `light`)           | `dark`         |
| `{wallpaper}` | Absolute path to the current wallpaper image     | `/home/user/Pictures/wall.jpg` |
| `{alpha}`     | Alpha transparency value (0.00 - 1.00)   | `0.80`         |


**Example usage in a template:**

```
# For color0 (background)
background = {color0.hex}
background_rgb = {color0.rgb}
background_alpha = {color0.rgba}

# For color1 (foreground)
foreground = {color1.strip}
foreground_red = {color1.red}
```

## 🔧 Advanced Usage

- Check available backends: `cwal --list-backends`
- Choose backend: `cwal --img image.jpg --backend libimagequant`
- Post-process script: `cwal --img image.jpg --script "~/.local/bin/update-theme.sh $current_wallpaper"`
  *(Note: You can use the `$current_wallpaper` placeholder in your script path or arguments, and `cwal` will automatically replace it with the path of the currently processed image).*
- Batch processing:

```bash
for img in ~/Pictures/wallpapers/*.{jpg,png,jpeg}; do
    cwal --img "$img" --quiet
done
```


## 🧩 Lua Scripting Support

cwal now supports custom backends using Lua scripts. This allows you to implement your own color quantization algorithms or image processing techniques.

To create a custom backend:

1. Create a Lua script with a `Main(image_path)` function that returns a table of 16 colors, each as `{r, g, b}` where `r`, `g`, `b` are integers 0-255.

2. Place the script in `${XDG_CONFIG_HOME:-~/.config}/cwal/backends/` (the directory will be created if it doesn't exist).

3. Use the backend by its name (script filename without `.lua`) with `--backend <name>`.

The script receives the image path and should process it to generate the palette.

Simple template:

```lua
function Main(image_path)
        -- Open the image from the path that cwal passed in.
        local image = open_image(image_path)

        -- Run your palette extraction or quantization code here.
        local palette = quantize_algorithm(image)

        return palette
end
```

This is pseudo-code. `open_image` and `quantize_algorithm` are placeholders for your own logic. The important part is that `Main` receives `image_path` and returns exactly 16 colors as `{r, g, b}` entries.

Save it as:

```text
${XDG_CONFIG_HOME:-~/.config}/cwal/backends/mybackend.lua
```

Then run:

```bash
cwal --img ~/Pictures/wallpapers/forest.jpg --backend mybackend
```

`image_path` is the wallpaper path passed in by `cwal`. You can ignore it like this example does, or use it later when you want more custom logic.


## 🐚 Shell Completions

`cwal` includes comprehensive, XDG-compliant completion scripts for **Bash**, **Zsh**, and **Fish**. These scripts provide smart suggestions for flags, theme names, and image files.

- **Zsh**: Ensure `/usr/local/share/zsh/site-functions` is in your `$fpath`.
- **Fish**: Completions are installed to `/usr/local/share/fish/vendor_completions.d/` and work automatically.
- **Bash**: Ensure the `bash-completion` package is installed.

For manual installation, you can source the scripts located in the `shell/` directory.

## 🖊 Name and Attribution

The name "cwal" is the official project name for the upstream repository:
https://github.com/nitinbhat972/cwal

If you distribute modified versions of this project, please use a different
name to avoid confusion with the original project.


## 🤝 Contributing

Report issues, request features, or contribute via PRs. See the GitHub repository for more info.

## 📄 License

Licensed under GNU GPL v3.0 — always free and open-source.

**Star the project on GitHub if you find it useful!**


## Special Thanks

- [pywal](https://github.com/dylanaraps/pywal) by [dylanaraps](https://github.com/dylanaraps)

- [pywal16](https://github.com/eylles/pywal16) by [eylles](https://github.com/eylles)
