## Introduction

<p align="center">
  <img src="https://github.com/user-attachments/assets/c35fe6fa-d919-4831-98c5-640a6bbc4637"
alt="cwal showcase" width="700"/>
</p>

`cwal` is a fast and lightweight command-line tool for generating dynamic color schemes from images. It extracts dominant colors from your chosen image and applies them to your terminal, applications, and other system components, providing a cohesive and visually appealing desktop experience.


## ✨ Features

- **Dynamic Color Generation**: Extracts a vibrant 16-color palette from any image
- **Advanced Backend Support**: Utilizes `imagemagick` or `libimagequant` for efficient color quantization
- **Extensive Customization**: Fine-tune saturation, contrast, alpha transparency, and theme mode (dark/light)
- **Template-Based Output**: Generates color schemes for various applications using customizable templates
- **Automatic Application Reloading**: Seamlessly integrates with your system to apply changes instantly
- **Palette Preview**: View the generated color palette directly in your terminal
- **Random Image Selection**: Automatically pick a random image from any specified directory
- **Theme Management**: Load predefined themes or select random themes based on mode (dark/light/all)


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

`cwal` requires `imagemagick` and `libimagequant` as dependencies.


### Prerequisites

Ensure the following libraries are installed on your system:

- `imagemagick`
- `libimagequant`

**Ubuntu/Debian:**

```bash
sudo apt install imagemagick libimagequant-dev
```

**Arch Linux:**

```bash
sudo pacman -S imagemagick libimagequant
```

**Fedora/RHEL:**

```bash
sudo dnf install ImageMagick-devel libimagequant-devel
```

**macOS**
```bash
brew install imagemagick libimagequant
```


### Arch Linux (AUR)

Install directly from the AUR:

```bash
yay -S cwal
# or
paru -S cwal
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
- `--random <directory>`                Select random image from directory
- `--theme <theme_name|random_dark|random_light|random_all>` Select a theme or a random one
- `--preview`                           Preview palette
- `--help`                              Help

**Examples:**

```bash
cwal --img /path/to/image.jpg
cwal --img /path/to/image.png --mode dark --saturation 0.2
cwal --img /path/to/image.jpg --preview
cwal --random ~/Pictures/wallpapers
cwal --theme random_dark
cwal --theme random_light
cwal --theme random_all
cwal --list-themes
cwal --img /path/to/image.jpg --out-dir ~/.config/colors --script ~/.local/bin/reload-apps.sh
```


## 🎨 Templates

Templates are stored in:

- `/usr/local/share/cwal/templates` (system-wide)
- `~/.config/cwal/templates` (user)

**Supported apps:**
Terminal emulators (Alacritty, Kitty, Wezterm), window managers (i3, bspwm, Hyprland), text editors (Vim, Neovim, VS Code), system themes (GTK, Qt).

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
| `alpha_dec`      | Alpha transparency value (0.0-1.0)              | `0.8`                                                        |

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
- Post-process: `cwal --img image.jpg --script ~/.local/bin/update-theme.sh`
- Batch processing:

```bash
for img in ~/Pictures/wallpapers/*.{jpg,png,jpeg}; do
    cwal --img "$img" --quiet
done
```


## 🤝 Contributing

Report issues, request features, or contribute via PRs. See the GitHub repository for more info.

## 📄 License

Licensed under GNU GPL v3.0 — always free and open-source.

**Star the project on GitHub if you find it useful!**


## Special Thanks

- [pywal](https://github.com/dylanaraps/pywal) by [dylanaraps](https://github.com/dylanaraps)

- [pywal16](https://github.com/eylles/pywal16) by [eylles](https://github.com/eylles)
