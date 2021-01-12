# Matterialize

Matterialize is a unified development framework to create, test, and deploy background matting techniques on Linux,
packaged with a modern and comprehensive graphical user interface. It has an open architecture for integration with
third-party applications and we have already integrated three different examples of matting strategies to showcase the
capabilities of the platform, one of which performs sufficiently well to merit real-world usage.

## Installation

Assuming git, make, clang, gcc, cmake, zip, unzip, fakeroot and dpkg are installed Run

```
git clone --recurse git@gitlab.doc.ic.ac.uk:g17/matterialize.git
cd matterialize
make install
```

To install only the frontend, or only the backend run `make install matterialize` or `make install-matterialize-server`
respectively. To uninstall the application run `make uninstall`.

## Running the application

Both the frontend and the backend will start simultaneously by running the `matterialize` binary.

## Developing a matter

The source files for your matting algorithm can be added anywhere under `src/files`, but we recommend `src/files/matting` as a more
appropriate location. Then

1. declare any configuration fields (IMatterConfigField) you wish to expose for updates by end users
2. implement the IMatter interface
3. implement theIMatterMode interface

To make the mode available within Matterialize, you should also instantiate it statically as as `constexpr`
in `src/files/matting/modes.hpp` and add it to the `modes` array. You might want to look at the already integrated
matting modes as an example.

Then recomplie and install by running `make install-matterialize-server`