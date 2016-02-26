#!/usr/bin/env python
#
# ESP8266 ROM Bootloader Utility
# https://github.com/themadinventor/esptool
#
# Copyright (C) 2014 Fredrik Ahlberg
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
# Street, Fifth Floor, Boston, MA 02110-1301 USA.

import sys
import struct
import serial
import time
import argparse
import os
import subprocess
import tempfile


class ESPROM:
    # These are the currently known commands supported by the ROM
    ESP_FLASH_BEGIN = 0x02
    ESP_FLASH_DATA  = 0x03
    ESP_FLASH_END   = 0x04
    ESP_MEM_BEGIN   = 0x05
    ESP_MEM_END     = 0x06
    ESP_MEM_DATA    = 0x07
    ESP_SYNC        = 0x08
    ESP_WRITE_REG   = 0x09
    ESP_READ_REG    = 0x0a
    ESP_SPI_FLASH_SET = 0xb

    # Maximum block sized for RAM and Flash writes, respectively.
    ESP_RAM_BLOCK   = 0x1800
    ESP_FLASH_BLOCK = 0x400

    # Default baudrate. The ROM auto-bauds, so we can use more or less whatever we want.
    ESP_ROM_BAUD    = 115200

    # First byte of the application image
    ESP_IMAGE_MAGIC = 0xe9

    # Initial state for the checksum routine
    ESP_CHECKSUM_MAGIC = 0xef

    # OTP ROM addresses
    ESP_OTP_MAC0    = 0x3ff00050
    ESP_OTP_MAC1    = 0x3ff00054
    ESP_OTP_MAC2    = 0x3ff00058
    ESP_OTP_MAC3    = 0x3ff0005c
    
    EFUSE_ESP32_RDATA16 = 0x6001a000+4*16
    EFUSE_ESP32_RDATA17 = 0x6001a000+4*17
    EFUSE_ESP32_RDATA18 = 0x6001a000+4*18
    EFUSE_ESP32_RDATA19 = 0x6001a000+4*19    
    

    # Sflash stub: an assembly routine to read from spi flash and send to host
    SFLASH_STUB     = "\x80\x3c\x00\x40\x1c\x4b\x00\x40\x21\x11\x00\x40\x00\x80" \
                      "\xfe\x3f\xc1\xfb\xff\xd1\xf8\xff\x2d\x0d\x31\xfd\xff\x41\xf7\xff\x4a" \
                      "\xdd\x51\xf9\xff\xc0\x05\x00\x21\xf9\xff\x31\xf3\xff\x41\xf5\xff\xc0" \
                      "\x04\x00\x0b\xcc\x56\xec\xfd\x06\xff\xff\x00\x00"

    def __init__(self, port=0, baud=ESP_ROM_BAUD,chip = "ESP8266"):
        self._port = serial.Serial(port)
        # setting baud rate in a separate step is a workaround for
        # CH341 driver on some Linux versions (this opens at 9600 then
        # sets), shouldn't matter for other platforms/drivers. See
        # https://github.com/themadinventor/esptool/issues/44#issuecomment-107094446
        self._port.baudrate = baud
        self.CHIP = chip

    """ Read bytes from the serial port while performing SLIP unescaping """
    def read(self, length=1):
        b = ''
        while len(b) < length:
            c = self._port.read(1)
            if c == '\xdb':
                c = self._port.read(1)
                if c == '\xdc':
                    b = b + '\xc0'
                elif c == '\xdd':
                    b = b + '\xdb'
                else:
                    raise FatalError('Invalid SLIP escape')
            else:
                b = b + c
        return b

    """ Write bytes to the serial port while performing SLIP escaping """
    def write(self, packet):
        buf = '\xc0' \
              + (packet.replace('\xdb','\xdb\xdd').replace('\xc0','\xdb\xdc')) \
              + '\xc0'
        self._port.write(buf)

    """ Calculate checksum of a blob, as it is defined by the ROM """
    @staticmethod
    def checksum(data, state=ESP_CHECKSUM_MAGIC):
        for b in data:
            state ^= ord(b)
        return state

    """ Send a request and read the response """
    def command(self, op=None, data=None, chk=0):
        if op:
            pkt = struct.pack('<BBHI', 0x00, op, len(data), chk) + data
            self.write(pkt)

        # tries to get a response until that response has the
        # same operation as the request or a retries limit has
        # exceeded. This is needed for some esp8266s that
        # reply with more sync responses than expected.
        retries = 100
        while retries > 0:
            (op_ret, val, body) = self.receive_response()
            if op is None or op_ret == op:
                return val, body  # valid response received
            retries = retries - 1

        raise FatalError("Response doesn't match request")

    """ Receive a response to a command """
    def receive_response(self):
        # Read header of response and parse
        if self._port.read(1) != '\xc0':
            raise FatalError('Invalid head of packet')
        hdr = self.read(8)
        (resp, op_ret, len_ret, val) = struct.unpack('<BBHI', hdr)
        if resp != 0x01:
            raise FatalError('Invalid response 0x%02x" to command' % resp)

        # The variable-length body
        body = self.read(len_ret)
        if self.CHIP=="ESP32" and len_ret == 2 and body[0] == '\0':
            body = '\0\0'

        # Terminating byte
        if self._port.read(1) != chr(0xc0):
            raise FatalError('Invalid end of packet')

        return op_ret, val, body

    """ Perform a connection test """
    def sync(self):
        self.command(ESPROM.ESP_SYNC, '\x07\x07\x12\x20' + 32 * '\x55')
        for i in xrange(7):
            self.command()

    """ Try connecting repeatedly until successful, or giving up """
    def connect(self):
        print 'Connecting...'

        for _ in xrange(4):
            # issue reset-to-bootloader:
            # RTS = either CH_PD or nRESET (both active low = chip in reset)
            # DTR = GPIO0 (active low = boot to flasher)
            self._port.setDTR(False)
            self._port.setRTS(True)
            time.sleep(0.05)
            self._port.setDTR(True)
            self._port.setRTS(False)
            time.sleep(0.05)
            self._port.setDTR(False)

            # worst-case latency timer should be 255ms (probably <20ms)
            self._port.timeout = 0.3
            for _ in xrange(4):
                try:
                    self._port.flushInput()
                    self._port.flushOutput()
                    self.sync()
                    self._port.timeout = 5
                    return
                except:
                    time.sleep(0.05)
        raise FatalError('Failed to connect to ' + self.CHIP)

    """ Read memory address in target """
    def read_reg(self, addr):
        res = self.command(ESPROM.ESP_READ_REG, struct.pack('<I', addr))
        if res[1] != "\0\0":
            raise FatalError('Failed to read target memory')
        return res[0]

    """ Write to memory address in target """
    def write_reg(self, addr, value, mask, delay_us=0):
        if self.command(ESPROM.ESP_WRITE_REG,
                        struct.pack('<IIII', addr, value, mask, delay_us))[1] != "\0\0":
            raise FatalError('Failed to write target memory')

    """ Start downloading an application image to RAM """
    def mem_begin(self, size, blocks, blocksize, offset):
        if self.command(ESPROM.ESP_MEM_BEGIN,
                        struct.pack('<IIII', size, blocks, blocksize, offset))[1] != "\0\0":
            raise FatalError('Failed to enter RAM download mode')

    """ Send a block of an image to RAM """
    def mem_block(self, data, seq):
        if self.command(ESPROM.ESP_MEM_DATA,
                        struct.pack('<IIII', len(data), seq, 0, 0) + data,
                        ESPROM.checksum(data))[1] != "\0\0":
            raise FatalError('Failed to write to target RAM')

    """ Leave download mode and run the application """
    def mem_finish(self, entrypoint=0):
        if self.command(ESPROM.ESP_MEM_END,
                        struct.pack('<II', int(entrypoint == 0), entrypoint))[1] != "\0\0":
            raise FatalError('Failed to leave RAM download mode')

    """ Start downloading to Flash (performs an erase) """
    def flash_begin(self, size, offset):
        old_tmo = self._port.timeout
        num_blocks = (size + ESPROM.ESP_FLASH_BLOCK - 1) / ESPROM.ESP_FLASH_BLOCK
        
        if self.CHIP == "ESP8266":
            sectors_per_block = 16
            sector_size = 4096
            num_sectors = (size + sector_size - 1) / sector_size
            start_sector = offset / sector_size
    
            head_sectors = sectors_per_block - (start_sector % sectors_per_block)
            if num_sectors < head_sectors:
                head_sectors = num_sectors
    
            if num_sectors < 2 * head_sectors:
                erase_size = (num_sectors + 1) / 2 * sector_size
            else:
                erase_size = (num_sectors - head_sectors) * sector_size
        elif self.CHIP == "ESP32":
            erase_size = size
            if erase_size>0:
                self.flash_spi_param_set()

        self._port.timeout = 20
        t = time.time()
        
        
        result = self.command(ESPROM.ESP_FLASH_BEGIN,
                              struct.pack('<IIII', erase_size, num_blocks, ESPROM.ESP_FLASH_BLOCK, offset))[1]
        if size != 0:
            print "Took %.2fs to erase flash block" % (time.time() - t)
        
        if result != "\0\0":
            raise FatalError.WithResult('Failed to enter Flash download mode (result "%s")', result)
        self._port.timeout = old_tmo

    """ Write block to flash """
    def flash_block(self, data, seq):
        result = self.command(ESPROM.ESP_FLASH_DATA, struct.pack('<IIII', len(data), seq, 0, 0) + data, ESPROM.checksum(data))[1]
        if result != "\0\0":
            raise FatalError.WithResult('Failed to write to target Flash after seq %d (got result %%s)' % seq, result)

    """ Leave flash mode and run/reboot """
    def flash_finish(self, reboot=False):
        pkt = struct.pack('<I', int(not reboot))
        if self.command(ESPROM.ESP_FLASH_END, pkt)[1] != "\0\0":
            raise FatalError('Failed to leave Flash mode')

    """ Run application code in flash """
    def run(self, reboot=False):
        # Fake flash begin immediately followed by flash end
        self.flash_begin(0, 0)
        self.flash_finish(reboot)

    """ Read MAC from OTP ROM """
    def read_mac(self):
        if self.CHIP == "ESP8266":
            mac0 = self.read_reg(self.ESP_OTP_MAC0)
            mac1 = self.read_reg(self.ESP_OTP_MAC1)
            mac2 = self.read_reg(self.ESP_OTP_MAC2)
            mac3 = self.read_reg(self.ESP_OTP_MAC3)
            efuse = (mac3<<96)|(mac2<<64)|(mac1<<32)|(mac0<<0)
            
            chip_type = (efuse>>77)&0x7
            if chip_type == 0x5: #ESP8266EX
                print("THIS IS ESP8266EX")
            elif chip_type == 0x4: #ESP8266
                print("THIS IS ESP8266")
            else:
                print("OTHER CHIPS...")
                return None
            
            mac_flag = (efuse>>76)&0x1
            if mac_flag == 0x0: #a fixed 3 bytes MAC header + 3 bytes MAC
                mac_map = (efuse>>48)&0xff
                if mac_map == 0:
                    oui = (0x18, 0xfe, 0x34)
                elif mac_map == 1:
                    oui = (0xac, 0xd0, 0x74)
                else:
                    raise FatalError("Unknown OUI")
                return oui + ((efuse >> 40) & 0xff, (efuse>>32) & 0xff, (efuse >> 24) & 0xff)
            elif mac_flag == 0x1: # a full 48 bit MAC address format
                return ((efuse>>112)&0xff,(efuse>>104)&0xff,(efuse>>96)&0xff,(efuse>>40)&0xff,(efuse>>32)&0xff,(efuse>>24)&0xff)
            else:
                print "UNKNOW MAC MAPPING..."
                return None
        elif self.CHIP == "ESP32":
            word16 = self.read_reg(self.EFUSE_ESP32_RDATA16)
            word17 = self.read_reg(self.EFUSE_ESP32_RDATA17)
            word18 = self.read_reg(self.EFUSE_ESP32_RDATA18)
            word19 = self.read_reg(self.EFUSE_ESP32_RDATA19)            
            WiFi_MAC = (((word17>>16)&0xff),((word17>>8)&0xff),((word17>>0)&0xff),((word16>>24)&0xff),((word16>>16)&0xff),((word16>>8)&0xff))
            BT_MAC = (((word19>>16)&0xff),((word19>>8)&0xff),((word19>>0)&0xff),((word18>>24)&0xff),((word18>>16)&0xff),((word18>>8)&0xff))
            return (WiFi_MAC,BT_MAC)

    """ Read SPI flash manufacturer and device id """
    def flash_id(self):
        if self.CHIP == "ESP32":
            return 0
        elif self.CHIP == 'ESP8266':
            self.flash_begin(0, 0)
            self.write_reg(0x60000240, 0x0, 0xffffffff)
            self.write_reg(0x60000200, 0x10000000, 0xffffffff)
            flash_id = self.read_reg(0x60000240)
            self.flash_finish(False)
            return flash_id

    """ Read SPI flash """
    def flash_read(self, offset, size, count=1):
        # Create a custom stub
        stub = struct.pack('<III', offset, size, count) + self.SFLASH_STUB

        # Trick ROM to initialize SFlash
        self.flash_begin(0, 0)

        # Download stub
        self.mem_begin(len(stub), 1, len(stub), 0x40100000)
        self.mem_block(stub, 0)
        self.mem_finish(0x4010001c)

        # Fetch the data
        data = ''
        for _ in xrange(count):
            if self._port.read(1) != '\xc0':
                raise FatalError('Invalid head of packet (sflash read)')

            data += self.read(size)

            if self._port.read(1) != chr(0xc0):
                raise FatalError('Invalid end of packet (sflash read)')

        return data

    """ Abuse the loader protocol to force flash to be left in write mode """
    def flash_unlock_dio(self):
        # Enable flash write mode
        self.flash_begin(0, 0)
        # Reset the chip rather than call flash_finish(), which would have
        # write protected the chip again (why oh why does it do that?!)
        self.mem_begin(0,0,0,0x40100000)
        self.mem_finish(0x40000080)

    """ Perform a chip erase of SPI flash """
    def flash_erase(self):
        # Trick ROM to initialize SFlash
        self.flash_begin(0, 0)

        # This is hacky: we don't have a custom stub, instead we trick
        # the bootloader to jump to the SPIEraseChip() routine and then halt/crash
        # when it tries to boot an unconfigured system.
        self.mem_begin(0,0,0,0x40100000)
        self.mem_finish(0x40004984)

        # Yup - there's no good way to detect if we succeeded.
        # It it on the other hand unlikely to fail.
      
    """Set the flash params for ESP booter"""
    def flash_spi_param_set(self):
        print("SET FLASH PARAMS")
        result = self.command(ESPROM.ESP_SPI_FLASH_SET,
                          struct.pack('<IIIIII', 0, (128/8)*1024*1024, 64*1024,4*1024,256,0xffff))[1]
        if result != "\0\0":
            raise FatalError.WithResult('Failed to config flash (result "%s")', result)        
    
    def calcrc_onebyte(self,abyte):
        abyte=abyte&0xff
        crc_1byte = 0
        for i in range(8):
            if ((crc_1byte^abyte)&0x01)>0:
                crc_1byte ^= 0x18
                crc_1byte >>= 1
                crc_1byte |= 0x80
            else:
                crc_1byte >>= 1
            abyte >>= 1
        return crc_1byte
    
    def calcrc8(self,dlist):
        crc = 0
        clen = len(dlist)
        for i in range(clen):
            crc = self.calcrc_onebyte(crc ^ dlist[i])

        return crc    
    
    def calcrc16(self,data_list):
	length = len(data_list)
        CRC_CCITT = 0x1021
	crc = 0;
	#unsigned char i ; 
	
	while length>0:
	    for data in data_list:
		length-=1
		i = 0x80
		while i >= 1:
		    crc*=2;
		    if (crc&0x10000) != 0:
			crc^=0x11021
			crc = crc&0xffffffff
		    if data&i != 0:
			crc^= CRC_CCITT
		    i/=2
	return crc
    def check_crc(self):
        if self.CHIP == "ESP8266":
            mac0 = self.read_reg(self.ESP_OTP_MAC0)
            mac1 = self.read_reg(self.ESP_OTP_MAC1)
            mac2 = self.read_reg(self.ESP_OTP_MAC2)
            mac3 = self.read_reg(self.ESP_OTP_MAC3)
            efuse = (mac3<<96)|(mac2<<64)|(mac1<<32)|(mac0<<0) 
            crc8_high_cal = self.calcrc8([(efuse>>96)&0xff,(efuse>>104)&0xff,(efuse>>112)&0xff])
            crc8_high = (efuse>>88)&0xff
            if not crc8_high_cal == crc8_high:
                print("CRC8_H ERROR...")
                return False
            crc_low_version = (efuse>>56)&0xf
            print("crc_low_version:",crc_low_version)
            crc8_low = (efuse>>16)&0xff
            if crc_low_version == 0:
                if not crc8_low == 0:
                    print("CRC8_L V0 ERROR...")
                    return False
            elif crc_low_version in [0x1,0x2]:
                crc8_low_cal = self.calcrc8([(efuse>>24)%0xff,(efuse>>32)%0xff,(efuse>>40)%0xff,(efuse>>48)%0xff])
                
                if not crc8_low_cal == crc8_low:
                    print("CRC8_L V1 ERROR... ")
                    return False
            return True
        elif self.CHIP == "ESP32":              
	    word16 = self.read_reg(self.EFUSE_ESP32_RDATA16)
	    word17 = self.read_reg(self.EFUSE_ESP32_RDATA17)
	    word18 = self.read_reg(self.EFUSE_ESP32_RDATA18)
	    word19 = self.read_reg(self.EFUSE_ESP32_RDATA19)            
	    wifi_crc_data = [((word17>>16)&0xff),((word17>>8)&0xff),((word17>>0)&0xff),((word16>>24)&0xff),((word16>>16)&0xff),((word16>>8)&0xff)]
	    bt_crc_data = [((word19>>16)&0xff),((word19>>8)&0xff),((word19>>0)&0xff),((word18>>24)&0xff),((word18>>16)&0xff),((word18>>8)&0xff)]
	    wifi_crc_data.reverse()
	    bt_crc_data.reverse()
	    efuse_crc_wifi = word16 & 0xff
	    efuse_crc_bt = word18 & 0xff
	    wifi_crc_cal = self.calcrc16(wifi_crc_data) &0xff
	    bt_crc_cal = self.calcrc16(bt_crc_data) &0xff
	    if not wifi_crc_cal == efuse_crc_wifi:
		print "WIFI CRC ERROR."
		return False
	    if not bt_crc_cal == efuse_crc_bt:
		print "BT CRC ERROR"
		return False
	    return True
	
    def get_mac(self):
	if self.CHIP == "ESP8266":
	    wifi_mac_sta = self.read_mac()
	    if wifi_mac_sta == None:
		return False
	    self.ESP_MAC_STA = "%02X-%02X-%02X-%02X-%02X-%02X"%(mac_sta[0],mac_sta[1],mac_sta[2],mac_sta[3],mac_sta[4],mac_sta[5])
	elif self.CHIP == "ESP32":
	    wifi_mac_sta,bt_mac = self.read_mac()
	    self.ESP_MAC_STA = "%02X-%02X-%02X-%02X-%02X-%02X"%(wifi_mac_sta[0],wifi_mac_sta[1],wifi_mac_sta[2],wifi_mac_sta[3],wifi_mac_sta[4],wifi_mac_sta[5])
	    self.ESP_BT_MAC = "%02X-%02X-%02X-%02X-%02X-%02X"%(bt_mac[0],bt_mac[1],bt_mac[2],bt_mac[3],bt_mac[4],bt_mac[5])
	return True
	
    """ GPIO Change """
    def HSPI_INIT(self):
	if self.CHIP == "ESP32":
	    print"hspi by gpio"
	    self.write_reg(0x60008474,0xfc,0xffffffff)#hold spi sig output
	     #hspi gpio init
	    val=self.read_reg(addr=0X60009030)
	    self.write_reg(addr=0X60009030, value=((2<<12)|val), mask=0XFFFFFFFF)
	    val=self.read_reg(addr=0X60009034)
	    self.write_reg(addr=0X60009034, value=((2<<12)|val), mask=0XFFFFFFFF)
	    val=self.read_reg(addr=0X60009038)
	    self.write_reg(addr=0X60009038, value=((2<<12)|val), mask=0XFFFFFFFF)
	    val=self.read_reg(addr=0X6000903c)
	    self.write_reg(addr=0X6000903c, value=((2<<12)|val), mask=0XFFFFFFFF)
	    val=self.read_reg(addr=0X60009040)
	    self.write_reg(addr=0X60009040, value=((2<<12)|val), mask=0XFFFFFFFF)
	    val=self.read_reg(addr=0X60009048)
	    self.write_reg(addr=0X60009048, value=((2<<12)|val), mask=0XFFFFFFFF)
	    #gpio enable
	    val=self.read_reg(addr=0X60004020)
	    self.write_reg(addr=0X60004020, value=(0xf014|val), mask=0XFFFFFFFF)
	    #gpio out enable 
	    self.write_reg(addr=0X600041a4, value=((5<<24)|(0<<16)|(2<<8)|(1<<0)), mask=0XFFFFFFFF)
	    self.write_reg(addr=0X6000419c, value=(3<<0), mask=0XFFFFFFFF)
	    self.write_reg(addr=0X60004198, value=(4<<16), mask=0XFFFFFFFF)
	    #gpio input enable
	    self.write_reg(addr=0X600041c0, value=0x3f, mask=0XFFFFFFFF)
	    self.write_reg(addr=0X60004130, value=((2<<24)|(4<<18)|(13<<12)|(12<<6)|(14<<0)), mask=0XFFFFFFFF)
	    self.write_reg(addr=0X60004134, value=(15<<0), mask=0XFFFFFFFF) 
	elif self.CHIP == "ESP8266":
	    print("Support ESP8266 LATER")
	    

class ESPFirmwareImage:

    def __init__(self, filename=None):
        self.segments = []
        self.entrypoint = 0
        self.flash_mode = 0
        self.flash_size_freq = 0

        if filename is not None:
            f = file(filename, 'rb')
            (magic, segments, self.flash_mode, self.flash_size_freq, self.entrypoint) = struct.unpack('<BBBBI', f.read(8))

            # some sanity check
            if magic != ESPROM.ESP_IMAGE_MAGIC or segments > 16:
                raise FatalError('Invalid firmware image')

            for i in xrange(segments):
                (offset, size) = struct.unpack('<II', f.read(8))
                if offset > 0x40200000 or offset < 0x3ffe0000 or size > 65536:
                    raise FatalError('Suspicious segment 0x%x, length %d' % (offset, size))
                segment_data = f.read(size)
                if len(segment_data) < size:
                    raise FatalError('End of file reading segment 0x%x, length %d (actual length %d)' % (offset, size, len(segment_data)))
                self.segments.append((offset, size, segment_data))

            # Skip the padding. The checksum is stored in the last byte so that the
            # file is a multiple of 16 bytes.
            align = 15 - (f.tell() % 16)
            f.seek(align, 1)

            self.checksum = ord(f.read(1))

    def add_segment(self, addr, data):
        # Data should be aligned on word boundary
        l = len(data)
        if l % 4:
            data += b"\x00" * (4 - l % 4)
        if l > 0:
            self.segments.append((addr, len(data), data))

    def save(self, filename):
        f = file(filename, 'wb')
        f.write(struct.pack('<BBBBI', ESPROM.ESP_IMAGE_MAGIC, len(self.segments),
                            self.flash_mode, self.flash_size_freq, self.entrypoint))

        checksum = ESPROM.ESP_CHECKSUM_MAGIC
        for (offset, size, data) in self.segments:
            f.write(struct.pack('<II', offset, size))
            f.write(data)
            checksum = ESPROM.checksum(data, checksum)

        align = 15 - (f.tell() % 16)
        f.seek(align, 1)
        f.write(struct.pack('B', checksum))


class ELFFile:

    def __init__(self, name):
        self.name = name
        self.symbols = None

    def _fetch_symbols(self):
        if self.symbols is not None:
            return
        self.symbols = {}
        try:
            tool_nm = "xtensa-lx106-elf-nm"
            if os.getenv('XTENSA_CORE') == 'lx106':
                tool_nm = "xt-nm"
            proc = subprocess.Popen([tool_nm, self.name], stdout=subprocess.PIPE)
        except OSError:
            print "Error calling %s, do you have Xtensa toolchain in PATH?" % tool_nm
            sys.exit(1)
        for l in proc.stdout:
            fields = l.strip().split()
            try:
                if fields[0] == "U":
                    print "Warning: ELF binary has undefined symbol %s" % fields[1]
                    continue
                self.symbols[fields[2]] = int(fields[0], 16)
            except ValueError:
                raise FatalError("Failed to strip symbol output from nm: %s" % fields)

    def get_symbol_addr(self, sym):
        self._fetch_symbols()
        return self.symbols[sym]

    def get_entry_point(self):
        tool_readelf = "xtensa-lx106-elf-readelf"
        if os.getenv('XTENSA_CORE') == 'lx106':
            tool_readelf = "xt-readelf"
        try:
            proc = subprocess.Popen([tool_readelf, "-h", self.name], stdout=subprocess.PIPE)
        except OSError:
            print "Error calling %s, do you have Xtensa toolchain in PATH?" % tool_readelf
            sys.exit(1)
        for l in proc.stdout:
            fields = l.strip().split()
            if fields[0] == "Entry":
                return int(fields[3], 0)

    def load_section(self, section):
        tool_objcopy = "xtensa-lx106-elf-objcopy"
        if os.getenv('XTENSA_CORE') == 'lx106':
            tool_objcopy = "xt-objcopy"
        tmpsection = tempfile.mktemp(suffix=".section")
        try:
            subprocess.check_call([tool_objcopy, "--only-section", section, "-Obinary", self.name, tmpsection])
            with open(tmpsection, "rb") as f:
                data = f.read()
        finally:
            os.remove(tmpsection)
        return data


def arg_auto_int(x):
    return int(x, 0)


def div_roundup(a, b):
    """ Return a/b rounded up to nearest integer,
    equivalent result to int(math.ceil(float(int(a)) / float(int(b))), only
    without possible floating point accuracy errors.
    """
    return (int(a) + int(b) - 1) / int(b)


class FatalError(RuntimeError):
    """
    Wrapper class for runtime errors that aren't caused by internal bugs, but by
    ESP8266 responses or input content.
    """
    def __init__(self, message):
        RuntimeError.__init__(self, message)

    @staticmethod
    def WithResult(message, result):
        """
        Return a fatal error object that includes the hex values of
        'result' as a string formatted argument.
        """
        return FatalError(message % ", ".join(hex(ord(x)) for x in result))


def main():
    parser = argparse.ArgumentParser(description='ESP CHIP ROM Bootloader Utility', prog='esptool')

    parser.add_argument('--chip', '-c', help='CHIP_TYPE',
                                    choices=['ESP8266', 'ESP32'], default='ESP32')
    
    parser.add_argument(
        '--port', '-p',
        help='Serial port device',
        default='/dev/ttyUSB0')

    parser.add_argument(
        '--baud', '-b',
        help='Serial port baud rate',
        type=arg_auto_int,
        default=ESPROM.ESP_ROM_BAUD)

    subparsers = parser.add_subparsers(
        dest='operation',
        help='Run esptool {command} -h for additional help')

    parser_load_ram = subparsers.add_parser(
        'load_ram',
        help='Download an image to RAM and execute')
    parser_load_ram.add_argument('filename', help='Firmware image')

    parser_dump_mem = subparsers.add_parser(
        'dump_mem',
        help='Dump arbitrary memory to disk')
    parser_dump_mem.add_argument('address', help='Base address', type=arg_auto_int)
    parser_dump_mem.add_argument('size', help='Size of region to dump', type=arg_auto_int)
    parser_dump_mem.add_argument('filename', help='Name of binary dump')

    parser_read_mem = subparsers.add_parser(
        'read_mem',
        help='Read arbitrary memory location')
    parser_read_mem.add_argument('address', help='Address to read', type=arg_auto_int)

    parser_write_mem = subparsers.add_parser(
        'write_mem',
        help='Read-modify-write to arbitrary memory location')
    parser_write_mem.add_argument('address', help='Address to write', type=arg_auto_int)
    parser_write_mem.add_argument('value', help='Value', type=arg_auto_int)
    parser_write_mem.add_argument('mask', help='Mask of bits to write', type=arg_auto_int)

    parser_write_flash = subparsers.add_parser(
        'write_flash',
        help='Write a binary blob to flash')
    parser_write_flash.add_argument('addr_filename', nargs='+', help='Address and binary file to write there, separated by space')
    parser_write_flash.add_argument('--flash_freq', '-ff', help='SPI Flash frequency',
                                    choices=['40m', '26m', '20m', '80m'], default='40m')
    parser_write_flash.add_argument('--flash_mode', '-fm', help='SPI Flash mode',
                                    choices=['qio', 'qout', 'dio', 'dout'], default='qio')
    parser_write_flash.add_argument('--flash_size', '-fs', help='SPI Flash size in Mbit',
                                    choices=['1MB', '2MB','4MB', '8MB', '16MB'], default='1MB')

    subparsers.add_parser(
        'run',
        help='Run application code in flash')

    parser_image_info = subparsers.add_parser(
        'image_info',
        help='Dump headers from an application image')
    parser_image_info.add_argument('filename', help='Image file to parse')

    parser_make_image = subparsers.add_parser(
        'make_image',
        help='Create an application image from binary files')
    parser_make_image.add_argument('output', help='Output image file')
    parser_make_image.add_argument('--segfile', '-f', action='append', help='Segment input file')
    parser_make_image.add_argument('--segaddr', '-a', action='append', help='Segment base address', type=arg_auto_int)
    parser_make_image.add_argument('--entrypoint', '-e', help='Address of entry point', type=arg_auto_int, default=0)

    parser_elf2image = subparsers.add_parser(
        'elf2image',
        help='Create an application image from ELF file')
    parser_elf2image.add_argument('input', help='Input ELF file')
    parser_elf2image.add_argument('--output', '-o', help='Output filename prefix', type=str)
    parser_elf2image.add_argument('--flash_freq', '-ff', help='SPI Flash frequency',
                                  choices=['40m', '26m', '20m', '80m'], default='40m')
    parser_elf2image.add_argument('--flash_mode', '-fm', help='SPI Flash mode',
                                  choices=['qio', 'qout', 'dio', 'dout'], default='qio')
    parser_elf2image.add_argument('--flash_size', '-fs', help='SPI Flash size in Mbit',
                                  choices=['1MB', '2MB', '4MB', '8MB', '16MB'], default='1MB')

    subparsers.add_parser(
        'read_mac',
        help='Read MAC address from OTP ROM')

    subparsers.add_parser(
        'flash_id',
        help='Read SPI flash manufacturer and device ID')

    parser_read_flash = subparsers.add_parser(
        'read_flash',
        help='Read SPI flash content')
    parser_read_flash.add_argument('address', help='Start address', type=arg_auto_int)
    parser_read_flash.add_argument('size', help='Size of region to dump', type=arg_auto_int)
    parser_read_flash.add_argument('filename', help='Name of binary dump')

    subparsers.add_parser(
        'erase_flash',
        help='Perform Chip Erase on SPI flash')

    subparsers.add_parser('test')
    
    args = parser.parse_args()

    # Create the ESPROM connection object, if needed
    esp = None
    if args.operation not in ('image_info','make_image','elf2image'):
        esp = ESPROM(args.port, args.baud,args.chip)
        esp.connect()

    # Do the actual work. Should probably be split into separate functions.
    if args.operation == 'load_ram':
        image = ESPFirmwareImage(args.filename)

        print 'RAM boot...'
        for (offset, size, data) in image.segments:
            print 'Downloading %d bytes at %08x...' % (size, offset),
            sys.stdout.flush()
            esp.mem_begin(size, div_roundup(size, esp.ESP_RAM_BLOCK), esp.ESP_RAM_BLOCK, offset)

            seq = 0
            while len(data) > 0:
                esp.mem_block(data[0:esp.ESP_RAM_BLOCK], seq)
                data = data[esp.ESP_RAM_BLOCK:]
                seq += 1
            print 'done!'

        print 'All segments done, executing at %08x' % image.entrypoint
        esp.mem_finish(image.entrypoint)

    elif args.operation == 'read_mem':
        print '0x%08x = 0x%08x' % (args.address, esp.read_reg(args.address))

    elif args.operation == 'write_mem':
        esp.write_reg(args.address, args.value, args.mask, 0)
        print 'Wrote %08x, mask %08x to %08x' % (args.value, args.mask, args.address)

    elif args.operation == 'dump_mem':
        f = file(args.filename, 'wb')
        for i in xrange(args.size / 4):
            d = esp.read_reg(args.address + (i * 4))
            f.write(struct.pack('<I', d))
            if f.tell() % 1024 == 0:
                print '\r%d bytes read... (%d %%)' % (f.tell(),
                                                      f.tell() * 100 / args.size),
                sys.stdout.flush()
        print 'Done!'

    elif args.operation == 'write_flash':
        assert len(args.addr_filename) % 2 == 0

        flash_mode = {'qio':0, 'qout':1, 'dio':2, 'dout': 3}[args.flash_mode]
        flash_size_freq = {'1MB':0x00, '2MB':0x10, '4MB':0x20, '8MB':0x30, '16MB':0x40}[args.flash_size]
        flash_size_freq += {'40m':0, '26m':1, '20m':2, '80m': 0xf}[args.flash_freq]
        flash_info = struct.pack('BB', flash_mode, flash_size_freq)

        while args.addr_filename:
            address = int(args.addr_filename[0], 0)
            filename = args.addr_filename[1]
            args.addr_filename = args.addr_filename[2:]
            image = file(filename, 'rb').read()
            print 'Erasing flash...'
            blocks = div_roundup(len(image), esp.ESP_FLASH_BLOCK)
            esp.flash_begin(blocks * esp.ESP_FLASH_BLOCK, address)
            seq = 0
            written = 0
            t = time.time()
            while len(image) > 0:
                print '\rWriting at 0x%08x... (%d %%)' % (address + seq * esp.ESP_FLASH_BLOCK, 100 * (seq + 1) / blocks),
                sys.stdout.flush()
                block = image[0:esp.ESP_FLASH_BLOCK]
                # Fix sflash config data
                if address == 0 and seq == 0 and block[0] == '\xe9':
                    block = block[0:2] + flash_info + block[4:]
                # Pad the last block
                block = block + '\xff' * (esp.ESP_FLASH_BLOCK - len(block))
                esp.flash_block(block, seq)
                image = image[esp.ESP_FLASH_BLOCK:]
                seq += 1
                written += len(block)
            t = time.time() - t
            print '\rWrote %d bytes at 0x%08x in %.1f seconds (%.1f kbit/s)...' % (written, address, t, written / t * 8 / 1000)
        print '\nLeaving...'
        if args.flash_mode == 'dio':
            esp.flash_unlock_dio()
        else:
            esp.flash_begin(0, 0)
            esp.flash_finish(False)

    elif args.operation == 'run':
        esp.run()

    elif args.operation == 'image_info':
        image = ESPFirmwareImage(args.filename)
        print ('Entry point: %08x' % image.entrypoint) if image.entrypoint != 0 else 'Entry point not set'
        print '%d segments' % len(image.segments)
        print
        checksum = ESPROM.ESP_CHECKSUM_MAGIC
        for (idx, (offset, size, data)) in enumerate(image.segments):
            print 'Segment %d: %5d bytes at %08x' % (idx + 1, size, offset)
            checksum = ESPROM.checksum(data, checksum)
        print
        print 'Checksum: %02x (%s)' % (image.checksum, 'valid' if image.checksum == checksum else 'invalid!')

    elif args.operation == 'make_image':
        image = ESPFirmwareImage()
        if len(args.segfile) == 0:
            raise FatalError('No segments specified')
        if len(args.segfile) != len(args.segaddr):
            raise FatalError('Number of specified files does not match number of specified addresses')
        for (seg, addr) in zip(args.segfile, args.segaddr):
            data = file(seg, 'rb').read()
            image.add_segment(addr, data)
        image.entrypoint = args.entrypoint
        image.save(args.output)

    elif args.operation == 'elf2image':
        if args.output is None:
            args.output = args.input + '-'
        e = ELFFile(args.input)
        image = ESPFirmwareImage()
        image.entrypoint = e.get_entry_point()
        for section, start in ((".text", "_text_start"), (".data", "_data_start"), (".rodata", "_rodata_start")):
            data = e.load_section(section)
            image.add_segment(e.get_symbol_addr(start), data)

        image.flash_mode = {'qio':0, 'qout':1, 'dio':2, 'dout': 3}[args.flash_mode]
        image.flash_size_freq = {'1MB':0x00, '2MB':0x10, '4MB':0x20, '8MB':0x30, '16MB':0x40}[args.flash_size]
        image.flash_size_freq += {'40m':0, '26m':1, '20m':2, '80m': 0xf}[args.flash_freq]

        image.save(args.output + "0x00000.bin")
        data = e.load_section(".irom0.text")
        off = e.get_symbol_addr("_irom0_text_start") - 0x40200000
        assert off >= 0
        f = open(args.output + "0x%05x.bin" % off, "wb")
        f.write(data)
        f.close()

    elif args.operation == 'read_mac':
        if esp.CHIP == "ESP8266":
            mac = esp.read_mac()
            print 'MAC: %s' % ':'.join(map(lambda x: '%02x' % x, mac))
        elif esp.CHIP == "ESP32":
            wifi_mac,bt_mac = esp.read_mac()
            print 'WiFi MAC: %s' % ':'.join(map(lambda x: '%02x' % x, wifi_mac)) 
            print 'BT MAC: %s' % ':'.join(map(lambda x: '%02x' % x, bt_mac)) 
        else:
            print "UNKNOW CHIP..."

    elif args.operation == 'flash_id':
        if esp.CHIP == 'ESP32':
            print 'SUPPORT ESP32 LATER'
        elif esp.CHIP == "ESP8266":
            flash_id = esp.flash_id()
            print 'Manufacturer: %02x' % (flash_id & 0xff)
            print 'Device: %02x%02x' % ((flash_id >> 8) & 0xff, (flash_id >> 16) & 0xff)

    elif args.operation == 'read_flash':
        print 'Please wait...'
        file(args.filename, 'wb').write(esp.flash_read(args.address, 1024, div_roundup(args.size, 1024))[:args.size])

    elif args.operation == 'erase_flash':
        esp.flash_erase()
    
    elif args.operation == "test":
        #esp.flash_begin(0x1000,0x200000)
        res = esp.check_crc()
        print "res:",res

if __name__ == '__main__':
    try:
        main()
    except FatalError as e:
        print '\nA fatal error occurred: %s' % e
        sys.exit(2)
