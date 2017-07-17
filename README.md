# ESP32_RTOS_SDK

ESP32 SDK based on FreeRTOS.

## Toolchain

We suggest to choose **Crosstool-ng** as the compiler toolchain. Follow the instructions below to install Crosstool-ng.

Step 1. Install the required toolchain packages.
```
sudo apt-get install git autoconf build-essential gperf bison flex texinfo libtool libncurses5-dev wget gawk libc6-dev-i386 python-serial libexpat-dev libtool-bin
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
> Notice： 
> You need to do Step 6 once you open a new shell, or you can put it inside your .bashrc file
  
## Project template Compile

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
> `export BIN_PATH` is optional, default BIN_PATH is `./bin`.

Step 6. Start to compile files
```
cd ~/Workspace/project_template
make clean
make
```
> Notice： 
> You need to do Step 5 every time you open a new shell, or you can put it inside your .bashrc file.

If your project is successfully compiled, the drom0.bin, irom0_flash.bin, and user.ota files will 
be generated in BIN_PATH directory. 

## Makefile Options
esp.conf in project_template includes makefile supported options, you can modify according to your needs.

### 1. esptool parameters
* serial port, depend on your platform, default /dev/ttyUSB0

	```
	ESP_PORT = /dev/ttyUSB0
	```

* serial baudrate, default 230400

	```
	ESP_BAUD = 230400
	```

* spi flash size: 1MB/2MB/4MB/8MB/16MB, default 1MB

	```
	ESP_FS = 1MB
	```

* spi flash freq: 40m/26m/20m/80m, default 40m

	```
	ESP_FF = 40m
	```

* spi flash mode: qio/qout/dio/dout, default qio

	```
	ESP_FM = qio
	```

### 2. compile & link parameters
* ld folder location choose: 0/1, default 0
	- 0: use ld folder at sdk.
	- 1: use local ld folder at your project.

	> Notice: 
	> you can copy ld folder from sdk to your project, then do some modifications.

	```
	ESP_LOCAL_LD = 1
	```

* ld file name
	> Notice:
	> if ESP_LOCAL_LD is set to 1, ESP_LD_NAME MUST be set.

	```
	ESP_LD_FILE = pro.map.ld
	```

* ram map mode, default 1.1
	- 1.1: not use app cpu's iram, use drom0(dcache) to store rodata.
	- 1.2: not use app cpu's iram, not use drom0(dcache).
	- 2.1: use all app cpu's iram as pro cpu's dram, use drom0(dcache) to store rodata.
	- 2.2: use all app cpu's iram as pro cpu's dram, not use drom0(dcache).
	- 3.1: reverse 16K for app cpu, other as pro cpu's dram, use drom0(dcache) to store rodata.
	- 3.2: reverse 16K for app cpu, other as pro cpu's dram, not use drom0(dcache).

	> Notice:
	> if ESP_LOCAL_LD is set to 0, ESP_MAP_MODE CAN be set.

	```
	ESP_MAP_MODE = 1.1
	```


## Download

### 1. Use Espressif's 'Flash Download tools'

Download addresses:

* For map 1.1/2.1/3.1

	```
	boot.bin---------->0x00000
	drom0.bin--------->0x04000
	irom0_flash.bin--->0x40000
	blank.bin--------->0xfe000(for 1MB SPI Flash)
	```

* For map 1.2/2.2/3.2

	```
	boot.bin---------->0x00000
	irom0_flash.bin--->0x04000
	blank.bin--------->0xfe000(for 1MB SPI Flash)
	``` 

### 2. Use esptool.py
Prepare: modify esptool options in esp.conf.

makefile supports targets:

* flash_all : upload boot.bin/app.bin/blank.bin

	```
	make flash_all
	```

* flash_boot : upload boot.bin

	```
	make flash_boot
	```
	> Notice:
	> only needed when boot.bin update

* flash_app : update user application (drom0.bin/irom0_flash.bin)

	```
	make flash_app
	```

* flash_blank : update blank.bin to system parameter address

	```
	make flash_blank
	```
	> Notice:
	> only needed when the system parameters needed to be restored

For more details, please refer to http://www.esp32.com
