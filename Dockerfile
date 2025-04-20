# FROM ubuntu
FROM continuumio/miniconda3:main

# essential things
RUN <<EOF 
apt update
apt install -y build-essential cmake nano git wget curl unzip
EOF

WORKDIR /root
# inih for reading .ini file
RUN apt install -y libinih-dev

# hdf5 for better output format
RUN <<EOF
export HDF5_VERSION=1.14.5
wget https://github.com/HDFGroup/hdf5/releases/download/hdf5_${HDF5_VERSION}/hdf5-${HDF5_VERSION}.tar.gz
tar -xvzf hdf5-${HDF5_VERSION}.tar.gz
rm hdf5-${HDF5_VERSION}.tar.gz
cd hdf5-${HDF5_VERSION}
cmake -B build \
-DCMAKE_BUILD_TYPE=Release \
-DHDF5_ENABLE_ZLIB_SUPPORT=Off \
-DHDF5_ENABLE_SZIP_SUPPORT=Off
cmake --build build --parallel
cmake --install build --prefix /usr/local
EOF

# highfive for easier hdf5 usage
RUN <<EOF
export HIGHFIVE_VERSION=2.10.1
wget https://github.com/BlueBrain/HighFive/archive/refs/tags/v${HIGHFIVE_VERSION}.tar.gz -O highfive-${HIGHFIVE_VERSION}.tar.gz
tar -xvzf highfive-${HIGHFIVE_VERSION}.tar.gz
rm highfive-${HIGHFIVE_VERSION}.tar.gz
cd HighFive-${HIGHFIVE_VERSION}
cmake -B build \
-DHIGHFIVE_USE_BOOST=Off \
-DHIGHFIVE_EXAMPLES=Off \
-DHIGHFIVE_USE_BOOST=Off \
-DHIGHFIVE_UNIT_TESTS=Off
cmake --build build --parallel
cmake --install build --prefix /usr/local
EOF

# gridformat for generating vtk files for paraview visualization
# it's better to have highfive install
RUN <<EOF
# export GRIDFORMAT_VERSION=0.3.0
# wget https://github.com/dglaeser/gridformat/archive/refs/tags/v${GRIDFORMAT_VERSION}.tar.gz -O gridformat-${GRIDFORMAT_VERSION}.tar.gz
# tar -xvzf gridformat-${GRIDFORMAT_VERSION}.tar.gz
# rm gridformat-${GRIDFORMAT_VERSION}.tar.gz
# cd gridformat-${GRIDFORMAT_VERSION}
git clone https://github.com/dglaeser/gridformat.git
cd gridformat
git checkout cleanup/api-unavailable-writer-error
cmake -B build
cmake --build build --parallel
cmake --install build --prefix /usr/local
EOF


# Kokkos HPC framework
RUN <<EOF
export KOKKOS_VERSION=4.5.01
wget https://github.com/kokkos/kokkos/releases/download/${KOKKOS_VERSION}/kokkos-${KOKKOS_VERSION}.tar.gz
tar -xvzf kokkos-${KOKKOS_VERSION}.tar.gz
rm kokkos-${KOKKOS_VERSION}.tar.gz
cd kokkos-${KOKKOS_VERSION}
cmake -B build \
-DCMAKE_CXX_COMPILER=g++ \
-DCMAKE_BUILD_TYPE=Release \
-DKokkos_ENABLE_OPENMP=On \
-DKokkos_ARCH_NATIVE=On \
-DKokkos_ENABLE_DEPRECATED_CODE_4=Off
cmake --build build --parallel
cmake --install build --prefix /usr/local
EOF
# set these env vars to optimize OPENMP 4.0 performance
ENV OMP_PROC_BIND=spread
ENV OMP_PLACES=threads

# Neovim
RUN <<EOF
# install lua5.1 for luarock
apt install -y liblua5.1-0-dev
# install python3 and unzip for some lsp installation
# wget -qO- https://astral.sh/uv/install.sh | sh
# uv python install --default --preview
# uv venv ~/.venv/nvim
# export PATH="$HOME/.venv/nvim/bin:$PATH"
# install imagemagick for image preview
apt install -y libmagickwand-dev
# install nodejs and npm for copilot 
curl -fsSL https://deb.nodesource.com/setup_20.x | bash - && apt install -y nodejs
# install neovim
wget https://github.com/neovim/neovim/releases/latest/download/nvim-linux-x86_64.appimage
chmod +x nvim-linux-x86_64.appimage
./nvim-linux-x86_64.appimage --appimage-extract
mv squashfs-root /
ln -s /squashfs-root/AppRun /usr/bin/nvim
# config
mkdir ~/.config
cd ~/.config
git clone https://github.com/HuntFeng/nvim.git
rm nvim/lua/plugins/image.lua
EOF
