#!/usr/bin/python
#
# File	: gen_appbin.py
# This file is part of Espressif's generate bin script.
# Copyright (C) 2013 - 2016, Espressif Systems
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of version 3 of the GNU General Public License as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>.

"""This file is part of Espressif's generate bin script.
    argv[1] is elf file name
    argv[2] is version num"""

import string
import sys
import os
import re
import binascii
import struct
import zlib


TEXT_ADDRESS = 0x40040000
TEXT_ADDRESS_APPCPU = 0x3FFA8000
# app_entry = 0
# data_address = 0x3ffb0000
# data_end  = 0x40000000
# text_end  = 0x40120000

CHECKSUM_INIT = 0xEF

chk_sum = CHECKSUM_INIT
blocks = 0
flash_blocks = 0

def write_file(file_name,data):
	if file_name is None:
		print 'file_name cannot be none\n'
		sys.exit(0)

	fp = open(file_name,'ab')

	if fp:
		fp.seek(0,os.SEEK_END)
		fp.write(data)
		fp.close()
	else:
		print '%s write fail\n'%(file_name)

def combine_bin(file_name,dest_file_name,start_offset_addr,need_chk):
    global chk_sum
    
    if dest_file_name is None:
        print 'dest_file_name cannot be none\n'
        sys.exit(0)

    if file_name:        
        fp = open(file_name,'rb')
        if fp:
        	########## write text ##########
            fp.seek(0,os.SEEK_END)
            data_len = fp.tell()
            if data_len:
		if need_chk:
                    tmp_len = (data_len + 3) & (~3)
		else:
	            tmp_len = (data_len + 15) & (~15)
                data_bin = struct.pack('<II',start_offset_addr,tmp_len)
                write_file(dest_file_name,data_bin)
                fp.seek(0,os.SEEK_SET)
                data_bin = fp.read(data_len)
                write_file(dest_file_name,data_bin)
		if need_chk:
		    for loop in range(len(data_bin)):
		        chk_sum ^= ord(data_bin[loop])
                # print '%s size is %d(0x%x),align 4 bytes,\nultimate size is %d(0x%x)'%(file_name,data_len,data_len,tmp_len,tmp_len)
                tmp_len = tmp_len - data_len
                if tmp_len:
                    data_str = ['00']*(tmp_len)
                    data_bin = binascii.a2b_hex(''.join(data_str))
                    write_file(dest_file_name,data_bin)
		    if need_chk:
			for loop in range(len(data_bin)):
			    chk_sum ^= ord(data_bin[loop])
                
        	fp.close()
        else:
        	print '!!!Open %s fail!!!'%(file_name)


def getFileCRC(_path): 
    try: 
        blocksize = 1024 * 64 
        f = open(_path,"rb") 
        str = f.read(blocksize) 
        crc = 0 
        while(len(str) != 0): 
            crc = binascii.crc32(str, crc) 
            str = f.read(blocksize) 
        f.close() 
    except: 
        print 'get file crc error!' 
        return 0 
    return crc

def gen_appbin(app_cpu_flag,combine_bin_flag):
    global chk_sum
    global crc_sum
    global blocks 
    global irom0_addr
    global irom1_addr

    elf_file = sys.argv[1]
    app_bin_file = sys.argv[5]
    ld_fd = open(sys.argv[2])
    ld_data = ld_fd.read()
    app_cpu_flag = int(app_cpu_flag)
    combine_bin_flag = int(combine_bin_flag)
    match = re.search("irom0_0_seg.+?org = (0x[0-9a-fA-F]+),",ld_data)
    if match is not None:
        irom0_addr = match.group(1)
        #irom0_addr = int(addr,base=16)
        #print irom0_addr
    match = re.search("irom0_0_seg.+?len = (0x[0-9a-fA-F]+)",ld_data)
    if match is not None:
        irom0_len = match.group(1)
        irom0_len = int(irom0_len,base=16)
        #print irom0_len
    match = re.search("drom0_0_seg.+?org = (0x[0-9a-fA-F]+),",ld_data)
    if match is not None:
        drom0_addr = match.group(1)
        #drom0_addr = int(addr,base=16)
        #print drom0_addr
    match = re.search("drom0_0_seg.+?len = (0x[0-9a-fA-F]+)",ld_data)
    if match is not None:
        drom0_len = match.group(1)
        drom0_len = int(drom0_len,base=16)
        #print irom1_len
    flash_data_line  = 16
    data_line_bits = 0xf

    irom0text_bin_name = 'eagle.app.v7.irom0text.bin'
    drom0text_bin_name = 'eagle.app.v7.drom0text.bin'
    text_bin_name = 'eagle.app.v7.text.bin'
    data_bin_name = 'eagle.app.v7.data.bin'
    rodata_bin_name = 'eagle.app.v7.rodata.bin'
    flash_bin_name = sys.argv[6] + '/user.ota'
    Dcache_bin_name = sys.argv[6] + '/drom0.bin'
    Icache_flash_bin_name = sys.argv[6] + '/irom0_flash.bin'

    BIN_MAGIC_FLASH  = 0xE9
    BIN_MAGIC_IROM0  = 0xEA
    BIN_MAGIC_DROM0  = 0xEC
    BIN_MAGIC_APPCPU = 0xE1
    data_str = ''
    sum_size = 0

    if os.getenv('COMPILE')=='xcc' :
        cmd = 'xt-nm -g ' + elf_file + ' > eagle.app.sym'
    else :
        cmd = 'xtensa-esp108-elf-nm -g ' + elf_file + ' > eagle.app.sym'

    os.system(cmd)

    fp = file('./eagle.app.sym')
    if fp is None:
        print "open sym file error\n"
        sys.exit(0)

    lines = fp.readlines()
    fp.close()

    entry_addr = None
    p = re.compile('(\w*)(\sT\s)(call_user_start)$')
    for line in lines:
        m = p.search(line)
        if m != None:
            entry_addr = m.group(1)
                        

    if entry_addr is None:
        print 'no entry point!!'
        sys.exit(0)

    data_start_addr = '0'
    p = re.compile('(\w*)(\sA\s)(_data_start)$')
    for line in lines:
        m = p.search(line)
        if m != None:
            data_start_addr = m.group(1)        
            if app_cpu_flag != 1:
                data_start_addr = m.group(1)
                data_start_addr = long(data_start_addr,16)
            elif app_cpu_flag == 1 and long(data_start_addr,16) >=0x3FF80000 and long(data_start_addr,16) <= 0x3FF97FFF: 
                data_start_addr = long(data_start_addr,16) + 0x28000
            elif app_cpu_flag == 1 and long(data_start_addr,16) >=0x3FF98000 and long(data_start_addr,16) <= 0x3FFBC000: 
                data_start_addr = long(data_start_addr,16) - 0x18000
            else:               
                data_start_addr = m.group(1)
                data_start_addr = long(data_start_addr,16)
                                        

    rodata_start_addr = '0'
    p = re.compile('(\w*)(\sA\s)(_rodata_start)$')
    for line in lines:
        m = p.search(line)
        if m != None:
            rodata_start_addr = m.group(1)
            if app_cpu_flag != 1:
                rodata_start_addr = m.group(1)
                rodata_start_addr = long(rodata_start_addr,16)
            elif app_cpu_flag == 1 and long(rodata_start_addr,16) >=0x3FF80000 and long(rodata_start_addr,16) <= 0x3FF97FFF: 
                rodata_start_addr = long(rodata_start_addr,16) + 0x28000
            elif app_cpu_flag == 1 and long(rodata_start_addr,16) >=0x3FF98000 and long(rodata_start_addr,16) <= 0x3FFBC000: 
                rodata_start_addr = long(rodata_start_addr,16) - 0x18000
            else:               
                rodata_start_addr = m.group(1)
                rodata_start_addr = long(rodata_start_addr,16)
                        
    if os.path.getsize(text_bin_name):
        blocks = blocks + 1
    if os.path.getsize(data_bin_name):
        blocks = blocks + 1
    if os.path.getsize(rodata_bin_name):
        blocks = blocks + 1
    if (app_cpu_flag != 1):
        fp1 = open(drom0text_bin_name,'rb')
        fp1.seek(0,os.SEEK_END)
        Dcache_data_len = fp1.tell()
        if Dcache_data_len:       
            Dcache_data_len = struct.pack('<BBBBI',BIN_MAGIC_DROM0,1,0,0,long(drom0_addr,16))
            sum_size = len(Dcache_data_len)
            write_file(flash_bin_name,Dcache_data_len)
            #write_file(Dcache_bin_name,data_bin)
            # drom0.text.bin
            combine_bin(drom0text_bin_name,flash_bin_name,drom0_len,0)
            open(Dcache_bin_name, "wb").write(open(flash_bin_name, "rb").read())
        
    # write irom0 bin head
        fp2 = open(irom0text_bin_name,'rb')
        fp2.seek(0,os.SEEK_END)
        data_len = fp2.tell()
    # print data_len
        if data_len :  
            data_bin = struct.pack('<BBBBI',BIN_MAGIC_IROM0,blocks + 1,0,0,long(irom0_addr,16)) 
            sum_size = len(data_bin)
            write_file(flash_bin_name,data_bin)

            # irom0.text.bin
            if combine_bin_flag != 1:
                icache_flash_size = data_len +  os.path.getsize(text_bin_name) + os.path.getsize(data_bin_name) + os.path.getsize(rodata_bin_name) + 60 + 8 * blocks
            else :
                icache_flash_size = data_len +  os.path.getsize(text_bin_name) + os.path.getsize(data_bin_name) + os.path.getsize(rodata_bin_name) + 60 + 8 * blocks + os.path.getsize(app_bin_file)

            combine_bin(irom0text_bin_name,flash_bin_name,icache_flash_size,0)  

    # text.bin
    
    if app_cpu_flag != 1:  
        data_bin = struct.pack('<BBBBI',BIN_MAGIC_FLASH,blocks,0,0,long(entry_addr,16))
    else:
        data_bin = struct.pack('<BBBBI',BIN_MAGIC_APPCPU,blocks,0,0,long(entry_addr,16))
    sum_size = len(data_bin)
    write_file(flash_bin_name,data_bin)

    if app_cpu_flag != 1:
        combine_bin(text_bin_name,flash_bin_name,TEXT_ADDRESS,1)
    else: 
        combine_bin(text_bin_name,flash_bin_name,TEXT_ADDRESS_APPCPU,1)


    # data.bin
    if data_start_addr:
        combine_bin(data_bin_name,flash_bin_name,data_start_addr,1)

    # rodata.bin
    combine_bin(rodata_bin_name,flash_bin_name,rodata_start_addr,1)


    # write checksum header
    sum_size = os.path.getsize(flash_bin_name) + 1
    sum_size = flash_data_line - (data_line_bits&sum_size)
    if sum_size:
        data_str = ['00']*(sum_size)
        data_bin = binascii.a2b_hex(''.join(data_str))
        write_file(flash_bin_name,data_bin)
        #write_file(Icache_flash_bin_name,data_bin)
    write_file(flash_bin_name,chr(chk_sum & 0xFF)) 
    #write_file(Icache_flash_bin_name,chr(chk_sum & 0xFF))   	  	
  
    if combine_bin_flag != 0:
        print os.path.getsize(app_bin_file)
        print os.path.getsize(flash_bin_name)
        app_bin = open(app_bin_file, "rb")
        write_file(flash_bin_name,app_bin.read())
       
    if app_cpu_flag != 1:
        all_bin_crc = getFileCRC(flash_bin_name)
        # print all_bin_crc
        if all_bin_crc < 0:
            all_bin_crc = abs(all_bin_crc) - 1
        else :
            all_bin_crc = abs(all_bin_crc) + 1
        # print all_bin_crc
        write_file(flash_bin_name,chr((all_bin_crc & 0x000000FF))+chr((all_bin_crc & 0x0000FF00) >> 8)+chr((all_bin_crc & 0x00FF0000) >> 16)+chr((all_bin_crc & 0xFF000000) >> 24))
        icache_file = open(flash_bin_name, "rb")
        if int(os.path.getsize(drom0text_bin_name)):
            Dcache_bin_len = (int(os.path.getsize(drom0text_bin_name)) + 16 +15 ) &(~15)
            #print Dcache_bin_len
            icache_file.seek(Dcache_bin_len ,0)
        open(Icache_flash_bin_name, "wb").write(icache_file.read())
        cmd = 'rm eagle.app.sym'
        os.system(cmd)
        
        print "Generate related files successully in folder " + sys.argv[6]

        if sys.argv[3] == '0':
            print "boot.bin------------------>0x00000"
            if Dcache_data_len:
                print "drom0.bin----------------->0x04000"
                print "irom0_flash.bin----------->0x40000"
            else :
                print "irom0_flash.bin----------->0x04000"
            print "user.ota------------------>(used for OTA)"

if __name__=='__main__':
    
    if len(sys.argv) != 7:
        print 'Usage: gen_appbin.py eagle.app.out'
        sys.exit(0) 

    gen_appbin(sys.argv[3],sys.argv[4])
