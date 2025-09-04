## Introduction

<p align="center">
  <img src="https://github.com/user-attachments/assets/c35fe6fa-d919-4831-98c5-640a6bbc4637"
alt="cwal showcase" width="700"/>
</p>

`cwal` is a fast and lightweight command-line tool for generating dynamic color schemes from images. It extracts dominant colors from your chosen image and applies them to your terminal, applications, and other system components, providing a cohesive and visually appealing desktop experience.


## ✨ Features

- **Dynamic Color Generation**: Extracts a vibrant 16-color palette from any image
- **Advanced Backend Support**: Utilizes `imagemagick` or `libimagequant` for efficient color quantization
- **Lua Scripting Support**: Create custom backends using Lua scripts for advanced color quantization
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


## Lua Scripting Support

cwal now supports custom backends using Lua scripts. This allows you to implement your own color quantization algorithms or image processing techniques.

To create a custom backend:

1. Create a Lua script with a `Main(image_path)` function that returns a table of 16 colors, each as `{r, g, b}` where `r`, `g`, `b` are integers 0-255.

2. Place the script in `~/.config/cwal/backends/` (the directory will be created if it doesn't exist).

3. Use the backend by its name (script filename without `.lua`) with `--backend <name>`.

The script receives the image path and should process it to generate the palette.

<details><summary>Example</summary>

```lua
local ffi = require("ffi")

local function raw_to_pixels(data, size)
	local expected = size * size * 3
	if #data < expected then
		return nil, string.format("expected >= %d bytes, got %d", expected, #data)
	end

	local buf = ffi.cast("const unsigned char*", data)
	local pixels = {}
	pixels[#pixels + size * size] = false -- preallocate

	local idx = 1
	for i = 0, size * size - 1 do
		local base = i * 3
		pixels[idx] = { buf[base], buf[base + 1], buf[base + 2] }
		idx = idx + 1
	end
	return pixels
end

local function try_read_pixels_with(path, size)
	local quoted = string.format('"%s"', path)
	local conv = string.format("magick %s -resize %dx%d! -colorspace sRGB -depth 8 rgb:-", quoted, size, size)
	local f = io.popen(conv, "r")
	if not f then
		return nil, "popen failed"
	end
	local data = f:read("*all")
	f:close()
	if not data or #data == 0 then
		return nil, "no data"
	end
	return raw_to_pixels(data, size)
end

local function read_pixels(path, size)
	local px, err = try_read_pixels_with(path, size)
	if px then
		return px
	end
	error("Could not read pixels via ImageMagick: " .. tostring(err))
end

local function dist2(a, b)
	local dr = a[1] - b[1]
	local dg = a[2] - b[2]
	local db = a[3] - b[3]
	return dr * dr + dg * dg + db * db
end

local function init_centroids_kpp(pixels, k)
	local n = #pixels
	if k > n then
		k = n
	end
	local centroids = {}
	local i1 = math.random(n)
	centroids[1] = { pixels[i1][1], pixels[i1][2], pixels[i1][3] }

	local function nearest_d2(p)
		local best = math.huge
		for i = 1, #centroids do
			local d = dist2(p, centroids[i])
			if d < best then
				best = d
			end
		end
		return best
	end

	while #centroids < k do
		local dsum, d2s = 0.0, {}
		for i = 1, n do
			local d = nearest_d2(pixels[i])
			d2s[i] = d
			dsum = dsum + d
		end
		local r, acc = math.random() * dsum, 0.0
		for i = 1, n do
			acc = acc + d2s[i]
			if acc >= r then
				local p = pixels[i]
				centroids[#centroids + 1] = { p[1], p[2], p[3] }
				break
			end
		end
		if #centroids < 2 then
			break
		end
	end
	return centroids
end

local function kmeans(pixels, k, max_iter)
	max_iter = max_iter or 25
	local n = #pixels
	if n == 0 then
		return {}
	end
	if k < 1 then
		k = 1
	elseif k > n then
		k = n
	end

	math.randomseed(tonumber(tostring(os.clock()):gsub("%D", "")))

	local centroids = init_centroids_kpp(pixels, k)
	local assign = ffi.new("int[?]", n)

	local changed, iter = true, 0
	while changed and iter < max_iter do
		iter, changed = iter + 1, false

		-- assign step
		for i = 1, n do
			local p = pixels[i]
			local best_k, best_d = 1, dist2(p, centroids[1])
			for c = 2, k do
				local d = dist2(p, centroids[c])
				if d < best_d then
					best_d, best_k = d, c
				end
			end
			if assign[i - 1] ~= best_k then
				assign[i - 1] = best_k
				changed = true
			end
		end

		if not changed then
			break
		end

		-- update step
		local sumR, sumG, sumB, count = {}, {}, {}, {}
		for c = 1, k do
			sumR[c], sumG[c], sumB[c], count[c] = 0, 0, 0, 0
		end

		for i = 1, n do
			local c = assign[i - 1]
			local p = pixels[i]
			sumR[c] = sumR[c] + p[1]
			sumG[c] = sumG[c] + p[2]
			sumB[c] = sumB[c] + p[3]
			count[c] = count[c] + 1
		end

		for c = 1, k do
			if count[c] > 0 then
				centroids[c][1] = sumR[c] / count[c]
				centroids[c][2] = sumG[c] / count[c]
				centroids[c][3] = sumB[c] / count[c]
			else
				local rp = pixels[math.random(n)]
				centroids[c][1], centroids[c][2], centroids[c][3] = rp[1], rp[2], rp[3]
				changed = true
			end
		end
	end

	-- build palette
	local palette = {}
	for c = 1, k do
		local r = centroids[c][1] + 0.5
		if r < 0 then
			r = 0
		elseif r > 255 then
			r = 255
		end
		local g = centroids[c][2] + 0.5
		if g < 0 then
			g = 0
		elseif g > 255 then
			g = 255
		end
		local b = centroids[c][3] + 0.5
		if b < 0 then
			b = 0
		elseif b > 255 then
			b = 255
		end
		palette[#palette + 1] = {
			math.floor(r),
			math.floor(g),
			math.floor(b),
		}
	end
	return palette
end

local function sort_by_population(pixels, palette)
	local k, counts = #palette, {}
	for c = 1, k do
		counts[c] = 0
	end
	for i = 1, #pixels do
		local p, best_c, best_d = pixels[i], 1, dist2(p, palette[1])
		for c = 2, k do
			local d = dist2(p, palette[c])
			if d < best_d then
				best_d, best_c = d, c
			end
		end
		counts[best_c] = counts[best_c] + 1
	end
	local idx = {}
	for c = 1, k do
		idx[c] = c
	end
	table.sort(idx, function(a, b)
		return counts[a] > counts[b]
	end)
	local sorted = {}
	for i = 1, k do
		sorted[i] = palette[idx[i]]
	end
	return sorted
end

function Main(image_path)
	local k, sample_size, max_iter = 16, 128, 25
	local pixels = read_pixels(image_path, sample_size)
	local palette = kmeans(pixels, k, max_iter)
	palette = sort_by_population(pixels, palette)

	while #palette > k do
		table.remove(palette)
	end
	while #palette < k do
		local last = palette[#palette]
		palette[#palette + 1] = { last[1], last[2], last[3] }
	end
	return palette
end
```

</details>


## 🤝 Contributing

Report issues, request features, or contribute via PRs. See the GitHub repository for more info.

## 📄 License

Licensed under GNU GPL v3.0 — always free and open-source.

**Star the project on GitHub if you find it useful!**


## Special Thanks

- [pywal](https://github.com/dylanaraps/pywal) by [dylanaraps](https://github.com/dylanaraps)

- [pywal16](https://github.com/eylles/pywal16) by [eylles](https://github.com/eylles)
