# Wireless thermometer receiver.

Project emobodies receiving end of this wireless thermometer project: https://www.elektroda.pl/rtvforum/topic3550281.html. Hardware related CAD files here: https://github.com/m-xor/Thermometer_hardware

File "error_codes.txt" describes error codes (if any) displayed by device on its LED display. 
File "balkonowy_rx_ult_.qm" provides model of application. It is to be opened and modified by QP Modeler: https://www.state-machine.com/qm/ Source code genearated by QM is in "qm_src" subdirectory. It is strictly advised not to edit files in this subdirectory by hand but use QM tool instead.
The rest of subdirectories contain source files and scripts not covered by QM model (e.g. device drivers etc.)
To compile project one needs QP/C RTOS framework: http://www.state-machine.com/qpc/ (ver. 6.5.1 when it was written). 
Directories of the framework compiler should know about (see Makefile for GCC in $(QPC_DIR)/examples/arm-cm subdirectories):

Includes:

* $(QPC_DIR)/include
* $(QPC_DIR)/ports/arm-cm/qv/gnu
* $(QPC_DIR)/src

Source directories:

* $(QPC_DIR)/include
* $(QPC_DIR)/src/qf
* $(QPC_DIR)/src/qv
* $(QPC_DIR)/ports/arm-cm/qv/gnu

