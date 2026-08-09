how to make your own:
- create any c/c++ project in vs
- right click project -> build config -> check `masm(.targets, .props)`
- create your `.asm` file
- right click your `.asm` file -> properties -> Item Type -> `Type = Microsoft Macro Assembler`
- use standard masm syntax with `PROC`/`ENDP` (use x64 calling convention)
- Add an extern "C" declaration in a header, e.g. `extern "C" auto my_func(auto a, auto b)`;
