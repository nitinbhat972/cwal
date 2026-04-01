#
#  cwal: Blazing-fast pywal-like color palette generator written in C.
#  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
#  Repository: https://github.com/nitinbhat972/cwal
#
#  Licensed under the GNU General Public License v3.0.
#  If you find this code useful, please consider giving it a star on GitHub!
#  Any contributions or forks must retain this original header.
#

complete -c cwal -f

function __fish_cwal_themes
    set -l themes random_dark random_light random_all
    
    set -l config_home "$XDG_CONFIG_HOME"
    if test -z "$config_home"
        set config_home "$HOME/.config"
    end
    
    set -l data_home "$XDG_DATA_HOME"
    if test -z "$data_home"
        set data_home "$HOME/.local/share"
    end

    set -l search_dirs "$config_home/cwal/themes" "$data_home/cwal/themes"
    
    set -l xdg_dirs "$XDG_DATA_DIRS"
    if test -z "$xdg_dirs"
        set xdg_dirs "/usr/local/share:/usr/share"
    end
    
    for dir in (string split : "$xdg_dirs")
        set -a search_dirs "$dir/cwal/themes"
    end

    for base in $search_dirs
        for mode in dark light
            set -l dir "$base/$mode"
            if test -d "$dir"
                for theme in (ls "$dir" | sed 's/\.cwal$//')
                    echo $theme
                end
            end
        end
    end
    for t in $themes
        echo $t
    end
end

function __fish_cwal_backends
    echo cwal
    echo libimagequant
    
    set -l config_home "$XDG_CONFIG_HOME"
    if test -z "$config_home"
        set config_home "$HOME/.config"
    end
    
    set -l dir "$config_home/cwal/backends"
    if test -d "$dir"
        for script in (ls "$dir" | sed 's/\.lua$//')
            echo $script
        end
    end
end

complete -c cwal -s m -l mode -d "Set theme mode (required: <dark|light>)" -r -xa "dark light"
complete -c cwal -s c -l cols16-mode -d "Set 16-color mode (required: <darken|lighten>)" -r -xa "darken lighten"
complete -c cwal -s s -l saturation -d "Overall saturation (required: <float>)" -r
complete -c cwal -s C -l contrast -d "Contrast ratio (required: <float>)" -r
complete -c cwal -s a -l alpha -d "Alpha transparency (required: <float>)" -r
complete -c cwal -s o -l out-dir -d "Output directory (required: <path>)" -r -xa "(__fish_complete_directories)"
complete -c cwal -s b -l backend -d "Processing backend (required: <name>)" -r -xa "(__fish_cwal_backends)"
complete -c cwal -s i -l img -d "Specify image path (required: <path>)" -r -xa "(__fish_complete_suffix .jpg .jpeg .png .gif .webp)"
complete -c cwal -s S -l script -d "Run custom script (required: <path>)" -r
complete -c cwal -s t -l theme -d "Select a theme (required: <name>)" -r -xa "(__fish_cwal_themes)"
complete -c cwal -s r -l random -d "Select random image (optional: [directory])" -xa "(__fish_complete_directories)"
complete -c cwal -s n -l no-reload -d "Disable reloading"
complete -c cwal -s B -l list-backends -d "List available backends"
complete -c cwal -s T -l list-themes -d "List available themes"
complete -c cwal -s q -l quiet -d "Suppress all output"
complete -c cwal -s p -l preview -d "Preview palette"
complete -c cwal -s v -l version -d "Show version"
complete -c cwal -s h -l help -d "Display help"
