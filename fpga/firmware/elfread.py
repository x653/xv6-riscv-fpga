#!/bin/python3
from makeelf.elf import *
elf = ELF(e_machine=EM.EM_LKV373A)
print(elf)
elf = ELF.from_file('firmware.elf')
print(elf)
