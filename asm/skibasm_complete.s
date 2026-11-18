global:
    glo1      "colors"

extern:
    ext5      "simPutPixel"
    ext6      "simHasClick"
    ext7      "simGetClick"
    ext8      "simFlush"

main:

b1:
    alloc     r0      2359296
    alloc     r1      2359296
    jmp       b2

b2:
    callev    r2      ext6
    ieqi      r3      r2      0
    cjmp      r3      b8      b3

b3:
    callev    r4      ext7
    lshri     r2      r4      16
    andi      r3      r4      65535
    movi      r4      30
    movi      r5      0
    jmp       b4

b4:
    add       r6      r5      r2
    sub       r7      r2      r5
    add       r8      r5      r3
    muli      r9      r8      1536
    lshri     r10     r9      1
    gep8      r8      r1      r10
    sub       r9      r3      r5
    muli      r10     r9      1536
    lshri     r11     r10     1
    gep8      r9      r1      r11
    mul       r10     r5      r5
    mov       r11     r4
    jmp       b5

b5:
    add       r12     r11     r3
    muli      r13     r12     1536
    lshri     r13     r13     1
    gep8      r12     r1      r13
    gep8      r14     r12     r6
    load      r15     r14
    faddi     r15     r15     90
    store     r15     r14
    sub       r13     r3      r11
    muli      r13     r13     1536
    lshri     r13     r13     1
    gep8      r13     r1      r13
    gep8      r14     r13     r6
    load      r15     r14
    faddi     r15     r15     90
    store     r15     r14
    gep8      r14     r12     r7
    load      r15     r14
    faddi     r15     r15     90
    store     r15     r14
    gep8      r14     r13     r7
    load      r15     r14
    faddi     r15     r15     90
    store     r15     r14
    add       r12     r11     r2
    gep8      r14     r8      r12
    load      r15     r14
    faddi     r15     r15     90
    store     r15     r14
    gep8      r14     r9      r12
    load      r15     r14
    faddi     r15     r15     90
    store     r15     r14
    sub       r12     r2      r11
    gep8      r14     r8      r12
    load      r15     r14
    faddi     r15     r15     90
    store     r15     r14
    gep8      r14     r9      r12
    load      r15     r14
    faddi     r15     r15     90
    store     r15     r14
    addi      r4      r11     -1
    mul       r12     r4      r4
    add       r13     r12     r10
    ilti      r14     r13     900
    mov       r11     r4
    cjmp      r14     b6      b5

b6:
    addi      r12     r5      1
    ilt       r13     r12     r4
    mov       r5      r12
    cjmp      r13     b4      b7

b7:
    callev    r4      ext6
    ieqi      r2      r4      0
    cjmp      r2      b8      b3

b8:
    movi      r3      0
    jmp       b9

b9:
    muli      r2      r3      1536
    lshri     r4      r2      1
    gep8      r5      r1      r4
    ieqi      r6      r3      0
    muli      r4      r3      6144
    gep       r7      r0      r4
    addi      r8      r2      -1536
    lshri     r4      r8      1
    gep8      r9      r1      r4
    ieqi      r8      r3      383
    addi      r4      r2      1536
    lshri     r10     r4      1
    gep8      r2      r1      r10
    load      r4      r5
    mov       r12     r4
    cjmp      r6      b11     b10

b10:
    load      r12     r9
    mov       r13     r4
    cjmp      r8      b12     b11

b11:
    load      r11     r2
    mov       r13     r11
    jmp       b12

b12:
    gepi      r10     r5      8
    load      r10     r10
    fadd      r11     r12     r13
    fadd      r11     r4      r11
    fadd      r12     r10     r11
    fmuli     r11     r4      -4.0
    fadd      r11     r11     r12
    fmuli     r10     r11     0.25
    fadd      r10     r10     r4
    store     r10     r7
    movi      r15     1
    jmp       b17

b13:
    gepi      r10     r5      6136
    load      r4      r10
    mov       r12     r4
    cjmp      r6      b15     b14

b14:
    gepi      r10     r9      6136
    load      r12     r10
    mov       r13     r4
    cjmp      r8      b16     b15

b15:
    gepi      r10     r2      6136
    load      r11     r10
    mov       r13     r11
    jmp       b16

b16:
    gepi      r10     r5      6128
    load      r11     r10
    fadd      r12     r12     r13
    fadd      r10     r12     r11
    fadd      r12     r10     r4
    fmuli     r11     r4      -4.0
    fadd      r11     r11     r12
    fmuli     r10     r11     0.25
    fadd      r10     r10     r4
    gepi      r4      r7      6136
    store     r10     r4
    addi      r10     r3      1
    ieqi      r4      r10     384
    mov       r3      r10
    cjmp      r4      b21     b9

b17:
    gep8      r10     r5      r15
    load      r4      r10
    mov       r12     r4
    cjmp      r6      b19     b18

b18:
    gep8      r11     r9      r15
    load      r12     r11
    mov       r13     r4
    cjmp      r8      b20     b19

b19:
    gep8      r11     r2      r15
    load      r11     r11
    mov       r13     r11
    jmp       b20

b20:
    gepi      r11     r10     -8
    load      r11     r11
    gepi      r14     r10     8
    load      r14     r14
    fadd      r12     r12     r13
    fadd      r12     r12     r11
    fadd      r12     r12     r14
    fmuli     r11     r4      -4.0
    fadd      r11     r11     r12
    fmuli     r11     r11     0.25
    fadd      r11     r11     r4
    gep8      r12     r7      r15
    store     r11     r12
    addi      r15     r15     1
    ieqi      r12     r15     767
    cjmp      r12     b13     b17

b21:
    movi      r3      0
    jmp       b212

b212:
    muli      r4      r3      6144
    gep       r2      r0      r4
    movi      r4      0
    jmp       b23

b22:
    addi      r5      r3      1
    ieqi      r6      r5      384
    mov       r3      r5
    cjmp      r6      b26     b212

b23:
    gep8      r5      r2      r4
    load      r5      r5
    flti      r7      r5      100
    movi      r6      15955986
    cjmp      r7      b24     b25

b24:
    flti      r7      r5      0
    cjmp      r7      b240    b241

b240:
    movi      r5      0
    jmp       b241

b241:
    fdivi     r5      r5      10
    fptoui    r7      r5
    gepg      r8      glo1    r7
    load      r8      r8
    addi      r9      r7      1
    gepg      r10     glo1    r9
    load      r10     r10
    uitofp    r9      r7
    fsub      r11     r5      r9
    prep3     r8      r10     r11
    callv     r6      fn1
    jmp       b25

b25:
    prep3     r4      r3      r6
    calle     ext5
    addi      r4      r4      1
    ieqi      r7      r4      768
    cjmp      r7      b22     b23

b26:
    calle     ext8
    mov       r2      r0
    mov       r0      r1
    mov       r1      r2
    jmp       b2

fn1:

b1:
    lshri     r0      r13     16
    andi      r0      r0      255
    rgb2lin   r1      r0
    lshri     r0      r13     8
    andi      r0      r0      255
    rgb2lin   r0      r0
    andi      r4      r13     255
    rgb2lin   r4      r4
    lshri     r2      r14     16
    andi      r2      r2      255
    rgb2lin   r2      r2
    lshri     r3      r14     8
    andi      r3      r3      255
    rgb2lin   r3      r3
    andi      r7      r14     255
    rgb2lin   r7      r7
    fsub      r8      r2      r1
    fmul      r8      r8      r15
    fadd      r5      r8      r1
    lin2rgb   r5      r5
    fsub      r8      r3      r0
    fmul      r8      r8      r15
    fadd      r1      r8      r0
    lin2rgb   r1      r1
    fsub      r8      r7      r4
    fmul      r8      r8      r15
    fadd      r0      r8      r4
    lin2rgb   r0      r0
    fmuli     r3      r1      255
    fptoui    r2      r3
    fmuli     r1      r5      255
    fptoui    r4      r1
    fmuli     r3      r0      255
    fptoui    r1      r3
    shli      r0      r4      16
    shli      r2      r2      8
    or        r2      r2      r0
    or        r3      r2      r1
    retv      r3
