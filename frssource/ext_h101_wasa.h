/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_WASA_EXT_H101_WASA_H__
#define __GUARD_H101_WASA_EXT_H101_WASA_H__

#ifndef __CINT__
# include <stdint.h>
#else
/* For CINT (old version trouble with stdint.h): */
# ifndef uint32_t
typedef unsigned int uint32_t;
typedef          int  int32_t;
# endif
#endif
#ifndef EXT_STRUCT_CTRL
# define EXT_STRUCT_CTRL(x)
#endif

/********************************************************
 *
 * Plain structure (layout as ntuple/root file):
 */

typedef struct EXT_STR_h101_WASA_t
{
  /* RAW */
  uint32_t WASA_ENE /* [0,384] */;
  uint32_t WASA_ENEI[384 EXT_STRUCT_CTRL(WASA_ENE)] /* [1,384] */;
  uint32_t WASA_ENEv[384 EXT_STRUCT_CTRL(WASA_ENE)] /* [0,16777215] */;

} EXT_STR_h101_WASA;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_WASA_onion_t
{
  /* RAW */
  uint32_t WASA_ENE;
  uint32_t WASA_ENEI[384 /* WASA_ENE */];
  uint32_t WASA_ENEv[384 /* WASA_ENE */];

} EXT_STR_h101_WASA_onion;

/*******************************************************/

#define EXT_STR_h101_WASA_ITEMS_INFO(ok,si,offset,struct_t,printerr) do { \
  ok = 1; \
  /* RAW */ \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     WASA_ENE,                        UINT32,\
                    "WASA_ENE",384); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     WASA_ENEI,                       UINT32,\
                    "WASA_ENEI",                       "WASA_ENE"); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     WASA_ENEv,                       UINT32,\
                    "WASA_ENEv",                       "WASA_ENE"); \
  \
} while (0);

/********************************************************
 *
 * For internal use by the network data reader:
 * (version checks, etc)
 */

typedef struct EXT_STR_h101_WASA_layout_t
{
  uint32_t _magic;
  uint32_t _size_info;
  uint32_t _size_struct;
  uint32_t _size_struct_onion;
  uint32_t _pack_list_items;

  uint32_t _num_items;
  struct {
    uint32_t _offset;
    uint32_t _size;
    uint32_t _xor;
    const char *_name;
  } _items[1];
  uint32_t _pack_list[771];
} EXT_STR_h101_WASA_layout;

#define EXT_STR_h101_WASA_LAYOUT_INIT { \
  0x57e65c93, \
  sizeof(EXT_STR_h101_WASA_layout), \
  sizeof(EXT_STR_h101_WASA), \
  sizeof(EXT_STR_h101_WASA_onion), \
  771, \
  1, \
  { \
    { 0, sizeof(EXT_STR_h101_WASA), 0xb30b167f, "h101_WASA" }, \
  }, \
  { \
    0xc0000000, 0x00000180, 0x00000002, 0x40000004, \
    0x40000604, 0x40000008, 0x40000608, 0x4000000c, \
    0x4000060c, 0x40000010, 0x40000610, 0x40000014, \
    0x40000614, 0x40000018, 0x40000618, 0x4000001c, \
    0x4000061c, 0x40000020, 0x40000620, 0x40000024, \
    0x40000624, 0x40000028, 0x40000628, 0x4000002c, \
    0x4000062c, 0x40000030, 0x40000630, 0x40000034, \
    0x40000634, 0x40000038, 0x40000638, 0x4000003c, \
    0x4000063c, 0x40000040, 0x40000640, 0x40000044, \
    0x40000644, 0x40000048, 0x40000648, 0x4000004c, \
    0x4000064c, 0x40000050, 0x40000650, 0x40000054, \
    0x40000654, 0x40000058, 0x40000658, 0x4000005c, \
    0x4000065c, 0x40000060, 0x40000660, 0x40000064, \
    0x40000664, 0x40000068, 0x40000668, 0x4000006c, \
    0x4000066c, 0x40000070, 0x40000670, 0x40000074, \
    0x40000674, 0x40000078, 0x40000678, 0x4000007c, \
    0x4000067c, 0x40000080, 0x40000680, 0x40000084, \
    0x40000684, 0x40000088, 0x40000688, 0x4000008c, \
    0x4000068c, 0x40000090, 0x40000690, 0x40000094, \
    0x40000694, 0x40000098, 0x40000698, 0x4000009c, \
    0x4000069c, 0x400000a0, 0x400006a0, 0x400000a4, \
    0x400006a4, 0x400000a8, 0x400006a8, 0x400000ac, \
    0x400006ac, 0x400000b0, 0x400006b0, 0x400000b4, \
    0x400006b4, 0x400000b8, 0x400006b8, 0x400000bc, \
    0x400006bc, 0x400000c0, 0x400006c0, 0x400000c4, \
    0x400006c4, 0x400000c8, 0x400006c8, 0x400000cc, \
    0x400006cc, 0x400000d0, 0x400006d0, 0x400000d4, \
    0x400006d4, 0x400000d8, 0x400006d8, 0x400000dc, \
    0x400006dc, 0x400000e0, 0x400006e0, 0x400000e4, \
    0x400006e4, 0x400000e8, 0x400006e8, 0x400000ec, \
    0x400006ec, 0x400000f0, 0x400006f0, 0x400000f4, \
    0x400006f4, 0x400000f8, 0x400006f8, 0x400000fc, \
    0x400006fc, 0x40000100, 0x40000700, 0x40000104, \
    0x40000704, 0x40000108, 0x40000708, 0x4000010c, \
    0x4000070c, 0x40000110, 0x40000710, 0x40000114, \
    0x40000714, 0x40000118, 0x40000718, 0x4000011c, \
    0x4000071c, 0x40000120, 0x40000720, 0x40000124, \
    0x40000724, 0x40000128, 0x40000728, 0x4000012c, \
    0x4000072c, 0x40000130, 0x40000730, 0x40000134, \
    0x40000734, 0x40000138, 0x40000738, 0x4000013c, \
    0x4000073c, 0x40000140, 0x40000740, 0x40000144, \
    0x40000744, 0x40000148, 0x40000748, 0x4000014c, \
    0x4000074c, 0x40000150, 0x40000750, 0x40000154, \
    0x40000754, 0x40000158, 0x40000758, 0x4000015c, \
    0x4000075c, 0x40000160, 0x40000760, 0x40000164, \
    0x40000764, 0x40000168, 0x40000768, 0x4000016c, \
    0x4000076c, 0x40000170, 0x40000770, 0x40000174, \
    0x40000774, 0x40000178, 0x40000778, 0x4000017c, \
    0x4000077c, 0x40000180, 0x40000780, 0x40000184, \
    0x40000784, 0x40000188, 0x40000788, 0x4000018c, \
    0x4000078c, 0x40000190, 0x40000790, 0x40000194, \
    0x40000794, 0x40000198, 0x40000798, 0x4000019c, \
    0x4000079c, 0x400001a0, 0x400007a0, 0x400001a4, \
    0x400007a4, 0x400001a8, 0x400007a8, 0x400001ac, \
    0x400007ac, 0x400001b0, 0x400007b0, 0x400001b4, \
    0x400007b4, 0x400001b8, 0x400007b8, 0x400001bc, \
    0x400007bc, 0x400001c0, 0x400007c0, 0x400001c4, \
    0x400007c4, 0x400001c8, 0x400007c8, 0x400001cc, \
    0x400007cc, 0x400001d0, 0x400007d0, 0x400001d4, \
    0x400007d4, 0x400001d8, 0x400007d8, 0x400001dc, \
    0x400007dc, 0x400001e0, 0x400007e0, 0x400001e4, \
    0x400007e4, 0x400001e8, 0x400007e8, 0x400001ec, \
    0x400007ec, 0x400001f0, 0x400007f0, 0x400001f4, \
    0x400007f4, 0x400001f8, 0x400007f8, 0x400001fc, \
    0x400007fc, 0x40000200, 0x40000800, 0x40000204, \
    0x40000804, 0x40000208, 0x40000808, 0x4000020c, \
    0x4000080c, 0x40000210, 0x40000810, 0x40000214, \
    0x40000814, 0x40000218, 0x40000818, 0x4000021c, \
    0x4000081c, 0x40000220, 0x40000820, 0x40000224, \
    0x40000824, 0x40000228, 0x40000828, 0x4000022c, \
    0x4000082c, 0x40000230, 0x40000830, 0x40000234, \
    0x40000834, 0x40000238, 0x40000838, 0x4000023c, \
    0x4000083c, 0x40000240, 0x40000840, 0x40000244, \
    0x40000844, 0x40000248, 0x40000848, 0x4000024c, \
    0x4000084c, 0x40000250, 0x40000850, 0x40000254, \
    0x40000854, 0x40000258, 0x40000858, 0x4000025c, \
    0x4000085c, 0x40000260, 0x40000860, 0x40000264, \
    0x40000864, 0x40000268, 0x40000868, 0x4000026c, \
    0x4000086c, 0x40000270, 0x40000870, 0x40000274, \
    0x40000874, 0x40000278, 0x40000878, 0x4000027c, \
    0x4000087c, 0x40000280, 0x40000880, 0x40000284, \
    0x40000884, 0x40000288, 0x40000888, 0x4000028c, \
    0x4000088c, 0x40000290, 0x40000890, 0x40000294, \
    0x40000894, 0x40000298, 0x40000898, 0x4000029c, \
    0x4000089c, 0x400002a0, 0x400008a0, 0x400002a4, \
    0x400008a4, 0x400002a8, 0x400008a8, 0x400002ac, \
    0x400008ac, 0x400002b0, 0x400008b0, 0x400002b4, \
    0x400008b4, 0x400002b8, 0x400008b8, 0x400002bc, \
    0x400008bc, 0x400002c0, 0x400008c0, 0x400002c4, \
    0x400008c4, 0x400002c8, 0x400008c8, 0x400002cc, \
    0x400008cc, 0x400002d0, 0x400008d0, 0x400002d4, \
    0x400008d4, 0x400002d8, 0x400008d8, 0x400002dc, \
    0x400008dc, 0x400002e0, 0x400008e0, 0x400002e4, \
    0x400008e4, 0x400002e8, 0x400008e8, 0x400002ec, \
    0x400008ec, 0x400002f0, 0x400008f0, 0x400002f4, \
    0x400008f4, 0x400002f8, 0x400008f8, 0x400002fc, \
    0x400008fc, 0x40000300, 0x40000900, 0x40000304, \
    0x40000904, 0x40000308, 0x40000908, 0x4000030c, \
    0x4000090c, 0x40000310, 0x40000910, 0x40000314, \
    0x40000914, 0x40000318, 0x40000918, 0x4000031c, \
    0x4000091c, 0x40000320, 0x40000920, 0x40000324, \
    0x40000924, 0x40000328, 0x40000928, 0x4000032c, \
    0x4000092c, 0x40000330, 0x40000930, 0x40000334, \
    0x40000934, 0x40000338, 0x40000938, 0x4000033c, \
    0x4000093c, 0x40000340, 0x40000940, 0x40000344, \
    0x40000944, 0x40000348, 0x40000948, 0x4000034c, \
    0x4000094c, 0x40000350, 0x40000950, 0x40000354, \
    0x40000954, 0x40000358, 0x40000958, 0x4000035c, \
    0x4000095c, 0x40000360, 0x40000960, 0x40000364, \
    0x40000964, 0x40000368, 0x40000968, 0x4000036c, \
    0x4000096c, 0x40000370, 0x40000970, 0x40000374, \
    0x40000974, 0x40000378, 0x40000978, 0x4000037c, \
    0x4000097c, 0x40000380, 0x40000980, 0x40000384, \
    0x40000984, 0x40000388, 0x40000988, 0x4000038c, \
    0x4000098c, 0x40000390, 0x40000990, 0x40000394, \
    0x40000994, 0x40000398, 0x40000998, 0x4000039c, \
    0x4000099c, 0x400003a0, 0x400009a0, 0x400003a4, \
    0x400009a4, 0x400003a8, 0x400009a8, 0x400003ac, \
    0x400009ac, 0x400003b0, 0x400009b0, 0x400003b4, \
    0x400009b4, 0x400003b8, 0x400009b8, 0x400003bc, \
    0x400009bc, 0x400003c0, 0x400009c0, 0x400003c4, \
    0x400009c4, 0x400003c8, 0x400009c8, 0x400003cc, \
    0x400009cc, 0x400003d0, 0x400009d0, 0x400003d4, \
    0x400009d4, 0x400003d8, 0x400009d8, 0x400003dc, \
    0x400009dc, 0x400003e0, 0x400009e0, 0x400003e4, \
    0x400009e4, 0x400003e8, 0x400009e8, 0x400003ec, \
    0x400009ec, 0x400003f0, 0x400009f0, 0x400003f4, \
    0x400009f4, 0x400003f8, 0x400009f8, 0x400003fc, \
    0x400009fc, 0x40000400, 0x40000a00, 0x40000404, \
    0x40000a04, 0x40000408, 0x40000a08, 0x4000040c, \
    0x40000a0c, 0x40000410, 0x40000a10, 0x40000414, \
    0x40000a14, 0x40000418, 0x40000a18, 0x4000041c, \
    0x40000a1c, 0x40000420, 0x40000a20, 0x40000424, \
    0x40000a24, 0x40000428, 0x40000a28, 0x4000042c, \
    0x40000a2c, 0x40000430, 0x40000a30, 0x40000434, \
    0x40000a34, 0x40000438, 0x40000a38, 0x4000043c, \
    0x40000a3c, 0x40000440, 0x40000a40, 0x40000444, \
    0x40000a44, 0x40000448, 0x40000a48, 0x4000044c, \
    0x40000a4c, 0x40000450, 0x40000a50, 0x40000454, \
    0x40000a54, 0x40000458, 0x40000a58, 0x4000045c, \
    0x40000a5c, 0x40000460, 0x40000a60, 0x40000464, \
    0x40000a64, 0x40000468, 0x40000a68, 0x4000046c, \
    0x40000a6c, 0x40000470, 0x40000a70, 0x40000474, \
    0x40000a74, 0x40000478, 0x40000a78, 0x4000047c, \
    0x40000a7c, 0x40000480, 0x40000a80, 0x40000484, \
    0x40000a84, 0x40000488, 0x40000a88, 0x4000048c, \
    0x40000a8c, 0x40000490, 0x40000a90, 0x40000494, \
    0x40000a94, 0x40000498, 0x40000a98, 0x4000049c, \
    0x40000a9c, 0x400004a0, 0x40000aa0, 0x400004a4, \
    0x40000aa4, 0x400004a8, 0x40000aa8, 0x400004ac, \
    0x40000aac, 0x400004b0, 0x40000ab0, 0x400004b4, \
    0x40000ab4, 0x400004b8, 0x40000ab8, 0x400004bc, \
    0x40000abc, 0x400004c0, 0x40000ac0, 0x400004c4, \
    0x40000ac4, 0x400004c8, 0x40000ac8, 0x400004cc, \
    0x40000acc, 0x400004d0, 0x40000ad0, 0x400004d4, \
    0x40000ad4, 0x400004d8, 0x40000ad8, 0x400004dc, \
    0x40000adc, 0x400004e0, 0x40000ae0, 0x400004e4, \
    0x40000ae4, 0x400004e8, 0x40000ae8, 0x400004ec, \
    0x40000aec, 0x400004f0, 0x40000af0, 0x400004f4, \
    0x40000af4, 0x400004f8, 0x40000af8, 0x400004fc, \
    0x40000afc, 0x40000500, 0x40000b00, 0x40000504, \
    0x40000b04, 0x40000508, 0x40000b08, 0x4000050c, \
    0x40000b0c, 0x40000510, 0x40000b10, 0x40000514, \
    0x40000b14, 0x40000518, 0x40000b18, 0x4000051c, \
    0x40000b1c, 0x40000520, 0x40000b20, 0x40000524, \
    0x40000b24, 0x40000528, 0x40000b28, 0x4000052c, \
    0x40000b2c, 0x40000530, 0x40000b30, 0x40000534, \
    0x40000b34, 0x40000538, 0x40000b38, 0x4000053c, \
    0x40000b3c, 0x40000540, 0x40000b40, 0x40000544, \
    0x40000b44, 0x40000548, 0x40000b48, 0x4000054c, \
    0x40000b4c, 0x40000550, 0x40000b50, 0x40000554, \
    0x40000b54, 0x40000558, 0x40000b58, 0x4000055c, \
    0x40000b5c, 0x40000560, 0x40000b60, 0x40000564, \
    0x40000b64, 0x40000568, 0x40000b68, 0x4000056c, \
    0x40000b6c, 0x40000570, 0x40000b70, 0x40000574, \
    0x40000b74, 0x40000578, 0x40000b78, 0x4000057c, \
    0x40000b7c, 0x40000580, 0x40000b80, 0x40000584, \
    0x40000b84, 0x40000588, 0x40000b88, 0x4000058c, \
    0x40000b8c, 0x40000590, 0x40000b90, 0x40000594, \
    0x40000b94, 0x40000598, 0x40000b98, 0x4000059c, \
    0x40000b9c, 0x400005a0, 0x40000ba0, 0x400005a4, \
    0x40000ba4, 0x400005a8, 0x40000ba8, 0x400005ac, \
    0x40000bac, 0x400005b0, 0x40000bb0, 0x400005b4, \
    0x40000bb4, 0x400005b8, 0x40000bb8, 0x400005bc, \
    0x40000bbc, 0x400005c0, 0x40000bc0, 0x400005c4, \
    0x40000bc4, 0x400005c8, 0x40000bc8, 0x400005cc, \
    0x40000bcc, 0x400005d0, 0x40000bd0, 0x400005d4, \
    0x40000bd4, 0x400005d8, 0x40000bd8, 0x400005dc, \
    0x40000bdc, 0x400005e0, 0x40000be0, 0x400005e4, \
    0x40000be4, 0x400005e8, 0x40000be8, 0x400005ec, \
    0x40000bec, 0x400005f0, 0x40000bf0, 0x400005f4, \
    0x40000bf4, 0x400005f8, 0x40000bf8, 0x400005fc, \
    0x40000bfc, 0x40000600, 0x40000c00, \
  } \
};

#endif/*__GUARD_H101_WASA_EXT_H101_WASA_H__*/

/*******************************************************/