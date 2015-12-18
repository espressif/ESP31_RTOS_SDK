# ESP32_RTOS_SDK #

ESP32 SDK based on FreeRTOS.

## Toolchain ##

We suggest to choose **Crosstool-ng** as the compiler toolchain. Follow the instructions below to install Crosstool-ng.

Step 1. Install the required toolchain packages.
```
sudo apt-get install git autoconf build-essential gperf bison flex texinfo libtool libncurses5-dev wget gawk libc6-dev-i386 python-serial libexpat-dev
```
Step 2. Create a directory (e.g./opt/Espressif) to store the toolchain. 
```	
sudo mkdir /opt/Espressif
```
Step 3. Make the current user the owner.
```
sudo chown $USER /opt/Espressif/
```
Step 4. Download the latest toolchain installation file to the directory created in Step 2.
```
cd /opt/Espressif/
git clone -b esp108-1.21.0 git://github.com/jcmvbkbc/crosstool-NG.git
```
Step 5. Install toolchain.
```
cd crosstool-NG
./bootstrap && ./configure --prefix=`pwd` && make && make install 
./ct-ng xtensa-esp108-elf 
./ct-ng build
```
Step 6. Set the PATH variable to point to the newly compiled toolchain. 
```
export PATH=/opt/Espressif/crosstool-NG/builds/xtensa-esp108-elf/bin:$PATH
```
> Note： 
You need to do Step 6 once you open a new shell, or you can put it inside your .bashrc file
  
## Project template Compile ##

Step 1. Create a directory (e.g.~/Workspace) to store a new project.
```
mkdir ~/Workspace
```
Step 2. Clone ESP32 RTOS SDK.
```
cd ~/Workspace
git clone https://github.com/espressif/ESP32_RTOS_SDK.git
```
Step 3. Copy ESP32_RTOS_SDK/examples/project_template to Workspace directory created in Step 1.
```
cp ~/Workspace/ESP32_RTOS_SDK/examples/project_template ~/Workspace/ -r
```
Step 4. Create a directory (e.g.~/Workspace/ESP32_BIN) to store the bin files compiled.
```
mkdir –p ~/Workspace/ESP32_BIN
```
Step 5. Set SDK_PATH as the path of SDK files and BIN_PATH as the path of .bin files compiled.
```
export SDK_PATH=~/Workspace/ESP32_RTOS_SDK 
export BIN_PATH=~/Workspace/ESP32_BIN
```
> Notice： 
> Make sure you set the correct paths, or it will occur a compile error.

Step 6. Start to compile files
```
cd ~/Workspace/project_template
make clean
make
```
> Note： 
You need to do Step 3 every time you open a new shell, or you can put it inside your .bashrc file.

If your project is successfully compiled, the irom1.bin, irom0_flash.bin, and user.ota files will 
be generated in ~/Workspace/ESP32_BIN directory. 

## Download ##

Please use espressif's **Flash Download tools**.

Download addresses:
```
boot.bin---------->0x00000
irom1.bin--------->0x04000
irom0_flash.bin--->0x40000
blank.bin--------->0xfe000(for 1MB SPI Flash)
```
For more details, please refer to http://www.esp32.com
