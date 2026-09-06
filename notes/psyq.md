# Psy-Q Runtime and SDK Integration

## Scope

The resident executable contains 598 functions classified as Psy-Q CRT or SDK
code:

| Region | Address range | Functions | Bytes |
|---|---:|---:|---:|
| CRT startup | `0x800129D8-0x80012B50` | 3 | `0x178` |
| SDK and runtime | `0x80073704-0x800906D4` | 595 | `0x1C8DC` |

These functions remain exact assembly and do not count toward game-code
decompilation progress. The goal is to identify their original interfaces and
reuse compatible declarations in game C, not to rewrite Sony library objects.

The Sony Computer Entertainment *Run-Time Library Reference* is the primary
API reference:

<https://archive.org/details/SCE-RunTimeLibRef-Sep1999/>

The manual documents interface names and layouts, but it is not evidence that
this executable used the manual's September 1999 library revisions. Embedded
RCS strings, binary signatures, call sites, and data layout remain the local
version authority.

## Evidence and version policy

Use SDK identities only when supported by local evidence. In descending order:

1. An exact object or function signature from a verified Psy-Q library.
2. A documented BIOS vector or syscall wrapper with matching service values.
3. An embedded library string plus matching implementation and call contract.
4. A manual signature corroborated by all local callers and data accesses.
5. An external symbol catalogue used only as a candidate for local review.

The executable contains these library anchors:

| Evidence | Address |
|---|---:|
| `intr.c` revision 1.75, 1997-02-07 | `0x800119B8` |
| `bios.c` revision 1.86, 1997-03-28 | `0x80011D70` |
| `sys.c` revision 1.140, 1998-01-12 | `0x80012148` |
| Sony library copyright 1993-1997 | `0x800919A8` |

They establish member provenance and minimum dates, not a complete SDK release
number. The project independently identifies the toolchain as Psy-Q 4.6.
External Psy-Q 4.7 signature catalogues are useful research evidence, but must
not be imported in bulk or used to override a conflicting local signature.

## Confirmed interface anchors

The following resident addresses have especially strong API-level evidence.
Project symbols remain address-based until each identity passes the normal
symbol review.

| Address | Candidate SDK identity | Local evidence |
|---|---|---|
| `0x80058F10` | `GsGetWorkBase` | Confirmed from the canonical four-instruction getter, the real `libgs.h` `PACKET *` return type, and independent GMS and Unchiga identities. Unlike the three false-positive 16-byte FLIRT matches in the resident LIBDS range, this function returns the actual LIBGS packet work-base pointer consumed by model renderers. |
| `0x80073830` | `InitHeap` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/C57.OBJ` signature. |
| `0x80073840` | `_bu_init` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/C112.OBJ` signature; matching memory-card setup invokes it after lower-level card initialization. |
| `0x80073850` | `SetMem` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/C159.OBJ` signature; resident startup calls `SetMem(2)`. |
| `0x80073860` | `OpenEvent` | Applied Psy-Q 4.6 identity; memory-card setup passes documented event classes, specifications, mode, and callbacks. |
| `0x80073870` | `CloseEvent` | Applied Psy-Q 4.6 identity; `MemCard_CloseIOEvents` calls it on each stored event descriptor during teardown. |
| `0x80073880` | `TestEvent` | Applied Psy-Q 4.6 identity; matching helpers probe four result handles before an operation or map the first signaled handle to result `0`-`3`. |
| `0x80073890` | `EnableEvent` | Applied Psy-Q 4.6 identity; called on all eight memory-card descriptors immediately after creation. |
| `0x800738A0` | `DisableEvent` | Applied from the unique Psy-Q 4.6 `LIBAPI.LIB/A13.OBJ` signature; exact sound teardown disables the stored root-counter event immediately before passing the same handle to `CloseEvent`. |
| `0x800738B0` | `EnterCriticalSection` | Applied Psy-Q 4.6 identity; brackets event creation and teardown with `ExitCriticalSection`. |
| `0x800738C0` | `ExitCriticalSection` | Applied Psy-Q 4.6 identity; paired critical-section exit. |
| `0x800738D0` | `open` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A50.OBJ` signature. |
| `0x800738E0` | `lseek` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A51.OBJ` signature. |
| `0x800738F0` | `read` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A52.OBJ` signature. |
| `0x80073900` | `write` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A53.OBJ` signature. |
| `0x80073910` | `close` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A54.OBJ` signature. |
| `0x80073920` | `nextfile` | Applied Psy-Q 4.6 identity; advances a caller-owned directory record and returns that same pointer on success. |
| `0x80073930` | `Krom2RawAdd` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A81.OBJ` signature. |
| `0x80073940` | `ChangeClearPAD` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A91.OBJ` signature; matching memory-card setup passes zero immediately before `_bu_init`. |
| `0x80073950` | `SetRCnt` | Applied at offset zero of the unique 368-byte Psy-Q 4.6 `LIBAPI.LIB/COUNTER.OBJ` signature; matching sound setup passes `RCntCNT2`, target `0xE000`, and `RCntMdINTR`. |
| `0x800739EC` | `GetRCnt` | Applied at offset `0x9C` of the same unique counter signature; `SD_SequenceTimerCallback` invokes it on `RCntCNT2` once per interrupt. |
| `0x80073A24` | `StartRCnt` | Applied at offset `0xD4` of the same unique counter signature; matching sound setup calls it after configuring the counter and enabling its event. |
| `0x80073A54` | `StopRCnt` | Applied at offset `0x104` of the same unique counter signature; matching setup stops `RCntCNT2` before reconfiguration and both shutdown paths stop it again. |
| `0x80073A88` | `ResetRCnt` | Applied at offset `0x138` of the same unique counter signature; the resident wrapper writes zero to the selected current-count register. |
| `0x80073AC0` | `firstfile` | Applied Psy-Q 4.6 identity; receives a formatted device path and caller-owned directory record, returning that record on success. |
| `0x80073D60` | `firstfile2` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A66.OBJ` signature. |
| `0x80073D7C` | `ReadInitPadFlag` | Applied at offset `0xC` of the unique 656-byte Psy-Q 4.6 `LIBAPI.LIB/PAD.OBJ` signature. |
| `0x80073D8C` | `PAD_init` | Applied at offset `0x1C` of the same unique `LIBAPI.LIB/PAD.OBJ` signature. |
| `0x80073E1C` | `InitPAD` | `Input_InitPads` passes two adjacent 34-byte receive buffers and their exact lengths. |
| `0x80073EAC` | `StartPAD` | Called immediately after `InitPAD` to start the controller service before local input state is reset. |
| `0x80074000` | `InitPAD2` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A18.OBJ` signature. |
| `0x80074010` | `StartPAD2` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A19.OBJ` signature. |
| `0x80074020` | `PAD_init2` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A21.OBJ` signature. |
| `0x80074030` | `SysEnqIntRP` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/L02.OBJ` signature. |
| `0x80074040` | `SysDeqIntRP` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/L03.OBJ` signature. |
| `0x80074050`, `0x80074064` | `EnablePAD`, `DisablePAD` | Applied at offsets zero and `0x14` of the unique `0xA0`-byte Psy-Q 4.6 `LIBAPI.LIB/PATCH.OBJ` signature. |
| `0x80074078` | `_patch_pad` | Applied at offset `0x28` of the same unique `PATCH.OBJ` signature. |
| `0x800740F0` | `FlushCache` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/C68.OBJ` signature. |
| `0x80074100` | `_remove_ChgclrPAD` | Applied at offset zero of the unique `0x70`-byte Psy-Q 4.6 `LIBAPI.LIB/CHCLRPAD.OBJ` signature. |
| `0x80074170` | `VSync` | Applied Psy-Q 4.6 identity; matching callers query frame timing for AI yielding and time-varying screen effects. |
| `0x80074380` | `ChangeClearRCnt` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/L10.OBJ` signature. |
| `0x80074390` | `ResetCallback` | Applied at offset zero of the unique 1,728-byte Psy-Q 4.6 `LIBETC.LIB/INTR.OBJ` signature. |
| `0x800743C0` | `InterruptCallback` | Applied at offset `0x30` of the same unique `LIBETC.LIB/INTR.OBJ` signature. |
| `0x800743F0` | `DMACallback` | Applied at offset `0x60` of the same unique `LIBETC.LIB/INTR.OBJ` signature. |
| `0x80074420` | `VSyncCallback` | Applied at offset `0x90` of the same unique `LIBETC.LIB/INTR.OBJ` signature. |
| `0x80074454` | `VSyncCallbacks` | Applied at offset `0xC4` of the same unique `LIBETC.LIB/INTR.OBJ` signature. |
| `0x80074484` | `StopCallback` | Applied at offset `0xF4` of the same unique `LIBETC.LIB/INTR.OBJ` signature. |
| `0x800744B4` | `RestartCallback` | Applied at offset `0x124` of the same unique `LIBETC.LIB/INTR.OBJ` signature. |
| `0x800744E4` | `CheckCallback` | Applied at offset `0x154` of the same unique `LIBETC.LIB/INTR.OBJ` signature. |
| `0x800744F4` | `GetIntrMask` | Applied at offset `0x164` of the same unique `LIBETC.LIB/INTR.OBJ` signature. |
| `0x8007450C` | `SetIntrMask` | Applied at offset `0x17C` of the same unique `LIBETC.LIB/INTR.OBJ` signature. |
| `0x80074A58` | `_96_remove` | Applied at offset `0x8` of the unique 32-byte Psy-Q 4.6 `LIBAPI.LIB/C114.OBJ` signature. |
| `0x80074A70` | `ReturnFromException` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A23.OBJ` signature. |
| `0x80074A80` | `ResetEntryInt` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A24.OBJ` signature. |
| `0x80074A90` | `HookEntryInt` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A25.OBJ` signature. |
| `0x80074AA0` | `startIntrVSync` | Applied at offset zero of the unique 288-byte Psy-Q 4.6 `LIBETC.LIB/INTR_VB.OBJ` signature. |
| `0x80074E80` | `_SpuInit` | Applied at offset zero of the unique 352-byte Psy-Q 4.6 `LIBSPU.LIB/S_INI.OBJ` signature. |
| `0x80074F68` | `SpuStart` | Applied at offset `0xE8` of the same unique `LIBSPU.LIB/S_INI.OBJ` signature. |
| `0x80074FE0` | `_spu_init` | Applied at offset zero of the unique 2,880-byte Psy-Q 4.6 `LIBSPU.LIB/SPU.OBJ` signature. |
| `0x80075420` | `_spu_FiDMA` | Applied at offset `0x440` of the same unique `LIBSPU.LIB/SPU.OBJ` signature. |
| `0x800754DC` | `_spu_Fr_` | Applied at offset `0x4FC` of the same unique `LIBSPU.LIB/SPU.OBJ` signature. |
| `0x80075584` | `_spu_t` | Applied at offset `0x5A4` of the same unique `LIBSPU.LIB/SPU.OBJ` signature. |
| `0x80075804` | `_spu_Fw` | Applied at offset `0x824` of the same unique `LIBSPU.LIB/SPU.OBJ` signature. |
| `0x80075888` | `_spu_Fr` | Applied at offset `0x8A8` of the same unique `LIBSPU.LIB/SPU.OBJ` signature. |
| `0x800758EC` | `_spu_FsetRXX` | Applied at offset `0x90C` of the same unique `LIBSPU.LIB/SPU.OBJ` signature. |
| `0x80075930` | `_spu_FsetRXXa` | Applied at offset `0x950` of the same unique `LIBSPU.LIB/SPU.OBJ` signature. |
| `0x800759D4` | `_spu_FgetRXXa` | Applied at offset `0x9F4` of the same unique `LIBSPU.LIB/SPU.OBJ` signature. |
| `0x80075A10` | `_spu_FsetPCR` | Applied at offset `0xA30` of the same unique `LIBSPU.LIB/SPU.OBJ` signature. |
| `0x80075AB8` | `_spu_Fw1ts` | Applied at offset `0xAD8` of the same unique `LIBSPU.LIB/SPU.OBJ` signature. |
| `0x80075B20` | `DeliverEvent` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A07.OBJ` signature. |
| `0x80075B30` | `_SpuDataCallback` | Applied from the unique 48-byte Psy-Q 4.6 `LIBSPU.LIB/S_DCB.OBJ` signature. |
| `0x80075B60` | `SpuQuit` | Applied from the unique 128-byte Psy-Q 4.6 `LIBSPU.LIB/S_Q.OBJ` signature; matching shutdown paths stop the SPU after releasing game-owned sound state. |
| `0x80075BE0` | `SpuSetReverb` | Applied from the unique 208-byte Psy-Q 4.6 `LIBSPU.LIB/S_SR.OBJ` signature; matching sound control toggles the active reverb engine. |
| `0x80075CB0` | `_SpuIsInAllocateArea` | Applied at offset zero of the unique 272-byte Psy-Q 4.6 `LIBSPU.LIB/S_M_UTIL.OBJ` signature. |
| `0x80075D30` | `_SpuIsInAllocateArea_` | Applied at offset `0x80` of the same unique `LIBSPU.LIB/S_M_UTIL.OBJ` signature. |
| `0x80075DC0` | `SpuGetReverb` | Applied from the unique 32-byte Psy-Q 4.6 `LIBSPU.LIB/S_GR.OBJ` signature; matching sound control queries the current reverb state. |
| `0x80075DE0` | `SpuSetReverbModeParam` | Applied from the unique 1,248-byte Psy-Q 4.6 `LIBSPU.LIB/S_SRMP.OBJ` signature; matching sound commands submit the 20-byte reverb parameter prefix. |
| `0x800762C0` | `_spu_setReverbAttr` | Applied from the unique 1,232-byte Psy-Q 4.6 `LIBSPU.LIB/S_SRA.OBJ` signature. |
| `0x80076790` | `SpuReserveReverbWorkArea` | Applied from the unique 80-byte Psy-Q 4.6 `LIBSPU.LIB/S_RRWA.OBJ` signature; matching sound setup reserves or releases the work area. |
| `0x800767E0` | `SpuIsReverbWorkAreaReserved` | Applied from the unique 64-byte Psy-Q 4.6 `LIBSPU.LIB/S_IRWAR.OBJ` signature; matching sound setup checks the reservation state. |
| `0x80076820` | `SpuSetReverbVoice` | Applied from the unique 48-byte Psy-Q 4.6 `LIBSPU.LIB/S_SRV.OBJ` signature. |
| `0x80076850` | `_SpuSetAnyVoice` | Applied from the unique 704-byte Psy-Q 4.6 `LIBSPU.LIB/S_SAV.OBJ` signature. |
| `0x80076B10` | `SpuGetReverbVoice` | Applied from the unique 48-byte Psy-Q 4.6 `LIBSPU.LIB/S_GRV.OBJ` signature. |
| `0x80076B40` | `_SpuGetAnyVoice` | Applied from the unique 48-byte Psy-Q 4.6 `LIBSPU.LIB/S_GAV.OBJ` signature. |
| `0x80076B70` | `SpuClearReverbWorkArea` | Applied from the unique 416-byte Psy-Q 4.6 `LIBSPU.LIB/S_CRWA.OBJ` signature. |
| `0x80076D10` | `WaitEvent` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/A10.OBJ` signature. |
| `0x80076D20` | `SpuReadDecodedData` | Applied from the unique 112-byte Psy-Q 4.6 `LIBSPU.LIB/S_RDD.OBJ` signature. |
| `0x80076D90` | `SpuSetIRQ` | Applied from the unique 320-byte Psy-Q 4.6 `LIBSPU.LIB/S_SI.OBJ` signature; matching sound initialization disables the SPU IRQ before shutdown. |
| `0x80076ED0` | `SpuSetKey` | Applied Psy-Q 4.6 identity; matching sound-driver paths switch selected voice masks off during cleanup and slot reuse. |
| `0x80077090` | `SpuGetKeyStatus` | Applied from the unique 144-byte Psy-Q 4.6 `LIBSPU.LIB/S_GKS.OBJ` signature; matching sound-driver paths poll individual voice masks during cleanup and reuse. |
| `0x80077120` | `SpuSetKeyOnWithAttr` | Applied from the unique 48-byte Psy-Q 4.6 `LIBSPU.LIB/S_SKOWA.OBJ` signature. |
| `0x800771B0` | `SpuSetTransferStartAddr` | Applied from the unique 96-byte Psy-Q 4.6 `LIBSPU.LIB/S_STSA.OBJ` signature; matching transfer paths select the SPU RAM destination. |
| `0x80077210` | `SpuSetTransferMode` | Applied from the unique 48-byte Psy-Q 4.6 `LIBSPU.LIB/S_STM.OBJ` signature; matching initialization selects DMA mode zero. |
| `0x80077240` | `SpuIsTransferCompleted` | Applied from the unique 176-byte Psy-Q 4.6 `LIBSPU.LIB/S_ITC.OBJ` signature; matching reset code selects blocking or nonblocking status. |
| `0x800772F0` | `SpuRGetAllKeysStatus` | Applied Psy-Q 4.6 identity at offset zero of the unique 352-byte `LIBSPU.LIB/SR_GAKS.OBJ` signature. |
| `0x800773C4` | `SpuGetAllKeysStatus` | Applied Psy-Q 4.6 identity at offset `0xD4` of the same object; matching sound-driver callers collect all voice key states before update or cleanup work. |
| `0x80077450` | `SpuSetVoiceAttr` | Applied from the unique 1,536-byte Psy-Q 4.6 `LIBSPU.LIB/S_SVA.OBJ` signature; matching sound paths submit raw layout-compatible voice attribute blocks. |
| `0x80077A50` | `_spu_note2pitch` | Applied at offset zero of the unique 512-byte Psy-Q 4.6 `LIBSPU.LIB/S_N2P.OBJ` signature. |
| `0x80077B20` | `_spu_pitch2note` | Applied at offset `0xD0` of the same unique `LIBSPU.LIB/S_N2P.OBJ` signature. |
| `0x80077C50` | `SpuGetVoiceEnvelope` | Applied from the unique 32-byte Psy-Q 4.6 `LIBSPU.LIB/S_GVEX.OBJ` signature; matching sound queries read envelope levels for voices 20-23. |
| `0x80077C70` | `SpuSetCommonAttr` | Applied from the unique 896-byte Psy-Q 4.6 `LIBSPU.LIB/S_SCA.OBJ` signature; matching output transition code submits a layout-compatible common attribute record. |
| `0x80077FF0` | `SpuSetReverbModeType` | Applied Psy-Q 4.6 identity; matching sound-state paths select reverb mode zero while resetting playback state. |
| `0x800781C0` | `StSetRing` | Applied Psy-Q 4.6 identity from the unique 48-byte `LIBCD.LIB/CDROM.OBJ` signature; the matching movie setup installs its sector ring buffer. |
| `0x80078270` | `StClearRing` | Applied Psy-Q 4.6 identity from the unique 96-byte `LIBCD.LIB/C_002.OBJ` signature; the matching movie setup clears the installed ring. |
| `0x800782D0` | `StUnSetRing` | Applied Psy-Q 4.6 identity from the unique 128-byte `LIBCD.LIB/C_003.OBJ` signature; the matching movie teardown releases the ring setup. |
| `0x800783DC` | `StGetBackloc` | Applied Psy-Q 4.6 identity at offset `0x8C` of the unique 240-byte `LIBCD.LIB/C_004.OBJ` signature. |
| `0x80078440` | `StSetStream` | Applied Psy-Q 4.6 identity from the unique 144-byte `LIBCD.LIB/C_005.OBJ` signature; the matching movie setup supplies mode, frame bounds, and completion callback. |
| `0x800784D0` | `StFreeRing` | Applied Psy-Q 4.6 identity from the unique 176-byte `LIBCD.LIB/C_007.OBJ` signature. |
| `0x800785C0` | `StGetNext` | Applied Psy-Q 4.6 identity from the unique 192-byte `LIBCD.LIB/C_009.OBJ` signature. |
| `0x80078680` | `StSetMask` | Applied Psy-Q 4.6 identity from the unique 32-byte `LIBCD.LIB/C_010.OBJ` signature. |
| `0x800786A0` | `StCdInterrupt` | Applied Psy-Q 4.6 identity from the unique 2,800-byte `LIBCD.LIB/C_011.OBJ` signature. |
| `0x8007A860`, `0x8007E8A0` | `CdDataCallback` copies | Byte-identical wrappers that install a callback on DMA channel `3`. |
| `0x8007D3F0` | `DsSearchFile` | Receives a 24-byte file record and a path, then supplies disc-position data. |
| `0x8007E350` | `CdFlush` | No-argument wrapper around the CD library's internal state-reset routine. |
| `0x8007E3D0` | `CdGetSector` | Identified CD-sector transfer interface in the resident CD library. |
| `0x8007E4F0` | `CdGetSector2` | Parallel two-argument sector-transfer wrapper using the library's second transfer path. |
| `0x8007A710` | `CdIntToPos` | Applied Psy-Q 4.6 LIBCD identity; canonical copy of the sector-to-packed-BCD position conversion. |
| `0x8007CDE0` | `DsRead` | Applied Psy-Q 4.6 identity at offset zero of the unique 1,296-byte `LIBDS.LIB/DSREAD.OBJ` signature. |
| `0x8007D190` | `DsReadSync` | Applied Psy-Q 4.6 identity at offset `0x3B0` of the same unique `LIBDS.LIB/DSREAD.OBJ` signature. |
| `0x8007D200` | `DsReadCallback` | Applied Psy-Q 4.6 identity at offset `0x420` of the same unique `LIBDS.LIB/DSREAD.OBJ` signature. |
| `0x8007D214` | `DsReadBreak` | Applied Psy-Q 4.6 identity at offset `0x434` of the same unique `LIBDS.LIB/DSREAD.OBJ` signature. |
| `0x8007D2D0` | `DsReadMode` | Applied Psy-Q 4.6 identity at offset `0x4F0` of the same unique `LIBDS.LIB/DSREAD.OBJ` signature. |
| `0x8007D2F0` | `DsRead2` | Applied Psy-Q 4.6 identity from the unique 256-byte `LIBDS.LIB/DSREAD2.OBJ` signature; the matching movie control path retries this two-argument read. |
| `0x8007E600` | `CdIntToPos_8007E600` | Applied address-qualified identity for the second byte-identical resident copy used by matching game C. |
| `0x800781F0` | `CdPosToInt` | Applied Psy-Q 4.6 LIBCD identity; canonical copy of the packed-BCD position-to-sector conversion. |
| `0x8007E710` | `CdPosToInt_8007E710` | Applied address-qualified identity for the second byte-identical resident copy used by matching game C. |
| `0x8007DD50` | `DsStartReadySystem` | Applied Psy-Q 4.6 identity at offset zero of the unique 1,440-byte `LIBDS.LIB/DSREADY.OBJ` signature; the matching file-transfer path installs its ready callback with an unlimited count. |
| `0x8007DDD4` | `DsEndReadySystem` | Applied Psy-Q 4.6 identity at offset `0x84` of the same unique `LIBDS.LIB/DSREADY.OBJ` signature. |
| `0x8007DE38` | `DsReadySystemMode` | Applied Psy-Q 4.6 identity at offset `0xE8` of the same unique `LIBDS.LIB/DSREADY.OBJ` signature; the matching file-transfer path selects mode `1`. |
| `0x8007E390` | `DsFlush` | Applied Psy-Q 4.6 identity from the unique 64-byte `LIBDS.LIB/D2_005.OBJ` signature. |
| `0x8007E790` | `DsLastPos` | Applied Psy-Q 4.6 identity from the unique 96-byte `LIBDS.LIB/D3_008.OBJ` signature. |
| `0x8007E7F0` | `CdControlB` | Submits the three-argument CD command and blocks until the internal completion code is `2`. |
| `0x8007E860` | `CdReadyCallback` | Replaces and returns the callback invoked with a ready-event status and result pointer. |
| `0x8007E880` | `CdSyncCallback` | Replaces and returns the callback invoked from the command-completion path. |
| `0x8007E8D0` | `SetDumpFnt` | Applied at offset zero of the unique Psy-Q 4.6 `LIBGPU.LIB/FONT.OBJ` signature; matching setup paths select the debug-font stream returned by `FntOpen`. |
| `0x8007E9B0` | `FntOpen` | Applied at offset `0xE0` of the unique `FONT.OBJ` signature; matching callers open a 320x240 on-screen debug text window. |
| `0x8007EC68` | `FntFlush` | Applied at offset `0x398` of the unique `FONT.OBJ` signature. |
| `0x8007EF84` | `FntPrint` | Applied at offset `0x6B4` of the unique `FONT.OBJ` signature; the matching duel debug helper prints its effect values and divider strings. |
| `0x8007F350` | `ResetGraph` | Applied at offset zero of the unique 12,032-byte Psy-Q 4.6 `LIBGPU.LIB/SYS.OBJ` signature. |
| `0x8007F4C4` | `SetGraphDebug` | Applied at offset `0x174` of the same unique `SYS.OBJ` signature. |
| `0x8007F520` | `SetGraphQueue` | Applied at offset `0x1D0` of the same unique `SYS.OBJ` signature. |
| `0x8007F5C4` | `GetGraphDebug` | Applied at offset `0x274` of the same unique `SYS.OBJ` signature. |
| `0x8007F5D4` | `DrawSyncCallback` | Applied at offset `0x284` of the same unique `SYS.OBJ` signature. |
| `0x8007F634` | `SetDispMask` | Applied at offset `0x2E4` of the same unique `SYS.OBJ` signature. |
| `0x8007F6CC` | `DrawSync` | Applied Psy-Q 4.6 identity; `model_handler_registry.c` waits for queued GPU drawing after dispatching a model primitive handler. |
| `0x8007F850` | `ClearImage` | Applied at offset `0x500` of the unique Psy-Q 4.6 `LIBGPU.LIB/SYS.OBJ` signature; matching movie paths clear their display rectangles with the configured RGB triplet. |
| `0x8007F8E0` | `ClearImage2` | Applied at offset `0x590` of the same unique `SYS.OBJ` signature. |
| `0x8007F978` | `LoadImage` | Applied Psy-Q identity; `func_800249E0` uses the tracked `RECT *` / `u32 *` prototype for two image transfers. |
| `0x8007F9D8` | `StoreImage` | Applied at offset `0x688` of the same unique `SYS.OBJ` signature. |
| `0x8007FA38` | `MoveImage` | Applied Psy-Q 4.6 identity; matching callers copy rectangular VRAM regions for screen transitions and palette processing. |
| `0x8007FAF0` | `ClearOTag` | Applied at offset `0x7A0` of the same unique `SYS.OBJ` signature. |
| `0x8007FBB8` | `ClearOTagR` | Applied at offset `0x868` of the same unique `SYS.OBJ` signature. |
| `0x8007FC64` | `DrawPrim` | Applied at offset `0x914` of the same unique `SYS.OBJ` signature. |
| `0x8007FCC0` | `DrawOTag` | Applied at offset `0x970` of the same unique `SYS.OBJ` signature. |
| `0x8007FD30` | `PutDrawEnv` | Applied at offset `0x9E0` of the same unique `SYS.OBJ` signature. |
| `0x8007FDF0` | `DrawOTagEnv` | Applied at offset `0xAA0` of the same unique `SYS.OBJ` signature. |
| `0x8007FEC8` | `GetDrawEnv` | Applied at offset `0xB78` of the same unique `SYS.OBJ` signature. |
| `0x8007FEFC` | `PutDispEnv` | Applied at offset `0xBAC` of the same unique `SYS.OBJ` signature. |
| `0x80080428` | `GetODE` | Applied at offset `0x10D8` of the same unique `SYS.OBJ` signature. |
| `0x80080458` | `SetDrawArea` | Applied at offset `0x1108` of the same unique `SYS.OBJ` signature. |
| `0x800804D8` | `SetDrawOffset` | Applied at offset `0x1188` of the same unique `SYS.OBJ` signature. |
| `0x80080518` | `SetDrawEnv` | Applied at offset `0x11C8` of the same unique `SYS.OBJ` signature. |
| `0x80081DE8` | `LoadImage2` | Applied Psy-Q identity; streamed package callbacks pass rectangle-shaped records and staged image data. |
| `0x80081ED4` | `StoreImage2` | Applied Psy-Q 4.6 identity; `func_800582C0` reads a VRAM rectangle into a local pixel buffer before transforming and re-uploading it. |
| `0x80081FC0` | `MoveImage2` | Applied Psy-Q identity; `file_cd_helpers.c` passes the current display rectangle plus destination coordinates. |
| `0x80082104` | `DrawOTag2` | Applied at offset `0x2DB4` of the unique Psy-Q 4.6 `LIBGPU.LIB/SYS.OBJ` signature. |
| `0x80082200` | `_GPU_ResetCallback` | Applied at offset `0x2EB0` of the unique Psy-Q 4.6 `LIBGPU.LIB/SYS.OBJ` signature. |
| `0x80082250` | `GPU_cw` | Applied from the unique 16-byte Psy-Q 4.6 `LIBAPI.LIB/C73.OBJ` signature. |
| `0x80082290` | `BreakDraw` | Applied at offset zero of the unique 544-byte Psy-Q 4.6 `LIBGPU.LIB/BREAK.OBJ` signature. |
| `0x80082324` | `IsIdleGPU` | Applied Psy-Q 4.6 identity; matching upload and move paths poll mode `3` around GPU image operations. |
| `0x80082384` | `ContinueDraw` | Applied at offset `0xF4` of the same unique `BREAK.OBJ` signature. |
| `0x80082780` | `GetTPage` | Applied from the unique 64-byte Psy-Q 4.6 `LIBGPU.LIB/P00.OBJ` signature. |
| `0x800827C0` | `GetClut` | Applied from the unique 32-byte Psy-Q 4.6 `LIBGPU.LIB/P01.OBJ` signature. |
| `0x800827E0` | `AddPrim` | Applied from the unique 64-byte Psy-Q 4.6 `LIBGPU.LIB/P06.OBJ` signature. |
| `0x80082820` | `TermPrim` | Applied from the unique 32-byte Psy-Q 4.6 `LIBGPU.LIB/P09.OBJ` signature. |
| `0x80082840` | `SetSemiTrans` | Applied from the unique 40-byte Psy-Q 4.6 `LIBGPU.LIB/P10.OBJ` signature. |
| `0x80082870` | `SetShadeTex` | Applied from the unique 40-byte Psy-Q 4.6 `LIBGPU.LIB/P11.OBJ` signature. |
| `0x800828A0` | `SetPolyF3` | Applied from the unique 20-byte Psy-Q 4.6 `LIBGPU.LIB/P12.OBJ` signature. |
| `0x800828C0` | `SetPolyFT3` | Applied from the unique 20-byte Psy-Q 4.6 `LIBGPU.LIB/P13.OBJ` signature. |
| `0x800828E0` | `SetPolyG3` | Applied from the unique 20-byte Psy-Q 4.6 `LIBGPU.LIB/P14.OBJ` signature. |
| `0x80082900` | `SetPolyGT3` | Applied from the unique 20-byte Psy-Q 4.6 `LIBGPU.LIB/P15.OBJ` signature. |
| `0x80082920` | `SetPolyF4` | Applied from the unique 20-byte Psy-Q 4.6 `LIBGPU.LIB/P16.OBJ` signature. |
| `0x80082940` | `SetPolyFT4` | Applied from the unique 20-byte Psy-Q 4.6 `LIBGPU.LIB/P17.OBJ` signature. |
| `0x80082960` | `SetPolyG4` | Applied from the unique 20-byte Psy-Q 4.6 `LIBGPU.LIB/P18.OBJ` signature. |
| `0x80082980` | `SetPolyGT4` | Applied from the unique 20-byte Psy-Q 4.6 `LIBGPU.LIB/P19.OBJ` signature. |
| `0x800829A0` | `SetSprt8` | Applied from the unique 20-byte Psy-Q 4.6 `LIBGPU.LIB/P20.OBJ` signature. |
| `0x800829C0` | `SetTile` | Applied from the unique 20-byte Psy-Q 4.6 `LIBGPU.LIB/P26.OBJ` signature. |
| `0x800829E0` | `SetDrawMode` | Applied from the unique 148-byte Psy-Q 4.6 `LIBGPU.LIB/P41.OBJ` signature. |
| `0x80082A80` | `OpenTIM` | Applied Psy-Q 4.6 identity at offset zero of the unique 400-byte `LIBGPU.LIB/T00.OBJ` signature. |
| `0x80082A90` | `ReadTIM` | Applied Psy-Q 4.6 identity at offset `0x10` of the same unique `LIBGPU.LIB/T00.OBJ` signature. |
| `0x80084240` | `GsSortBoxFill` | Applied Psy-Q 4.6 identity; the matching fade renderer submits strip or full-screen box fills to its ordering table. |
| `0x800849F0` | `GsSortFastSprite` | Applied Psy-Q 4.6 identity; matching UI paths submit sprite records to an ordering table at the requested priority. |
| `0x80084DD0` | `GsInitGraph` | Applied Psy-Q 4.6 identity at offset zero of the unique 1,360-byte `LIBGS.LIB/GS_001.OBJ` signature. |
| `0x80084F60` | `GsInitGraph2` | Applied Psy-Q 4.6 identity at offset `0x190` of the same `LIBGS.LIB/GS_001.OBJ` object. |
| `0x800851E8` | `GsSortClear` | Applied Psy-Q 4.6 identity at offset `0x418` of the same `LIBGS.LIB/GS_001.OBJ` object. |
| `0x800855D0` | `GsSetLsMatrix` | Applied Psy-Q 4.6 identity; matching projection paths install their local-screen matrix before GTE projection work. |
| `0x800857E0` | `GsSetFlatLight` | Applied Psy-Q 4.6 identity; the matching scene setup installs three directional light records. |
| `0x80086100` | `GsGetTimInfo` | Applied Psy-Q 4.6 identity; `model_texture_upload.c` parses a TIM image header before uploading its pixel and CLUT rectangles. |
| `0x800866A0` | `rsin` | Applied Psy-Q 4.6 identity; matching callers use its 4096-unit fixed-point sine output for model and display motion. |
| `0x80086770` | `rcos` | Applied Psy-Q 4.6 identity; matching callers use its 4096-unit fixed-point cosine output alongside `rsin`. |
| `0x80086810` | `SetFogNearFar` | Applied Psy-Q 4.6 identity; matching campaign-map callers configure near and far depth-cue distances from the current camera projection. |
| `0x80086DC8` | `InitGeom` | Applied Psy-Q 4.6 identity at offset `0x8` of `LIBGTE.LIB/MSC00.OBJ`; resident startup paths invoke it before further GTE setup. |
| `0x80086E50` | `SquareRoot0` | Applied Psy-Q 4.6 identity; matching model, duel, and spatial-sound callers derive integer distances from sums of squared coordinates. |
| `0x80087070` | `ApplyMatrixLV` | Applied Psy-Q 4.6 identity from the unique 352-byte `LIBGTE.LIB/MTX_004.OBJ` signature. |
| `0x800871D0` | `PushMatrix` | Applied Psy-Q 4.6 identity; the matching model transform path saves the active GTE matrix state. |
| `0x80087274` | `PopMatrix` | Applied Psy-Q 4.6 identity; restores the matrix state saved by `PushMatrix` after projection work. |
| `0x80087370` | `MulMatrix` | Applied Psy-Q 4.6 identity; the matching model path composes two rotation matrices before later transforms. |
| `0x80087670` | `ScaleMatrix` | Applied Psy-Q 4.6 identity; matching model paths scale a rotation matrix by a fixed-point vector. |
| `0x800877B0` | `SetRotMatrix` | Applied Psy-Q 4.6 identity; `func_80041F90` reloads the GTE rotation matrix after negating its first and third columns. |
| `0x80087870` | `SetBackColor` | Applied Psy-Q 4.6 identity; the matching scene-lighting setup installs equal ambient red, green, and blue values. |
| `0x80087890` | `SetFarColor` | Applied Psy-Q 4.6 identity; matching scene setup paths clear or configure the GTE far color before lighting and fog work. |
| `0x800878B0` | `SetGeomOffset` | Applied Psy-Q 4.6 identity; matching projection paths set the GTE screen-center coordinates. |
| `0x800878D0` | `SetGeomScreen` | Applied Psy-Q 4.6 identity; matching projection paths set the GTE projection-plane distance. |
| `0x80087A50` | `RotAverage3` | Applied Psy-Q 4.6 identity; the matching duel projection path transforms three vertices and inspects the returned flag. |
| `0x80087AB0` | `RotAverage4` | Applied Psy-Q 4.6 identity; the matching duel renderer transforms four vertices and returns depth and flag outputs. |
| `0x800899A0` | `ratan2` | Applied Psy-Q 4.6 identity; matching view and duel callers derive 4096-unit angles from coordinate deltas. |
| `0x8008AD50` | `GsSetRefView2` | Applied Psy-Q 4.6 identity; matching model paths install the shared 32-byte reference-view record. |
| `0x8008B330` | `_card_info` | Applied Psy-Q 4.6 identity from the unique 16-byte `LIBCARD.LIB/C171.OBJ` signature; matching memory-card request paths issue this BIOS operation before polling their event handles. |
| `0x8008B340` | `_card_load` | Applied Psy-Q 4.6 identity from the unique 16-byte `LIBCARD.LIB/C172.OBJ` signature; matching memory-card request paths issue it before polling load completion. |
| `0x8008B350` | `_card_write` | Applied Psy-Q 4.6 identity from the unique 16-byte `LIBCARD.LIB/A78.OBJ` signature. |
| `0x8008B360` | `_card_read` | Applied Psy-Q 4.6 identity from the unique 16-byte `LIBCARD.LIB/A79.OBJ` signature. |
| `0x8008B370` | `_new_card` | Applied Psy-Q 4.6 identity from the unique 16-byte `LIBCARD.LIB/A80.OBJ` signature. |
| `0x8008B380` | `_card_status` | Applied Psy-Q 4.6 identity from the unique 16-byte `LIBCARD.LIB/A92.OBJ` signature. |
| `0x8008B390` | `_card_wait` | Applied Psy-Q 4.6 identity from the unique 16-byte `LIBCARD.LIB/A93.OBJ` signature. |
| `0x8008B3A0` | `_card_clear` | Applied Psy-Q 4.6 identity from the unique 64-byte `LIBCARD.LIB/CARD.OBJ` signature; matching initialization and retry paths clear a channel before polling its events. |
| `0x8008B3E0` | `InitCARD` | Applied Psy-Q 4.6 identity at offset zero of the unique 256-byte `LIBCARD.LIB/INIT.OBJ` signature; the matching card setup forwards its incoming mode value. |
| `0x8008B470` | `StartCARD` | Applied Psy-Q 4.6 identity at offset `0x90` of the same unique `LIBCARD.LIB/INIT.OBJ` signature; the matching setup starts the card subsystem immediately after initialization. |
| `0x8008B4B8` | `StopCARD` | Applied Psy-Q 4.6 identity at offset `0xD8` of the same unique `LIBCARD.LIB/INIT.OBJ` signature. |
| `0x8008B4E0` | `InitCARD2` | Applied Psy-Q 4.6 identity from the unique 16-byte `LIBCARD.LIB/A74.OBJ` signature. |
| `0x8008B4F0` | `StartCARD2` | Applied Psy-Q 4.6 identity from the unique 16-byte `LIBCARD.LIB/A75.OBJ` signature. |
| `0x8008B500` | `StopCARD2` | Applied Psy-Q 4.6 identity from the unique 16-byte `LIBCARD.LIB/A76.OBJ` signature. |
| `0x8008B510` | `_patch_card_info` | Applied at offset zero of the unique 544-byte Psy-Q 4.6 `LIBCARD.LIB/PATCH.OBJ` signature. |
| `0x8008B5EC` | `_patch_card` | Applied at offset `0xDC` of the same unique `LIBCARD.LIB/PATCH.OBJ` signature. |
| `0x8008B680` | `_patch_card2` | Applied at offset `0x170` of the same unique `LIBCARD.LIB/PATCH.OBJ` signature. |
| `0x8008B6F0` | `_copy_memcard_patch` | Applied at offset `0x1E0` of the same unique `LIBCARD.LIB/PATCH.OBJ` signature. |
| `0x8008B730` | `_ExitCard` | Applied at offset zero of the unique 128-byte Psy-Q 4.6 `LIBCARD.LIB/END.OBJ` signature. |
| `0x8008B7B0` | `MemCardInit` | Applied at offset zero of the unique 80-byte Psy-Q 4.6 `LIBMCRD.LIB/INIT.OBJ` signature. |
| `0x8008B7E0` | `MemCardEnd` | Applied at offset `0x30` of the same unique `LIBMCRD.LIB/INIT.OBJ` signature. |
| `0x8008B800` | `PushCallbackFunc` | Applied at offset zero of the unique 6,352-byte Psy-Q 4.6 `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008B828` | `PullCallbackFunc` | Applied at offset `0x28` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008B850` | `McrdGetGlobalStructure` | Applied at offset `0x50` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008B85C` | `MemCardStart` | Applied at offset `0x5C` of the unique 6,352-byte Psy-Q 4.6 `LIBMCRD.LIB/LIBMCRD.OBJ` signature; matching dialog setup starts the high-level card service. |
| `0x8008B8CC` | `MemCardStop` | Applied at offset `0xCC` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature; matching dialog teardown stops the service. |
| `0x8008B90C` | `MemCardExist` | Applied at offset `0x10C` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008BC90` | `MemCardAccept` | Applied at offset `0x490` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008BFA0` | `MemCardOpen` | Applied at offset `0x7A0` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008C128` | `MemCardClose` | Applied at offset `0x928` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008C16C` | `MemCardReadData` | Applied at offset `0x96C` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008C3D4` | `MemCardWriteData` | Applied at offset `0xBD4` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008C638` | `MemCardReadFile` | Applied at offset `0xE38` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008C858` | `MemCardWriteFile` | Applied at offset `0x1058` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008CA78` | `MemCardGetDirentry` | Applied at offset `0x1278` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008CCD4` | `MemCardCallback` | Applied at offset `0x14D4` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008CCE8` | `MemCardSync` | Applied at offset `0x14E8` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature; matching dialog code polls command and result words until completion. |
| `0x8008CE04` | `MemCardCreateFile` | Applied at offset `0x1604` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008CF00` | `MemCardFormat` | Applied at offset `0x1700` of the same unique `LIBMCRD.LIB/LIBMCRD.OBJ` signature. |
| `0x8008D0D0` | `UserFuncInit` | Applied at offset zero of the unique 272-byte Psy-Q 4.6 `LIBMCRD.LIB/USERFUNC.OBJ` signature. |
| `0x8008D0E0` | `UserFuncOpen` | Applied at offset `0x10` of the same unique `LIBMCRD.LIB/USERFUNC.OBJ` signature. |
| `0x8008D15C` | `UserFuncExecute` | Applied at offset `0x8C` of the same unique `LIBMCRD.LIB/USERFUNC.OBJ` signature. |
| `0x8008D1C8` | `UserFuncComplete` | Applied at offset `0xF8` of the same unique `LIBMCRD.LIB/USERFUNC.OBJ` signature. |
| `0x8008D1E0` | `funcEvSpIOE` | Applied at offset zero of the unique 1,712-byte Psy-Q 4.6 `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D1F4` | `funcEvSpError` | Applied at offset `0x14` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D208` | `funcEvSpTimeout` | Applied at offset `0x28` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D21C` | `funcEvSpNewcard` | Applied at offset `0x3C` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D230` | `funcEvSpIOEx` | Applied at offset `0x50` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D244` | `funcEvSpErrorx` | Applied at offset `0x64` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D258` | `funcEvSpTimeoutx` | Applied at offset `0x78` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D26C` | `funcEvSpNewcardx` | Applied at offset `0x8C` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D280` | `_card_open` | Applied at offset `0xA0` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D2B0` | `_card_start` | Applied at offset `0xD0` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D48C` | `_card_close` | Applied at offset `0x2AC` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D4AC` | `_card_stop` | Applied at offset `0x2CC` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D560` | `_clr_card_event` | Applied at offset `0x380` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D668` | `_get_card_event` | Applied at offset `0x488` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D740` | `_get_card_event_x` | Applied at offset `0x560` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D818` | `_chk_card_event` | Applied at offset `0x638` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D854` | `_chk_card_event_x` | Applied at offset `0x674` of the same unique `LIBMCRD.LIB/BIOS.OBJ` signature. |
| `0x8008D890` | `_card_format2` | Applied at offset zero of the unique 2,704-byte Psy-Q 4.6 `LIBMCRD.LIB/LOW.OBJ` signature. |
| `0x8008DC60` | `_card_create2` | Applied at offset `0x3D0` of the same unique `LIBMCRD.LIB/LOW.OBJ` signature. |
| `0x8008E320` | `bcopy` | Applied Psy-Q 4.6 identity from the unique 64-byte `LIBC2.LIB/BCOPY.OBJ` signature. |
| `0x8008E360` | `bzero` | Applied Psy-Q 4.6 identity from the unique 48-byte `LIBC2.LIB/BZERO.OBJ` signature; `AiScript_Init` clears its three resident state blocks through this entry point. |
| `0x8008E390` | `memcpy` | Applied Psy-Q 4.6 identity from the unique 64-byte `LIBC2.LIB/MEMCPY.OBJ` signature. |
| `0x8008E3D0` | `memset` | Applied Psy-Q 4.6 identity from the unique 48-byte `LIBC2.LIB/MEMSET.OBJ` signature; matching model paths clear eight-byte vector records before filling their components. |
| `0x8008E400` | `qsort` | Applied Psy-Q 4.6 identity from the unique 400-byte `LIBC.LIB`/`LIBC2.LIB` `QSORT.OBJ` signature; matching callers sort resident and overlay record arrays. |
| `0x8008E590` | `rand` | Confirmed Psy-Q C runtime implementation: advances `gRand_dwSeed` with the standard `0x41C64E6D`/`0x3039` recurrence and returns bits 16–30, matching `RAND_MAX` 32767 in `rand.h`. |
| `0x8008E5C0` | `srand` | Confirmed Psy-Q C runtime seed entry point; directly stores its argument in `gRand_dwSeed`. |
| `0x8008E5D0` | `strcat` | Applied Psy-Q 4.6 identity from the unique 176-byte `LIBC2.LIB/STRCAT.OBJ` signature. |
| `0x8008E680` | `strcmp` | Applied Psy-Q 4.6 identity from the unique 112-byte `LIBC2.LIB/STRCMP.OBJ` signature; the matching memory-card directory search compares each entry against the requested name. |
| `0x8008E6F0` | `strcpy` | Applied Psy-Q 4.6 identity from the unique 80-byte `LIBC2.LIB/STRCPY.OBJ` signature; the matching data-transfer path copies its request string into the resident buffer. |
| `0x8008E740` | `strlen` | Applied Psy-Q 4.6 identity from the unique 64-byte `LIBC2.LIB/STRLEN.OBJ` signature. |
| `0x8008E780` | `strncmp` | Applied Psy-Q 4.6 identity from the unique 128-byte `LIBC2.LIB/STRNCMP.OBJ` signature. |
| `0x8008E800` | `strncpy` | Applied Psy-Q 4.6 identity from the unique 112-byte `LIBC2.LIB/STRNCPY.OBJ` signature. |
| `0x8008E870` | `printf` | Applied Psy-Q 4.6 identity from the unique 64-byte `LIBC2.LIB/PRINTF.OBJ` signature; matching diagnostic callers print file positions, AI state failures, duel values, and password-buffer details. |
| `0x8008E8B0` | `prnt` | Applied Psy-Q 4.6 identity at offset zero of the unique 1,696-byte `LIBC2.LIB/PRNT.OBJ` signature. |
| `0x8008EF50` | `memchr` | Applied Psy-Q 4.6 identity from the unique 80-byte `LIBC2.LIB/MEMCHR.OBJ` signature. |
| `0x8008EFA0` | `_putchar` | Applied Psy-Q 4.6 identity at offset zero of the unique 608-byte `LIBC2.LIB/PUTCHAR.OBJ` signature. |
| `0x8008F09C` | `_putchar_flash` | Applied Psy-Q 4.6 identity at offset `0xFC` of the same unique `LIBC2.LIB/PUTCHAR.OBJ` signature. |
| `0x8008F0D8` | `putchar` | Applied Psy-Q 4.6 identity at offset `0x138` of the same unique `LIBC2.LIB/PUTCHAR.OBJ` signature. |
| `0x8008F200` | `sprintf` | Applied Psy-Q 4.6 identity from the unique 2,176-byte `LIBC2.LIB/SPRINTF.OBJ` signature; matching callers format memory-card paths and request strings. |
| `0x8008FA80` | `memmove` | Applied Psy-Q 4.6 identity from the unique 112-byte `LIBC2.LIB/MEMMOVE.OBJ` signature. |
| `0x8008FAF0` | `puts` | Applied Psy-Q 4.6 identity from the unique 96-byte `LIBC2.LIB/PUTS.OBJ` signature. |
| `0x8008FB50` | `setjmp` | Applied Psy-Q 4.6 identity at offset zero of the unique 128-byte `LIBC2.LIB/SETJMP.OBJ` signature. |
| `0x8008FB8C` | `longjmp` | Applied Psy-Q 4.6 identity at offset `0x3C` of the same unique `LIBC2.LIB/SETJMP.OBJ` signature; matching frontend paths use the shared jump buffer for non-local returns. |
| `0x8008FBD0` | `exit` | Applied Psy-Q 4.6 identity from the unique 16-byte `LIBC2.LIB/EXIT.OBJ` signature. |
| `0x8008FBE0` | `DecDCTReset` | Applied Psy-Q 4.6 identity at offset zero of the unique 1,680-byte `LIBPRESS.LIB/LIBPRESS.OBJ` signature; the matching wait path requests mode `1` after a decode timeout. |
| `0x8008FC14` | `DecDCTGetEnv` | Applied Psy-Q 4.6 identity at offset `0x34` of the unique `LIBPRESS.LIB/LIBPRESS.OBJ` signature. |
| `0x8008FCA0` | `DecDCTPutEnv` | Applied Psy-Q 4.6 identity at offset `0xC0` of the same unique `LIBPRESS.LIB/LIBPRESS.OBJ` signature. |
| `0x8008FD38` | `DecDCTin` | Applied Psy-Q 4.6 identity at offset `0x158` of the unique 1,680-byte `LIBPRESS.LIB/LIBPRESS.OBJ` signature. |
| `0x8008FDB4` | `DecDCTout` | Applied Psy-Q 4.6 identity at offset `0x1D4` of the unique 1,680-byte `LIBPRESS.LIB/LIBPRESS.OBJ` signature. |
| `0x8008FDD4` | `DecDCTinSync` | Applied Psy-Q 4.6 identity at offset `0x1F4` of the unique 1,680-byte `LIBPRESS.LIB/LIBPRESS.OBJ` signature. |
| `0x8008FE10` | `DecDCToutSync` | Applied Psy-Q 4.6 identity at offset `0x230` of the unique 1,680-byte `LIBPRESS.LIB/LIBPRESS.OBJ` signature. |
| `0x8008FE58` | `DecDCTinCallback` | Applied Psy-Q 4.6 identity at offset `0x278` of the unique 1,680-byte `LIBPRESS.LIB/LIBPRESS.OBJ` signature. |
| `0x8008FE7C` | `DecDCToutCallback` | Applied Psy-Q 4.6 identity at offset `0x29C` of the unique 1,680-byte `LIBPRESS.LIB/LIBPRESS.OBJ` signature. |
| `0x80090270` | `DecDCTvlcSize2` | Applied Psy-Q 4.6 identity at offset zero of the unique 896-byte `LIBPRESS.LIB/VLC_C.OBJ` signature. |
| `0x800902A0` | `DecDCTvlc2` | Applied Psy-Q 4.6 identity at offset `0x30` of the unique 896-byte `LIBPRESS.LIB/VLC_C.OBJ` signature. |
| `0x800905F0` | `DecDCTvlcBuild` | Applied Psy-Q 4.6 identity at offset zero of the unique 240-byte `LIBPRESS.LIB/BUILD.OBJ` signature. |
| `0x8007FAF0` | `ClearOTag` | Ordering-table initialization behavior. |
| `0x8007FC64` | `DrawPrim` | Direct GPU primitive submission behavior. |
| `0x8007FCC0` | `DrawOTag` | Ordering-table submission behavior. |
| `0x8007FD30` | `PutDrawEnv` | Draw-environment submission contract. |
| `0x8007FEFC` | `PutDispEnv` | Display-environment submission contract. |
| `0x800803F4` | `GetDispEnv` | Applied Psy-Q identity; `file_cd_helpers.c` passes the tracked `DISPENV` record and reuses its leading `disp` rectangle. |

Matching campaign-map setup and per-frame view sources now include
`libgte.h` for `SetFarColor`, `SetFogNearFar`, `SetGeomOffset`, and
`SetGeomScreen`; no overlay-local declarations of those interfaces remain.

The root-counter identities are supported by the resident implementations, not
only by their order in an external symbol list. `SetRCnt`, `GetRCnt`, and
`ResetRCnt` address 16-byte hardware-counter records selected by the low
descriptor bits; `StartRCnt` and `StopRCnt` set and clear the corresponding
interrupt-mask entries. The exact sound lifecycle links those wrappers to the
event API: it stops and configures `RCntCNT2`, opens and enables an `EvSpINT`
event for `SD_SequenceTimerCallback`, starts the counter, then later stops the
counter and disables and closes the same stored event handle.

`SetRotMatrix` is backed by the 48-byte `MTX_09.OBJ` signature in
[the Psy-Q 4.6 LIBGTE catalogue](https://github.com/lab313ru/psx_psyq_signatures/blob/e9e46e7e133ef275a79bfce650924f98edb086bc/460/LIBGTE.LIB.json).
It contains no wildcard bytes and occurs exactly once in the complete retail
executable, at file offset `0x77FB0` (resident `0x800877B0`), with the
`SetRotMatrix` label at object offset zero. The body loads five words from
matrix offsets `0x00` through `0x10` into GTE control registers 0 through 4.
The caller uses the existing `void SetRotMatrix(MATRIX *)` declaration from
`libgte.h`; its local matrix has the same 3x3 signed-halfword rotation layout.

Duplicate library copies require address-qualified symbols rather than aliases.
For example, CD conversion helpers appear more than once in the executable,
and the two 0x24-byte routines at `0x8007A860` and `0x8007E8A0` are
instruction-for-instruction copies. Each forwards its callback argument to the
resident DMA callback installer with channel `3`, matching `CdDataCallback`.
CD teardown selects one of these wrappers according to the active library
state, so both linked addresses are live members rather than redundant padding.
One original name cannot be assigned to multiple resident addresses.

The callback invocation paths distinguish the two adjacent setter routines:

- `0x8007E860` swaps the pointer at `D_800F8394`. Initialization registers
  dispatcher `0x8007BB74` through `0x8007BED4`, whose destination is
  `D_800F5F88`. That dispatcher explicitly recognizes event code `1`
  (`CdlDataReady`) and forwards its one-byte event code and result pointer to
  `D_800F8394`.
- `0x8007E880` swaps the distinct pointer at `D_800F8398`. Initialization
  registers dispatcher `0x8007BC48` through `0x8007BEE0`, whose destination is
  `D_800F5F8C`. The command state machine at `0x8007C7D4` calls
  `D_800F5F8C` with literal event code `2` (`CdlComplete`) at `0x8007C940`;
  `0x8007BC48` then forwards the status and unchanged second argument to
  `D_800F8398`.

This pins `CdReadyCallback` and `CdSyncCallback`, respectively, rather than
relying on the adjacent function order. Both setters return the previous
pointer and use the common status-and-result callback shape.

## Shared declaration policy

The repository owner has chosen to track the real Psy-Q 4.6 headers under
`src/psyq/` so reverse-engineering effort remains focused on the game rather
than reconstructing SDK declarations. Game C should include those headers
directly when it uses a Psy-Q interface.

The imported headers retain Sony's interfaces, scalar spelling, and
documentation. Narrow project adaptations are made only where a fixed-width
record or local include boundary requires them: affected headers include
`src/types.h` and use `u8`, `u16`, or `u32` for those adapted widths. Native
SDK prototypes that use `long`, `unsigned long`, or `unsigned char` remain
unchanged when no project alias is needed. Do not maintain parallel clean-room
declarations for interfaces already provided by these headers.

The four original Japanese root-counter comments in `kernel.h` are stored as
UTF-8 rather than Shift-JIS so repository tools can follow the transitive
`libapi.h` include chain. The declarations and comment text are unchanged.

Recommended header boundaries are:

| Header | Interfaces and records |
|---|---|
| `src/psyq/r3000.h` | MIPS memory segments, exception/status bits, CP0 registers, and context indices |
| `src/psyq/asm.h` | Numeric and ABI register aliases for assembly-facing headers |
| `src/psyq/kernel.h` | BIOS event descriptors, task contexts, executable headers, and directory entries |
| `src/psyq/libapi.h` | Events, critical sections, counters, and low-level memory-card/BIOS wrappers |
| `src/psyq/malloc.h` | Heap initialization and the three allocator families |
| `src/psyq/stdlib.h` | C runtime umbrella for allocation, conversion, sorting, random numbers, search, and exit |
| `src/psyq/abs.h` | Integer absolute-value function and macro |
| `src/psyq/convert.h` | Integer text conversion plus `labs` |
| `src/psyq/qsort.h` | In-place generic sorting with the original unprototyped comparator |
| `src/psyq/rand.h` | Fifteen-bit `rand` result contract and `srand` seed entry point |
| `src/psyq/ctype.h` | Table-driven character classification plus case-conversion functions and macros |
| `src/psyq/limits.h` | Integral-width limits for the Psy-Q MIPS compiler target |
| `src/psyq/stddef.h` | Target definitions of `size_t`, `wchar_t`, `NULL`, and `WEOF` |
| `src/psyq/stdarg.h` | Integer-slot-aligned variadic argument traversal macros |
| `src/psyq/setjmp.h` | Single-task non-local jumps with an explicit saved MIPS register layout |
| `src/psyq/assert.h` | Debug assertion macro using `printf` and `exit`, disabled by `NDEBUG` |
| `src/psyq/stdio.h` | Minimal formatted and character console I/O declarations |
| `src/psyq/libmath.h` | Software floating-point math, conversion helpers, and math error globals |
| `src/psyq/memory.h` | Byte-memory operations plus the BSD `bcopy`/`bzero`/`bcmp` aliases |
| `src/psyq/strings.h` | String operations, including search/token helpers, layered over `memory.h` |
| `src/psyq/string.h` | Compatibility wrapper that includes `strings.h` |
| `src/psyq/libsn.h` | Debugger-host PC file service and `pollhost`/`PSYQpause` break traps |
| `src/psyq/fs.h` | Low-level filesystem device-table, character-buffer, and I/O-block records |
| `src/psyq/sys/types.h` | Target ABI typedefs plus major/minor device-number helpers |
| `src/psyq/sys/errno.h` | Runtime error numbers 1-37 and the external `errno` object |
| `src/psyq/sys/fcntl.h` | Internal `F*` file-mode and device-I/O flags |
| `src/psyq/sys/file.h` | Public `O_*` aliases and seek-origin constants layered over `sys/fcntl.h` |
| `src/psyq/sys/ioctl.h` | Encoded file, terminal/SIO, and disk control request constants |
| `src/psyq/romio.h` | ROM-monitor compatibility include for the system file interface |
| `src/psyq/libsio.h` | Base SIO status, mode, control, lifecycle, and callback interface |
| `src/psyq/libcomb.h` | COMB packet, transfer, control-line, and asynchronous request interface |
| `src/psyq/libcd.h` | CD commands, locations, file records, callbacks, and sector transfers |
| `src/psyq/libds.h` | Ds packet/streaming commands, locations, file records, and callbacks |
| `src/psyq/libetc.h` | Simple pad polling, vertical sync, callbacks, and video mode |
| `src/psyq/libpad.h` | Direct pad communication, Pad-driver multitap/gun setup, modes, and actuator services |
| `src/psyq/libgun.h` | Legacy `InitGUN`/`SelectGUN` light-gun lifecycle interface |
| `src/psyq/libtap.h` | Legacy `InitTAP` multitap lifecycle and enable/disable interface |
| `src/psyq/libgpu.h` | Rectangles, draw/display environments, images, primitives, and ordering tables |
| `src/psyq/libgte.h` | Fixed-point geometry records, assembler-side transfer macros, and out-of-line GTE helpers |
| `src/psyq/inline_c.h` | GCC extended-assembly loads, stores, commands, and state helpers for direct GTE use from C |
| `src/psyq/inline_o.h` | Alternate GCC GTE macros that route operands through fixed temporary registers |
| `src/psyq/gtemac.h` | High-level geometry macros composed from the lower-level `gte_*` operations |
| `src/psyq/inline_s.h` | Preprocessed `aspsx` GTE command macros, with padded and unpadded forms |
| `src/psyq/gtereg_s.h` | Preprocessed `aspsx` names for GTE data and control registers |
| `src/psyq/inline_a.h` | `macro`/`endm`-style assembler GTE command definitions |
| `src/psyq/gtereg.h` | `equs`-style assembler names for GTE data and control registers |
| `src/psyq/gtenom.h` | `macro`/`endm`-style assembler GTE read and store helpers |
| `src/psyq/libgs.h` | `Gs` work areas, objects, lights, cameras, and sorting helpers |
| `src/psyq/libhmd.h` | Hierarchical-model units, primitive handlers, animation, and MIMe records |
| `src/psyq/libsnd.h` | VAB banks, sequences, tones, and high-level playback |
| `src/psyq/libspu.h` | SPU voices, transfer state, reverb, and callbacks |
| `src/psyq/libmcrd.h` | High-level `MemCard` status, directory, and I/O interfaces |
| `src/psyq/libmcx.h` | MCX application, clock, memory, serial, LED, device, and UIFS services |
| `src/psyq/mcgui.h` | Configurable memory-card save/load UI environment and entry points |
| `src/psyq/libpress.h` | MDEC environment, callbacks, input, output, and VLC helpers |

`r3000.h` defines the hardware-facing MIPS vocabulary: cached and uncached
segment conversions, exception vectors, status/cause bits, coprocessor
register names, and saved-context indices. `asm.h` supplies the corresponding
numeric and assembler register aliases. `kernel.h` layers the BIOS ABI records
and constants over both headers, including `TCB`, `EvCB`, `EXEC`, `XF_HDR`,
and the 40-byte `DIRENTRY`. These are low-level ABI definitions; they are not
game-owned scheduler or filesystem structures merely because their fields
have similar roles.

`libapi.h` is the callable BIOS-wrapper layer over `kernel.h`. It declares
root-counter control, events, threads, low-level `open`/`read`/`write` and
directory iteration, executable loading, pad lifecycle, critical sections,
register access, and raw memory-card services. The records passed to
`firstfile`, `nextfile`, `Load`, and `Exec` come from `kernel.h`, while the
`O_*` mode aliases for `open` come from the separate `sys/file.h` stack.
`libmcrd.h` is a higher-level card API and must not be substituted for the
`_card_*` block operations merely because both address memory cards.

Game sources now include `libapi.h` directly for the confirmed pad lifecycle,
raw memory-card operations, memory-card and sound events, critical sections,
and directory iteration. `func_80044470` uses the real `DIRENTRY`,
`firstfile`, and `nextfile` interfaces. No current game C directly includes
`kernel.h` or `libmcrd.h`.

The graphics headers also form distinct layers. `libgpu.h` owns the GPU packet
ABI: `RECT`, `DRAWENV`, `DISPENV`, primitive records, packet-construction
macros, image transfers, and direct primitive or ordering-table submission.
`libgte.h` supplies the fixed-point vector and matrix types used for geometry.
`libgs.h` then builds scene coordinates, cameras, lights, object records, and
sorting helpers on top of both lower layers.

The imported `libgpu.h` also refers to `SVECTOR` in its model-primitive
records without including `libgte.h`. Game C that uses `libgpu.h` therefore
includes `libgte.h` first, even when its own direct use is limited to a GPU
environment or rectangle type.

The resident block at `0x800F56F0` now has field-level evidence matching the
32-byte `GsRVIEW2` record: viewpoint and reference-point triplets, roll, and a
parent-coordinate pointer. Game code initializes it before `GsSetRefView2`
and derives a distance plus two 4096-unit angles from the two points. Matching
`func_800134E0` also uses an embedded `GsRVIEW2` at object offset `+0x10` and
now calls the canonical one-argument `GsSetRefView2` interface byte-identically.
Other matching sources still use local views until their shared-type
migrations are proven exact.

The imported `libgs.h` includes only `src/types.h` even though it refers to
`MATRIX`, `VECTOR`, `SVECTOR`, `CVECTOR`, `RECT`, `DRAWENV`, `DISPENV`, and
`PACKET`. A translation unit using it must therefore expose `libgte.h` and
`libgpu.h` first, rather than treating `libgs.h` as a self-contained umbrella.
`libhmd.h` extends that stack again: its `GsCOORDUNIT`, `GsUNIT`, and
`GsARGUNIT*` records describe hierarchical-model primitive processing and
depend on GTE, GPU, and `libgs` declarations without including those headers.

Matching game C now uses `libgpu.h` across image transfers, display
environments, primitive records, and GPU synchronization. Representative
migrations include `func_800249E0.c`, `func_800289BC.c`,
`file_cd_helpers.c`, `func_800582C0.c`, and
`model_handler_registry.c`. Confirmed camera, lighting, object, packet, and
sorting paths also use `libgs.h`, including `func_800134E0.c`,
`func_8005B260.c`,
`func_800530C4.c`, `model_cleanup.c`, and `model_texture_upload.c`.
No current game C includes `libhmd.h`. These imports justify their specific
API and field uses; a local render or model record still requires field-level
and resident-call evidence before migration to an SDK type.

Matching `func_8005B260` exercises the shared packet ABI directly. It reads
the source primitive's `P_TAG.len`, copies that tag and payload into the
packet work buffer, inserts one `0xE1` draw-mode word, changes the copied
length to `len + 1`, and advances the buffer by the resulting `len + 2` total
words. The low two `flags` bits select the draw-mode semi-transparency rate,
and a nonnegative `flags` value also sets the copied primitive code's `0x2`
semi-transparency flag. It then links the packet through
`addPrim(&ot->org[index & 0xFFFF], packet)`. The exact build with `P_TAG`,
`GsOT`, `setlen`, and `addPrim` confirms that the SDK tag bitfields,
ordering-table member, and 24-bit link macros fit this resident path without a
parallel local packet or ordering-table declaration.

The tracked `libgpu.h` declares both `LoadImage` and `LoadImage2` with the
same `RECT *` / `u32 *` argument shape. Both current matching `LoadImage`
callers, `func_800249E0.c` and `func_800289BC.c`, use that interface.
`func_800582C0.c` likewise uses the shared `LoadImage2` prototype while
retaining rectangle-compatible local storage. Other image-transfer callers
may retain local record views where an SDK structure changes exact code
generation.

The GTE headers are a layered toolchain interface rather than interchangeable
umbrellas. `libgte.h` owns the geometry records and callable library
prototypes. Its `ASSEMBLER` branch also provides a small set of FIFO, matrix,
and register-transfer macros, but the header does not include any of the
standalone inline files. C that emits GTE instructions directly must include
`libgte.h` for types and then select one low-level macro implementation:

- `inline_c.h` lets GCC choose operand registers and supplies both the normal
  command macros, which emit two leading `nop` instructions, and `_b` variants
  that emit the command word without those stalls.
- `inline_o.h` instead moves operands through fixed registers `$12`-`$15` and
  declares those registers plus memory as clobbered. Its command macros include
  the two leading `nop` instructions and it has no `_b` command family.
- `gtemac.h` is a higher-level composition layer. Macros such as
  `gte_RotTransPers` call the selected low-level `gte_ld*`, command, and
  `gte_st*` macros; it provides no types, includes, or include guard itself.

These three macro headers also have no include guards and overlap in the
`gte_*` namespace, so including both low-level implementations is a
redefinition rather than a harmless compatibility choice. Selecting
`inline_c.h` versus `inline_o.h`, or a padded command versus its `_b` form, can
change register allocation and the emitted instruction schedule and therefore
requires an exact-match check. Matching game C now uses `libgte.h` across
camera, model, duel, display, image-transfer, and spatial-sound paths.
The two direct GTE-instruction users in `display_object_projection.c` also
include `inline_c.h` for `gte_stopz`. No current game C
includes `inline_o.h` or `gtemac.h`.

The remaining files target assembly sources. `inline_s.h` and `gtereg_s.h`
use C-preprocessor definitions; `inline_s.h` explicitly identifies `aspsx` as
its target and emits command words with `.word`. `inline_a.h`, `gtereg.h`, and
`gtenom.h` use the alternate `macro`/`endm`, `equs`, and `dw` syntax. The two
families encode the same coprocessor commands and register roles in different
source dialects; neither belongs in a C translation unit.

`libsnd.h` and `libspu.h` expose different sound layers. `libsnd.h` is the
high-level `Ss*` sequencer and VAB interface: it owns `VabHdr`, `ProgAtr`,
`VagAtr`, sequence/SEP playback, tick modes, bank transfer, and utility voice
allocation. `libspu.h` is the lower-level `Spu*` interface for the 24 hardware
voices, SPU RAM transfer, key state, IRQs, reverb, common mixer attributes,
the SPU allocator, and the `SpuSt*` streaming state machine.

The headers deliberately keep parallel API types and constants.
`SndVolume` and `SpuVolume` are both two-short stereo records, and the
`SS_REV_TYPE_*` and `SPU_REV_MODE_*` values describe the same named effect
families, but they belong to different call surfaces. Matching layout or
numeric values alone is not sufficient reason to substitute one family for
the other.

Matching game C now includes `libspu.h` at confirmed low-level API boundaries
for voice setup, key-state polling, key-off cleanup, and reverb control.
No current game C includes `libsnd.h`. These imports establish individual
`Spu*` call contracts; they do not imply that the surrounding game-owned
sound state is an SDK `Spu*` or `Ss*` structure.

`libpress.h` is the codec side of the media stack, not the disc-stream
controller. It defines the `DECDCTENV` quantization/IDCT record, the
`DECDCTTAB` VLC table, `DecDCT*` reset/decode/input/output interfaces, and the
separate `EncSPU*` PCM-to-SPU-waveform encoders. The `StSetRing`,
`StSetStream`, `StGetNext`, and `StFreeRing` functions are instead declared by
both `libcd.h` and `libds.h`; the `SpuSt*` family in `libspu.h` is a third,
SPU-audio streaming interface.

The applied DCT identities delimit three contiguous Psy-Q 4.6 object
signatures:

| Object | Resident signature range | Current naming boundary |
|---|---:|---|
| `LIBPRESS.LIB/LIBPRESS.OBJ` | `0x8008FBE0-0x80090270` (`0x690` bytes) | Nine API names are applied from `DecDCTReset` through `DecDCToutCallback`. Seven following function entries at `0x8008FEA0-0x800901EC` remain address-based; the final `0x8` bytes are zero alignment. |
| `LIBPRESS.LIB/VLC_C.OBJ` | `0x80090270-0x800905F0` (`0x380` bytes) | `DecDCTvlcSize2` and `DecDCTvlc2` occupy the range through `0x800905EC`; the final `0x4` bytes are zero alignment. |
| `LIBPRESS.LIB/BUILD.OBJ` | `0x800905F0-0x800906E0` (`0xF0` bytes) | `DecDCTvlcBuild` occupies `0xE4` bytes; the final `0xC` bytes are the executable's text padding. |

Exact object membership is not enough to name the seven remaining
`LIBPRESS.OBJ` bodies. They may include additional public header interfaces
and private helpers, so they retain address-based identities until an export
offset, caller contract, or implementation signature distinguishes them.

The resident movie setup path combines these layers: `func_8005B8A0` reaches
the CD `St*` ring/stream calls and `DecDCTvlcBuild`. That call chain is evidence
for cooperating APIs, not evidence that their similarly named stream
interfaces are interchangeable. Matching movie setup and teardown C now
includes `libcd.h` for `StSetRing`, `StClearRing`, `StSetStream`, and
`StUnSetRing`. Matching `func_8005C5D4` includes `libpress.h` for the
`DecDCTReset(1)` call used when its decode wait times out.

`libsn.h` is a development-host interface, not a retail storage API.
`PCinit`, `PCopen`, `PCcreat`, `PClseek`, `PCread`, `PCwrite`, and `PCclose`
communicate with the Psy-Q host file server, while `pollhost` and `PSYQpause`
emit debugger break instructions `1024` and `1031`. These declarations are
not interchangeable with `libcd`, `libds`, or memory-card calls. No current
game C includes `libsn.h`, so its presence in the imported SDK set does not
establish a resident dependency on the development host.

`fs.h` describes the implementation-facing filesystem switch rather than an
application file API. Its `device_table`, `device_buf`, and `iob` records carry
device callbacks, buffering state, transfer counts, and file offsets.
`romio.h` adds no independent records or functions; it only includes the
system file declarations used by the ROM-monitor environment. Neither header
is included by current game C. In particular, these records must not replace
`CdlFILE`, `DslFILE`, or `DIRENTRY` merely because all of them participate in
file operations.

The nested `sys/` headers are another compatibility stack. `sys/types.h`
defines target ABI names such as 32-bit `size_t`, `time_t`, and `off_t`,
16-bit `dev_t`, `uid_t`, and `gid_t`, plus the `major`, `minor`, and `makedev`
macros. `sys/fcntl.h` contains only the internal `FREAD`, `FWRITE`, creation,
buffering, and asynchronous-I/O flags. `sys/file.h` includes that header and
maps the public `O_*` names onto those flags while defining `SEEK_SET`,
`SEEK_CUR`, and `SEEK_END`; it declares no file functions itself. `sys/errno.h`
fixes the runtime's error-number ABI from `EPERM` 1 through `EALREADY` 37 and
declares the shared `errno` integer. `sys/ioctl.h` similarly contains request
numbers rather than an `ioctl` prototype: `FIOC*` controls file input,
`TIOC*` controls terminal/SIO behavior, and `DIOFORMAT` requests disk format.
`romio.h` is only an include wrapper around `sys/file.h`, while `libsnd.h`
uses `sys/types.h` for its target scalar types. No current game C includes the
`sys/` headers directly.

`stdlib.h` is an umbrella over the imported `abs.h`, `convert.h`, `malloc.h`,
`qsort.h`, and `rand.h` declarations, with `bsearch` and `exit` added directly.
`libmath.h` is separate: it exposes the double-precision transcendental
functions, `math_errno`/`math_err_point`, and the `printf2`/`sprintf2`
variants. Neither family is interchangeable with the fixed-point vector and
matrix operations in `libgte.h`. No current game C includes these runtime
headers, so existing hand-written declarations should only be migrated after
their exact ABI and compiler behavior are checked.

`ctype.h` classifies an unsigned-byte index through the external `_ctype_`
table. Its `is*` macros return the matching bit mask rather than a normalized
Boolean, while `toupper` and `tolower` are callable functions and the
underscore-prefixed forms are unchecked arithmetic macros. `limits.h`
records the compiler target's eight-bit characters, 16-bit shorts, and
32-bit `int` and `long` ranges; these implementation limits describe scalar
ABI widths, not the signedness or ownership of an unknown game field. No
current game C includes either header, so adopting a classification macro or
limit still requires exact code-generation evidence.

`stddef.h` defines `size_t` as `unsigned int` and `wchar_t` as
`unsigned long`; both are 32-bit types under this target's `limits.h`. It also
defines `NULL` as integer zero and `WEOF` as `0xFFFFFFFF`. `stdarg.h` uses a
`void *` `va_list` and advances it through arguments rounded up to
`sizeof(int)`, encoding the old compiler's stack and alignment assumptions.
These are target/compiler support declarations, not portable host-build
substitutes. No current game C includes either header directly, although
`stdlib.h` includes `stddef.h`.

`setjmp.h` defines `jmp_buf` as twelve 32-bit words for the saved PC, stack
pointer, frame pointer, registers `s0`-`s7`, and global pointer. It is the
single-task form and carries no signal mask or host-thread context.
`assert.h` expands a failed assertion to a formatted `printf` followed by
`exit(1)`; when `NDEBUG` is defined, both `assert` and the underlying
`_assert` macro expand to nothing. Neither header includes the declarations
for those output/termination functions itself. Matching frontend C includes
`setjmp.h` for the two confirmed `longjmp` calls; no current game C includes
`assert.h`.

`stdio.h` is not a complete hosted C stream interface. It defines
`BUFSIZ`, `EOF`, the three seek-origin constants, and a local `size_t`, then
declares only `printf`, `sprintf`, and basic character/string input and output.
There is no `FILE` type or `fopen`/`fread` family, and the `getc`/`putc`
signatures use integer handles rather than stream pointers. It must not be
substituted for the debugger-host file service in `libsn.h` or the retail
disc and memory-card APIs. Matching game and password-overlay C includes it
for the resident `printf` diagnostics, while `src/game/mem_card_requests.c`
uses its `sprintf` declaration for `func_80044470` and three request
formatters. `file_set_position_table.c` keeps its `printf` call unprototyped:
adding any declaration changes GCC 2.8.1's loop layout by three instructions.

`malloc.h` exposes three parallel allocator families:
`InitHeap`/`malloc`/`calloc`/`realloc`/`free`, then identically shaped `*2`
and `*3` variants. Each initializer receives an explicit memory base and
length, so allocations and releases must stay within the same numbered
family. These heaps are separate from game-owned fixed-slot allocators such
as the display-object pool. No current game C includes `malloc.h` directly;
it is also part of the `stdlib.h` umbrella.

The other `stdlib.h` component headers remain deliberately small. `abs.h`
provides both `abs(int)` and an `ABS` macro whose argument can be evaluated
more than once. `convert.h` declares decimal and base-selectable integer
parsers plus `labs`. `qsort.h` retains the original `int (*)()` comparator
prototype; changing a matching caller to a modern fully prototyped callback
can change argument setup. `rand.h` fixes `RAND_MAX` at 32767 and exposes the
`rand`/`srand` pair whose resident implementation is documented separately in
[`rng.md`](rng.md). Matching resident C includes `qsort.h` and `rand.h`
directly. The password/name-entry starter generator and module main, plus the
five matching main-menu sort comparators, now use `rand.h` rather than
duplicate the runtime declaration. The main-menu card-list builder also uses
`qsort.h` with the original unprototyped comparator shape. Newly integrated
`Duel_ShuffleDeck`, `func_80031084`, `func_80043BCC`, and `func_80050584`
also include `rand.h` for their resident RNG calls.

The imported string headers form a compatibility stack rather than three
independent libraries. `string.h` only includes `strings.h`; `strings.h`
declares the string routines and includes `memory.h`; `memory.h` declares the
memory routines and BSD-compatible aliases. Several prototypes intentionally
omit parameter types to avoid conflicts with old compiler built-ins. Matching
game C now includes `memory.h` for the confirmed `bzero` and `memset` calls,
and includes `strings.h` for confirmed `strcmp` and `strcpy` calls. Replacing
an exact hand-written copy loop with `memcpy` or `bcopy` still requires a full
executable match because GCC may choose different load/store sequences.

Do not add `src/types.h` to an imported header solely for uniformity. Headers
that expose project-adapted fixed-width records must include it directly and
use the project aliases; self-contained prototype-only headers may retain the
SDK's native scalar spelling.

`libpad.h` provides `PadInitMtap`, `PadInitGun`, and the shared controller
communication state APIs. The separately imported `libtap.h` and `libgun.h`
provide older `InitTAP` and `InitGUN` lifecycle families with different
buffers and control calls. Device role alone is therefore insufficient to
substitute one family for another; use the header that matches the resident
call signature.

`libetc.h` is a separate convenience layer. `PadRead` returns both simple-pad
states in one 32-bit value, and `_PAD(port, button)` shifts a 16-bit button
mask into the selected controller half. The same header owns `VSync`, callback
reset/restart controls, NTSC/PAL selection, and the `getScratchAddr` macro for
word-indexed access to the `0x1F800000` scratchpad. Its `PadInit`/`PadStop`
pair is not interchangeable with the buffer-oriented `PadInitDirect` and
`PadStopCom` interface in `libpad.h`. Matching `ai_script_vm.c` includes
`libetc.h` for `VSync(1)` while bounding one AI interpreter pass, and
`func_8004E9A0.c` uses `VSync(-1)` to choose time-varying image-copy frames.
No current game C uses the header's simple-pad interfaces.

`libsio.h` and `libcomb.h` expose parallel serial interfaces. The SIO header
defines the controller's status/mode bits, `AddSIO`/`DelSIO`,
`_sio_control`, and `Sio1Callback`. The COMB header uses a distinct
`_comb_control` entry point for packet sizing, buffered transfer counts,
asynchronous requests, RTS/CTS, and VBlank signaling. Their overlapping
status-bit names do not establish interchangeable call contracts. No current
game C includes either header.

`libapi.h` and `libmcrd.h` retain the SDK `kernel.h` dependency for BIOS
records such as `DIRENTRY`. Both include the local `r3000.h` and `asm.h`
first, establishing the kernel guards without relying on system include paths
that the matching compiler does not provide.

These three memory-card-adjacent headers are not interchangeable.
`libmcrd.h` provides file and block operations, `libmcx.h` exposes the
separate `Mcx` device command family, and `mcgui.h` combines card metadata
with background, controller, sound, texture, and cursor resources for
`McGuiSave`/`McGuiLoad`. Matching memory-card dialog C includes `libmcrd.h`
for `MemCardStart`, `MemCardSync`, and `MemCardStop`. No current game C
includes `libmcx.h` or `mcgui.h`, so those rows document the imported header
boundaries rather than claiming a resident function identity.

The real `src/psyq/libds.h` and `src/psyq/libcd.h` provide parallel record
families. The resident file-search anchor is `DsSearchFile`; matching
file-transfer C now uses `DsStartReadySystem` and `DsReadySystemMode`, while
the movie control path uses `DsRead2`. The position conversion used by
`File_GetPosition` is the address-qualified CD-library copy
`CdPosToInt_8007E710`:

| Record | Verified ABI surface |
|---|---|
| `DslLOC` / `CdlLOC` | Layout-compatible four-byte CD locations. The resident MSF-to-LBA routine reads the BCD minute, second, and sector bytes at offsets `0`-`2`. |
| `DslFILE` | 24-byte Ds search result with `DslLOC` at `0`, size at `4`, and a 16-byte name at `8`. `DsSearchFile` copies records at a `0x18` stride. |
| `CdlFILE` | Parallel 24-byte CD-library search result. Its leading `CdlLOC` layout lets `File_GetPosition` pass `DslFILE.pos` to the resident `CdPosToInt_8007E710` copy through an explicit compatible view. |

The conversion routine `CdPosToInt_8007E710` independently verifies the
shared `DslLOC`/`CdlLOC` field order. It converts each of the first three
bytes from packed BCD, then computes:

```text
logical_sector = (minute * 60 + second) * 75 + sector - 150
```

`File_GetPosition` passes `DslFILE.pos` through the explicit
layout-compatible `CdlLOC` view and stores the result as the file position.
The fourth `track` byte is not read by this conversion and should not be
mistaken for part of the sector calculation.

The adjacent inverse routine `CdIntToPos_8007E600` takes a logical sector
number and a destination `CdlLOC *`. After adding the 150-sector lead-in, it
computes:

```text
minute = adjusted_sector / (60 * 75)
second = (adjusted_sector / 75) % 60
sector = adjusted_sector % 75
```

It converts each result to packed BCD and writes offsets `0`, `1`, and `2`.
The destination pointer is also returned. Offset `3` remains untouched, so a
caller that needs a defined `track` value must initialize it separately.
Together, the two resident routines verify both conversion directions without
requiring a copied SDK structure definition.

### Controller receive-buffer evidence

`Input_InitPads` calls the real `libapi.h` `InitPAD` interface with two
adjacent receive buffers:

```text
port 1: gInput_abRawPadBuffers + 0x00, length 0x22
port 2: gInput_abRawPadBuffers + 0x22, length 0x22
```

It starts the service with `StartPAD` before resetting the game's published
input state. Matching `Input_ReadRawPads`, called from `Main_VBlankCB`, reads
the first four bytes of each record:

| Buffer offset | Observed role |
|---:|---|
| `+0` | Packet status; the game accepts the record only when this byte is zero. |
| `+1` | Controller/type byte; the game requires its low nibble to be nonzero. |
| `+2`, `+3` | Active-low 16-bit button word, read in high-byte/low-byte order. |

The parser XORs the button word with `0xFFFF`, turning pressed buttons into
set bits. Port 1 is accumulated into the low half of
`gInput_dwPendingHeld`; port 2 is shifted into the high half. This agrees with
the `_PAD(port, button)` convention in `libetc.h`, which shifts a 16-bit
button mask by 16 bits for the second controller. `Input_UpdatePads` later
consumes and clears the pending word, then publishes held, newly pressed, and
timer-repeated halfwords for both ports.

Only these four packet bytes currently have matching-C consumers. The
remaining `0x1E` bytes in each service-owned record should stay as an opaque
receive buffer until a resident caller establishes an extended controller
layout; importing a newer pad structure solely because it also fits the
buffer would exceed the local ABI evidence.

### Memory-card directory evidence

`func_80044470` formats a `bu%02X:%s` device path, passes it and a caller-owned
record to `firstfile`, then advances subsequent records through `nextfile`.
Both resident functions return the supplied record pointer on success. The
caller allows five retries after the initial attempt before stopping
enumeration.

Three matching-C consumers independently establish the directory geometry:

| Property | Local evidence |
|---|---|
| Directory record size | `func_80044470` and `func_80044598` advance records by 40 bytes. |
| File-size field | `func_80044544` reads a 32-bit size at record offset `24`. |
| Allocation block size | `func_80044544` rounds each file size up to `8192` bytes. |
| Card capacity | Enumeration and free-space accounting both stop at `15` blocks. |

These values are centralized as `MEM_CARD_DIRECTORY_ENTRY_SIZE`,
`MEM_CARD_BLOCK_SIZE`, and `MEM_CARD_BLOCK_COUNT` in `src/game/mem_card.h`.
This proves the directory-record ABI surface used by the game.

The routine at `0x8007E7F0` matches the blocking `CdControlB` interface rather
than the asynchronous `CdControl` variant. It truncates the command argument
to one byte, submits the command and parameter pointer, then repeatedly polls
the command handle while passing through the caller's result pointer. It
returns one only when the internal completion code is `2`, and zero when
command submission fails. Game callers corroborate the command contract:
`func_8005C62C` issues command `0x02` (`CdlSetloc`) followed by `0x16`
(`CdlSeekP`), while another caller loops on command `0x09` (`CdlPause`).

The no-argument wrapper at `0x8007E350` calls `0x8007BE00`, which clears the
resident CD command-active flag and two associated state words before
calling the lower-level hook with both arguments zero. This
reset-without-reinitialization contract matches `CdFlush`, whose purpose is to
discard the current command state while leaving the CD library available for
later operations. The wrapper itself adds no arguments or result
transformation. Matching-C caller `func_80043960` declares the wrapper as
`void func_8007E350(void)` and invokes it immediately before transferring
control to an overlay and polling that module. The overlay continues using
the drive afterward, independently confirming a flush rather than full
CD-library teardown.

The sector-transfer wrappers at `0x8007E3D0` and `0x8007E4F0` preserve the
same destination-pointer and word-count arguments, call distinct low-level
transfer routines, and convert a zero low-level result into return value one.
This mirrored contract and the documented adjacent `CdGetSector` /
`CdGetSector2` interface pair identify `0x8007E4F0` as `CdGetSector2`.
The resident streaming path at `0x8007CF9C` calls `0x8007E4F0`, while its
alternate branch at `0x8007CFEC` passes the same saved arguments to
`0x8007E3D0`; the second entry is therefore live code rather than an unused
library variant.

### Rectangle layout evidence

`func_800249E0` builds two consecutive eight-byte records in `D_80177EA4`.
Each record receives signed halfword stores at offsets `0`, `2`, `4`, and `6`,
then is passed to the resident `LoadImage` function at `0x8007F978`:

| Record | `+0` | `+2` | `+4` | `+6` |
|---|---|---|---|---|
| card image | x coordinate | y coordinate | width `0x14` | height `0x20` |
| palette row | x coordinate `0x380` | y coordinate | width `0x40` | height `1` |

This verifies the Psy-Q `RECT` ABI surface as four signed 16-bit fields in
`x`, `y`, `w`, `h` order and a total size of eight bytes. The declaration
lives in the real `src/psyq/libgpu.h`; `func_800249E0` uses it for both GPU
transfer rectangles while retaining the original byte-offset arithmetic that
selects each record.

Before replacing a local definition:

1. Identify the resident callee and verify its argument and return contract.
2. Compare every local field access with the documented SDK offsets and widths.
3. Check all other definitions with the same size or role; do not migrate a
   single convenient caller while leaving conflicting layouts unexplained.
4. Use the declaration from the matching real Psy-Q header.
5. Rebuild the complete executable. If the shared type changes code generation,
   retain the exact local view and document the exception.

## Initial migration candidates

The existing C sources expose several useful starting points:

| Current source pattern | SDK target | Required proof |
|---|---|---|
| Local `InitPAD` / `StartPAD` declarations | `libapi.h` | Initial migration complete in `src/game/input_init_pads.c`; the real prototypes preserve the exact build. |
| `DslFILE` in `src/psyq/libds.h` | Ds file-search result | Migration complete in `src/game/file_stream.c` and `File_Exists` in `src/game/file_cd_helpers.c`; the latter preserves its integer wrapper interface with explicit casts at the SDK boundary. |
| `RECT` in `src/psyq/libgpu.h` | GPU transfer rectangle | Initial migration complete in `func_800249E0`; preserve byte-offset selection when extending it to other callers. |
| Local `MoveImage` / `LoadImage2` / `StoreImage2` / `IsIdleGPU` declarations | `libgpu.h` | Initial migration complete in `func_800582C0`; the four adjacent signed halfwords remain a local rectangle-compatible view. |
| Local `DrawSync` declaration | `libgpu.h` | Initial migration complete in `model_handler_registry.c`; mode `0` waits for queued GPU work after model primitive dispatch. |
| Local draw/display environment buffers | `DRAWENV` and `DISPENV` | Initial `DISPENV` migration complete in `file_cd_helpers.c`; other buffers still require complete size, alignment, and field-use evidence. |
| Local vector and matrix records | `SVECTOR`, `VECTOR`, `MATRIX` | Separate fixed-point SDK layouts from game-specific render records. |
| Memory-card event descriptor arrays | event handles and card constants | Name the resident BIOS wrappers before centralizing prototypes and constants. |

These migrations are game-source refactors and must remain byte-identical.
Canonical SDK spelling improves call semantics, but exact code generation takes
priority over replacing every local layout.

## Review checklist

- The address belongs to `psyq/crt` or `psyq/sdk` in
  `config/slus_01411/functions.csv`.
- The proposed name follows Sony's API spelling rather than the game's semantic
  naming convention.
- Duplicate linked copies are disambiguated by address.
- Evidence and conflicts are recorded before changing tracked symbols.
- The interface comes from the tracked Psy-Q 4.6 headers.
- Imported declarations use fixed-width types and preserve the observed ABI.
- `make match` still reproduces the retail executable exactly.
