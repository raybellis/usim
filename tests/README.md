# Test files

## MC6809

The test files are for use with `usim09` which demonstrates a simple
MC6809 system with an MC6850 ACIA at address 0xc000, 32kB of RAM at
 0x0000 and 8kB of ROM at 0xe000.

### `test_main.s`

This is MC6809 code with reset vectors, etc, which can be used as a
test framework around user code supplied in `test.s`.    The included
`test.s` file just takes keyboard (serial) input and shows the contents
of the CPU registers after each input.

### `tbasic09.s`

TinyBASIC V1.38 for the MC6809.

## MOS 6502

The test files are for use with `usim02` which builds a MOS 6502
system with an MC6850 ACIA at address 0xa000, 32kB of RAM at 0x0000
and 16kB of ROM at 0xe000.

### `bbcbasic02.hex`

BBC BASIC v2 as ported by Hoglet67 to single board computers at
https://github.com/hoglet67/BBCBasicSBC02

The code was patched as below:

```
diff --git a/src/SBC_MOS.asm b/src/SBC_MOS.asm
index b3d6c54..c3c3766 100644
--- a/src/SBC_MOS.asm
+++ b/src/SBC_MOS.asm
@@ -4,11 +4,11 @@

 ;; Set to 0 for no flow control
 ;; Set to 1 to enable XON/XOFF handshakine in the recive direction
-USE_XON_XOFF = 1
+USE_XON_XOFF = 0

 ;; Set to 0 to fake the 100Hz timer (it just increments with each read)
 ;; Set to 1 to have NMI increment a 100Hz timer
-USE_NMI_TIMER = 1
+USE_NMI_TIMER = 0

 ;; Set to 0 to use NMI directly
 ;;     (NMI should be 100Hz)
diff --git a/src/combined.asm b/src/combined.asm
index c7d43cb..f0a7dcd 100644
--- a/src/combined.asm
+++ b/src/combined.asm
@@ -2,8 +2,8 @@ org $c000

 include_LANG_HEADER=FALSE        \ 35 bytes
 include_AUTO=TRUE                \ 54 bytes
-include_LOADSAVECHAIN=TRUE       \ 103 bytes
-include_OSCLI=TRUE               \ 34 bytes
+include_LOADSAVECHAIN=FALSE      \ 103 bytes
+include_OSCLI=FALSE              \ 34 bytes
                                  \ 14 byte shared
 include_RANDOMACCESS=FALSE       \ 443 bytes
 ```

After compilation with `beebasm`, the resulting binary was converted
to Intel Hex format using the `srecord` utilities:

```
srec_cat SBC02 -binary --offset 0xc000 -output bbcbasic02.hex -Intel
```

