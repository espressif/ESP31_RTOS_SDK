This is a simple OTA project demo code.

HOWTO:
1. Copy this folder to anywhere.
Example:
   Copy to ~/workspace/ota_demo
   You can rename this folder as you like.

2. Export SDK_PATH and BIN_PATH.
Example:
   Your SDK path is ~/ESP32_RTOS_SDK, and want generate bin at ~/ESP32_BIN.
   Do follow steps:
     1>. export SDK_PATH=~/ESP32_RTOS_SDK
     2>. export BIN_PATH=~/ESP32_BIN
   SDK and project are seperate, you can update SDK without change your project.

3. Enter ota_demo folder, run ./gen_misc.sh.

4. if there are no errors in compiler process ,you will generate three binary system files in you BIN_PATH(export in step 2)
   irom1.bin ,irom0_flash.bin ,user.ota. Do follow steps
     1>. put user.ota in you remote server （you can use a opensource server ,like apache，MINIwebserver）
     2>. use flash download tools download these bin files
		 boot.bin        ------ 0x00000
		 irom1.bin       ------ 0x04000
		 irom0_flash.bin ------ 0x40000
		 blank.bin       ------ last two sectors as you flash (for example :1MB ---- 0xFE000 ,2MB ---- 0x1FE000)
		 
5. you can change the config info in user_main.c 
	if define SYMMETRY_STRUCTURE_MODE = 1
	you will user a OTA mode like ESP8266.
	 userbin 0 is running -> connect to server -> download userbin1 in latter half part of flash -> jump to run userbin1
		↑																						               ↓			
	jump to run userbin0  <- download userbin0 in first half part of flash  <- connect to server <- userbin1 is running 
	if define SYMMETRY_STRUCTURE_MODE = 0 
	you can config the bin id (bin_id <= 4) and start flash id by youself
	
	for example：
	system_upgrade_init(3,5)
	you will download a bin file defined by 3 in addr 5 * (256*1024) = 0x14000, and you can call api system_reboot_to_userbin(3)
	reboot to userbin3;
	
	in the demo code ,flash size is 2MB ,3 bin files run in turns,userbin 0 download userbin1,userbin1 download 
	userbin2,userbin2 download userbin0	
