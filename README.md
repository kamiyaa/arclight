# arclight

## Description
Similar to [acpilight](https://github.com/wavexx/acpilight), but written in C and requires user to specify an interface
Why C? Because I don't want programs holding back python system updates.

## Installation
First, download the source using either HTTP or SSH: 
```
$ git clone https://github.com/Kamiyaa/arclight.git
$ git clone git@github.com:Kamiyaa/arclight.git
```
Then install using **meson**:
```
$ meson build
$ ninja -C build/
# ninja -C build/ install
```

## Usage
```
$ arclight -i intel_backlight -u 5  # increase brightness by 5%
$ arclight -i intel_backlight -d 5  # decrease brightness by 5%
$ arclight -i intel_backlight -s 255  # set brightness to 255 (vendor specific)
```

