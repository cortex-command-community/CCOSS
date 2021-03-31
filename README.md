# Cortex Command Community Project Source
*The Cortex Command Community Project is Free/Libre and Open Source under GNU AGPL v3*

This is a community-driven effort to continue the development of Cortex Command.  
Stay up to date in our [Discord channel](https://discord.gg/TSU6StNQUG).

***

# Windows Build Instructions
First you need to download the necessary files:

1. Install the necessary tools.  
You'll probably want [Visual Studio Community Edition 2019](https://visualstudio.microsoft.com/downloads/) with the Clang-C++ toolset installed (select Clang-C++ for Windows in the installer).  
You also need to have both x86 and x64 versions of the [Visual C++ Redistributable for Visual Studio 2017](https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads) installed in order to run the compiled builds.  
You may also want to check out the list of recommended Visual Studio plugins [here](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/wiki/Information,-Recommended-Plugins-and-Useful-Links).

2. Clone this Source Repository and the [Data Repository](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Data) in neighboring folders.  
**Do Not** change the folder names unless you want to make trouble for yourself.

3. Copy the following libraries from `Cortex-Command-Community-Project-Source\external\lib\` into the **Data Repository**:
* `lua51.dll`
* `lua51-64.dll`
* `fmod.dll`
* `fmodL.dll`

4. Copy `Scenes.rte` and `Metagames.rte` from your purchased copy of Cortex Command into the **Data Repository**.

Now you're ready to build and launch the game.  
Simply open `RTEA.sln` with Visual Studio, choose your target platform (x86 or x64) and configuration, and run the project.

* Use `Debug Full` for debugging with all visual elements enabled (builds fast, runs very slow).
* Use `Debug Minimal` for debugging with all visual elements disabled (builds fast, runs slightly faster).
* Use `Debug Release` for a debugger-enabled release build (builds slow, runs almost as fast as Final).
* Use `Final` to build release executable.

The first build will take a while, but future ones should be quicker.

# Linux Build Instructions
The Linux build uses the meson build system, and builds against system libraries.

## Dependencies:

* `g++>=8.1` (needs to support c++17 filesystem)
* `allegro4`
* `loadpng`
* `flac`
* `luajit`
* `minizip`
* `lz4`
* `libpng`
* `libX11`
* `meson>=0.49` (If your distro doesn't have a recent version of meson, use the pip version instead)
* `boost>=1.55`
* `xorg-misc-fonts`

## Building:

1. Install Dependencies (see below for some distro-specific instructions).

2. Clone this Source Repository and the [Data Respository](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Data).

3. Open a terminal in the Source Repository.

4. `meson build` or `meson --buildtype debug build` for debug build (default is release build)

5. `cd build`

6. `ninja`

If you want to change the buildtype aftwerwards, you can use `meson configure --buildtype {release or debug}` in the build directory or create a secondary build directory as in Step 4


## Running:

1. Copy (or link, might be preferable for testing builds) `builddir/CCCP_debug.x86_64` or `builddir/CCCP.x86_64` (depending on if you made a release build) into the **Data Repository**.

2. Copy all `libfmod` files from `external/lib/linux/x86_64` into the **Data Repository**.

3. Copy `Scenes.rte` and `Metagames.rte` from your purchased copy of Cortex Command into **Data Repository**.

4. Run `./CCCP.x86_64` or `./CCCP_debug.x86_64` in the **Data Repository**.

## Troubleshooting:
Until borderless windows are implemented, you might seem get stuck in fullscreen mode.
Try Alt-Return, or if that doesn't work kill CC with ctrl-alt-end.

On X11 media keys and such don't work in fullscreen, this is a known issue (this does not happen on Wayland).

## Installing Dependencies

**Arch Linux:**  
`# pacman -S allegro4 flac luajit minizip lz4 libpng libx11 meson xorg-fonts-misc`

**Ubuntu:**  
`# apt-get install liballegro4-dev libloadpng4-dev libflac++-dev luajit-5.1-dev libminizip-dev liblz4-dev libpng++-dev libx11-dev`  
`# pip install meson`

***

# More Information

For more information and recommendations, see [here](https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/wiki/Information,-Recommended-Plugins-and-Useful-Links).
