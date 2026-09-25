# c++ security for dummies

showcasing the basics of skcrypter and lazy importer combo

compile with g++<br />
and if youre using msvc, set your optimization to /Od or /O1

---

## unsecured output:
```cpp
int __fastcall main(int argc, const char **argv, const char **envp)
{
  DWORD CurrentProcessId; // eax

  _main(argc, argv, envp);
  CurrentProcessId = GetCurrentProcessId();
  _mingw_printf("PID: %lu\n", CurrentProcessId);
  return 0;
}
```

## secured output:
```cpp
int __fastcall main(int argc, const char **argv, const char **envp)
{
  char *v3; // rax
  unsigned int v5; // ebx
  struct _LIST_ENTRY *v7; // [rsp+40h] [rbp-40h]
  struct _LIST_ENTRY *i; // [rsp+50h] [rbp-30h]
  _BYTE v9[10]; // [rsp+59h] [rbp-27h] BYREF
  char v10; // [rsp+63h] [rbp-1Dh] BYREF
  int k; // [rsp+64h] [rbp-1Ch]
  _BYTE *v12; // [rsp+68h] [rbp-18h]
  _BYTE *v13; // [rsp+70h] [rbp-10h]
  _BYTE *v14; // [rsp+78h] [rbp-8h]
  _BYTE *v15; // [rsp+80h] [rbp+0h]
  struct _LIST_ENTRY *v16; // [rsp+88h] [rbp+8h]
  char *v17; // [rsp+90h] [rbp+10h]
  char *v18; // [rsp+98h] [rbp+18h]
  int v19; // [rsp+A4h] [rbp+24h]
  unsigned __int64 v20; // [rsp+A8h] [rbp+28h]
  char v21; // [rsp+B7h] [rbp+37h]
  int v22; // [rsp+B8h] [rbp+38h]
  char v23; // [rsp+BFh] [rbp+3Fh]
  int j; // [rsp+C0h] [rbp+40h]
  int v25; // [rsp+C4h] [rbp+44h]
  char *v26; // [rsp+C8h] [rbp+48h]
  int v27; // [rsp+D4h] [rbp+54h]
  unsigned __int64 v28; // [rsp+D8h] [rbp+58h]
  int Blink; // [rsp+E4h] [rbp+64h]
  struct _LIST_ENTRY *v30; // [rsp+E8h] [rbp+68h]
  struct _LIST_ENTRY *Flink; // [rsp+F0h] [rbp+70h]
  struct _PEB *v32; // [rsp+F8h] [rbp+78h]

  _main(argc, argv, envp);
  v32 = NtCurrentPeb();
  for ( i = v32->Ldr->InLoadOrderModuleList.Flink; ; i = i->Flink )
  {
    Flink = i[3].Flink;
    v30 = Flink;
    v7 = (struct _LIST_ENTRY *)((char *)Flink
                              + (unsigned int)*(struct _LIST_ENTRY **)((char *)&Flink[8].Blink + SHIDWORD(Flink[3].Blink)));
    if ( v7 != Flink )
      break;
LABEL_11:
    v16 = i;
  }
  Blink = (int)v7[1].Blink;
  do
  {
    if ( !Blink-- )
      goto LABEL_11;
    v28 = 0xDBF4BDF45E28D907uLL;
    v27 = Blink;
    v26 = (char *)Flink + *(unsigned int *)((char *)&Flink->Flink + 4 * Blink + LODWORD(v7[2].Flink));
    v25 = -604717580;
    for ( j = -604717580; ; j = 16777619 * (j ^ v23) )
    {
      v3 = v26++;
      v23 = *v3;
      if ( !v23 )
        break;
      v22 = j;
      v21 = v23;
    }
    v20 = 0xDBF4BDF45E28D907uLL;
  }
  while ( j != 1579735303 );
  v19 = Blink;
  v18 = (char *)Flink + HIDWORD(v7[1].Blink);
  v17 = (char *)Flink + HIDWORD(v7[2].Flink);
  v5 = ((__int64 (*)(void))((char *)Flink + *(unsigned int *)&v18[4 * *(unsigned __int16 *)&v17[2 * Blink]]))();
  main::{lambda(void)#1}::operator()(v9, &v10);
  v15 = v9;
  v14 = v9;
  if ( v9[9] )
  {
    v13 = v15;
    v12 = v15;
    for ( k = 0; k <= 9; ++k )
      v13[k] = (k % 52 + 48) ^ v12[k];
  }
  _mingw_printf(v9, v5);
  return 0;
}
```
