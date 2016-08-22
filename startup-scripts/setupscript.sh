#!/bin/bash

#Installation of required packages

#make prepare requiries
echo "Updating the system"
sudo apt-get update
echo "Installing requiries for make prepare in genode"
sudo apt-get install -y git bison gperf subversion flex


#requiries for genode make
echo "Installing requiries for genode make"
sudo apt-get install -y g++-4.9 expect pkg-config 

echo "Compiling genode"

DIRECTORY = ~/lil4

if [ ! -d "$DIRECTORY" ]; then
  mkdir ~/lil4
fi

cd ~/lil4

git clone https://github.com/702nADOS/genode.git

DIRECTORY = genode


if [ ! -d "$DIRECTORY" ]; then
  mkdir $DIRECTORY
fi

cd genode

cd repos/libports
make prepare


cd ../..
cd repos/dde_linux
make prepare


cd ../..
cd repos/base-foc
make prepare

cd ~/lil4/genode
./tool/create_builddir foc_panda BUILD_DIR=~/lil4/genode-build.foc_panda
cd ~/lil4/genode-build.foc_panda

echo "REPOSITORIES+= \$(GENODE_DIR)/repos/libports" >> etc/build.conf
echo "REPOSITORIES+= \$(GENODE_DIR)/repos/dde_linux" >> etc/build.conf

make run/demo

###################################################################

#cd ~/lil4/genode/var/run/demo
#cp -r genode image.elf modules.list <sd card>

##################################################################


echo "Preparing  hello-genode"
cd  ~/lil4/genode

mkdir repos/hello-genode
mkdir repos/hello-genode/run
mkdir repos/hello-genode/src
mkdir repos/hello-genode/src/gehello
touch repos/hello-genode/src/gehello/main.cc
touch repos/hello-genode/src/gehello/target.mk
touch repos/hello-genode/run/gehello.run



echo "TARGET = gehello" >> repos/hello-genode/src/gehello/target.mk
echo "SRC_CC = main.cc" >> repos/hello-genode/src/gehello/target.mk
echo "LIBS = base" >> repos/hello-genode/src/gehello/target.mk

cd ~/lil4/genode-build.foc_panda
echo "REPOSITORIES+= \$(GENODE_DIR)/repos/hello_genode" >> etc/build.conf

echo "Making gehello"
make gehello


echo "# Build
build { core init gehello }
create_boot_directory
# Generate config
install_config {
<config>
<parent-provides>
<service name=\"LOG\"/>
<service name=\"RM\"/>
</parent-provides>
<default-route>
<any-service> <parent/> <any-child/> </any-service>
</default-route>
<start name=\"gehello\">
<resource name=\"RAM\" quantum=\"1M\"/>
<provides><service name=\"GeHello\"/></provides>
</start>
</config>}
# Boot image
build_boot_image { core init gehello }
append qemu_args \" -nographic \"
run_genode_until \"hello!\n\" 5 " >> ~/lil4/genode/repos/hello-genode/run/gehello.run

echo "Making run/gehello"
make run/gehello


echo "Preparing enhanced gehello"

echo "TARGET = gehello" > ~/lil4/genode/repos/hello-genode/src/gehello/target.mk
echo "SRC_CC = main.cc" >> ~/lil4/genode/repos/hello-genode/src/gehello/target.mk
echo "LIBS = base stdcxx" >> ~/lil4/genode/repos/hello-genode/src/gehello/target.mk



echo "#include <base/printf.h>
#include <vector >
int main(int argc, char* argv[])
{
std::vector <int> vec(5);
PDBG("stdhello\n");
return 0;
}
" >> repos/hello-genode/src/gehello/main.cc


echo "# Build
build { core init gehello }
create_boot_directory
# Generate config
install_config {
<config>
<parent-provides>
<service name=\"LOG\"/>
<service name=\"RM\"/>
<service name=\"ROM\"/>
</parent-provides>
<default-route>
<any-service> <parent/> <any-child/> </any-service>
</default-route>
<start name=\"gehello\">
<resource name=\"RAM\" quantum=\"4M\"/>
<provides><service name=\"GeHello\"/></provides>
</start>
</config>}
# Boot image
build_boot_image { core init gehello ld.lib.so }
append qemu_args \" -nographic \"
run_genode_until \"hello!\n\" 5 " > ~/lil4/genode/repos/hello-genode/run/gehello.run

echo "Making run/gehello again"
make run/gehello

#useful tools for help see manual pages
echo "Install useful tools"
sudo apt-get install htop tree vim curl progress 

#requiries for toolchain build
echo "Installing toolchain build requiries"
sudo apt-get install autoconf autogen texinfo libexpat1-dev libncurses5-dev autoconf2.64 

#requiries for speeddreams

echo "Installing SpeedDreams requirements"
sudo apt-get install -y cmake libogg-dev libvorbis-dev libexpat-dev libjpeg-dev libplib-dev libopenal-dev libenet-dev libogg0 libvorbis0a libsdl1.2-dev

echo "Preparing SpeedDreams"
cd ~/lil4/cruise-and-park/

cd speeddreams

echo "Fixing isinf isnan bug"

echo -e "\033[31m"
echo "If isinf isnan std error occures please use this command within the folders of speeddreams
Take care if you use it twice or in the wrong folders it will destroy correct isinf isnan functions!"
echo -e "\033[00m"

sed -i 's/isinf/std::isinf/g' `grep "isinf" -rl`
sed -i 's/isnan/std::isnan/g' `grep "isnan" -rl`

DIRECTORY = build

if [ ! -d "$DIRECTORY" ]; then
  mkdir $DIRECTORY
fi

cd build

echo "Cmaking speeddreams"
cmake ..

make


#simcom

echo "Cloning Simcom"
cd ~/lil4/cruise-and-park/

cd simcom
./tool/create_builddir foc_pbxa9 BUILD_DIR=~/lil4/simcom-build

cd ~/lil4/simcom-build

echo "REPOSITORIES+= \$(GENODE_DIR)/repos/libports" >> ~/lil4/simcom-build/etc/build.conf
echo "REPOSITORIES+= \$(GENODE_DIR)/repos/dde_linux" >> ~/lil4/simcom-build/etc/build.conf
echo "REPOSITORIES+= \$(GENODE_DIR)/repos/simcom" >> ~/lil4/simcom-build/etc/build.conf
echo "REPOSITORIES+= \$(GENODE_DIR)/repos/ecu" >> ~/lil4/simcom-build/etc/build.conf

cd ~/lil4/simcom/repos/libports
make prepare

cd ../..
cd repos/dde_linux
make prepare

cd ../..
cd repos/base-foc
make prepare

cd ~/lil4/simcom-build
make simcom/sim

# vde requiries

echo "Installing VDE and required packages"

sudo apt-get install -y vde2 libvdeplug-dev libvde-dev libvde0 libvdeplug2


# qemu requiries

echo "Installing QEMU requiries"
sudo apt-get install  libpixman-1-dev libtool
sudo apt-get --no-install-recommends -y build-dep qemu
sudo apt-get install -y libfdt-dev libglib2.0-dev zlib1g-dev

cd ~/lil4/
git clone git://git.qemu-project.org/qemu.git



cd qemu
./configure --enable-vde
make -j4
sudo make install

echo "Everything is set, please continue at the point 6.4.1 in your task_description"
