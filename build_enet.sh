# CD into enet dir

cd src/enet

# Generate the build system.

autoreconf -vfi

# Compile and install the library.

./configure && make && make install
