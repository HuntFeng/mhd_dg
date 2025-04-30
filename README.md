# Discontinuous Galerkin Method for Ideal MHD

## Set Up Environment

- Docker is required for creating isolated and stable environment.

1. Build the docker image using the command

```bash
docker buildx build --platform linux/amd64 -t mhd_dg:amd64 --load .
```

This way we make sure the architecture is `x86` and `clangd` for neovim can be installed without issues.

2. Start the container as daemon

```bash
docker compose up -d
```

3. Start and/or attach to the existing container daemon

```bash
docker start -ai mhd_dg
```

## Setup Neovim (Optional)

1. In any directory, start `neovim` by

```bash
nvim
```

Plugins will be installed if you have neovim configs under `~/.config/nvim` on your computer, this config folder will be mounted to the container so we don't need to install plugins again.

2. Connect Copilot (Optional)
   In Neovim, type command `:Copilot`, then copy one-time passcode to GitHyb's device login page, https://github.com/login/device. Then it's good to go.

## Build Project

- CMake is used for building this C++ project.

```bash
cmake -B build
cmake --build build
```
