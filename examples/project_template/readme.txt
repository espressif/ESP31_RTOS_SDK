This is a simple project template.

sample_lib is an example for multi-level folder Makefile, notice the folder structure and each Makefile, you can get the clue.

HOWTO:
1. Copy this folder to anywhere.
Example:
   Copy to ~/workspace/project_template
   You can rename this folder as you like.

2. Export SDK_PATH and BIN_PATH.
Example:
   Your SDK path is ~/ESP32_RTOS_SDK, and want generate bin at ~/ESP32_BIN.
   Do follow steps:
     1>. export SDK_PATH=~/ESP32_RTOS_SDK
     2>. export BIN_PATH=~/ESP32_BIN
   SDK and project are seperate, you can update SDK without change your project.

3. Enter project_template folder, run ./gen_misc.sh.

