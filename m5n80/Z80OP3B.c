/**--------------------------------------------------------------------------
 **
 **		Z80 Emulator Ver 3.00
 **		Copyright (C)1993-2010 H.Kanemoto
 **
 **		This is a free software. You may copy or distribute
 **		it without any permissions. But there are NO WARRANTY
 **		of use. Entire risk of use is with you.
 **
 **------------------------------------------------------------------------*/
#include "z80.h"
#include "z80mac.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define export
/**--------------------------------------------------------------------------
 **		Decoder
 **------------------------------------------------------------------------*/
export void _z80_decoder_xd_cb(Z80CPU *cpu, int IXY) {
	LPZ80REGS	r = &cpu->r;
	USHORT		idx = __z80_index(r->xr[IXY], __z80_fetchb(cpu));
	BYTE		b = __z80_mbread(cpu, idx);

	switch (__z80_fetchb(cpu)) {
	case 0x00: /* RLC (IX+d),B*/__z80_microcode_rlc(&b, &r->h.F); r->h.B = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x01: /* RLC (IX+d),C*/__z80_microcode_rlc(&b, &r->h.F); r->h.C = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x02: /* RLC (IX+d),D*/__z80_microcode_rlc(&b, &r->h.F); r->h.D = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x03: /* RLC (IX+d),E*/__z80_microcode_rlc(&b, &r->h.F); r->h.E = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x04: /* RLC (IX+d),H*/__z80_microcode_rlc(&b, &r->h.F); r->h.H = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x05: /* RLC (IX+d),L*/__z80_microcode_rlc(&b, &r->h.F); r->h.L = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x06: /* RLC (IX+d)  */__z80_microcode_rlc(&b, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x07: /* RLC (IX+d),A*/__z80_microcode_rlc(&b, &r->h.F); r->h.A = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0x08: /* RRC (IX+d),B*/__z80_microcode_rrc(&b, &r->h.F); r->h.B = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x09: /* RRC (IX+d),C*/__z80_microcode_rrc(&b, &r->h.F); r->h.C = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x0A: /* RRC (IX+d),D*/__z80_microcode_rrc(&b, &r->h.F); r->h.D = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x0B: /* RRC (IX+d),E*/__z80_microcode_rrc(&b, &r->h.F); r->h.E = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x0C: /* RRC (IX+d),H*/__z80_microcode_rrc(&b, &r->h.F); r->h.H = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x0D: /* RRC (IX+d),L*/__z80_microcode_rrc(&b, &r->h.F); r->h.L = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x0E: /* RRC (IX+d)  */__z80_microcode_rrc(&b, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x0F: /* RRC (IX+d),A*/__z80_microcode_rrc(&b, &r->h.F); r->h.A = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0x10: /* RL (IX+d),B*/	__z80_microcode_rl(&b, &r->h.F); r->h.B = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x11: /* RL (IX+d),C*/	__z80_microcode_rl(&b, &r->h.F); r->h.C = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x12: /* RL (IX+d),D*/	__z80_microcode_rl(&b, &r->h.F); r->h.D = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x13: /* RL (IX+d),E*/	__z80_microcode_rl(&b, &r->h.F); r->h.E = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x14: /* RL (IX+d),H*/	__z80_microcode_rl(&b, &r->h.F); r->h.H = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x15: /* RL (IX+d),L*/	__z80_microcode_rl(&b, &r->h.F); r->h.L = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x16: /* RL (IX+d)  */	__z80_microcode_rl(&b, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x17: /* RL (IX+d),A*/	__z80_microcode_rl(&b, &r->h.F); r->h.A = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0x18: /* RR (IX+d),B*/	__z80_microcode_rr(&b, &r->h.F); r->h.B = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x19: /* RR (IX+d),C*/	__z80_microcode_rr(&b, &r->h.F); r->h.C = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x1A: /* RR (IX+d),D*/	__z80_microcode_rr(&b, &r->h.F); r->h.D = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x1B: /* RR (IX+d),E*/	__z80_microcode_rr(&b, &r->h.F); r->h.E = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x1C: /* RR (IX+d),H*/	__z80_microcode_rr(&b, &r->h.F); r->h.H = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x1D: /* RR (IX+d),L*/	__z80_microcode_rr(&b, &r->h.F); r->h.L = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x1E: /* RR (IX+d)  */	__z80_microcode_rr(&b, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x1F: /* RR (IX+d),A*/	__z80_microcode_rr(&b, &r->h.F); r->h.A = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0x20: /* SLA (IX+d),B*/__z80_microcode_sla(&b, &r->h.F); r->h.B = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x21: /* SLA (IX+d),C*/__z80_microcode_sla(&b, &r->h.F); r->h.C = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x22: /* SLA (IX+d),D*/__z80_microcode_sla(&b, &r->h.F); r->h.D = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x23: /* SLA (IX+d),E*/__z80_microcode_sla(&b, &r->h.F); r->h.E = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x24: /* SLA (IX+d),H*/__z80_microcode_sla(&b, &r->h.F); r->h.H = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x25: /* SLA (IX+d),L*/__z80_microcode_sla(&b, &r->h.F); r->h.L = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x26: /* SLA (IX+d)  */__z80_microcode_sla(&b, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x27: /* SLA (IX+d),A*/__z80_microcode_sla(&b, &r->h.F); r->h.A = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0x28: /* SRA (IX+d),B*/__z80_microcode_sra(&b, &r->h.F); r->h.B = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x29: /* SRA (IX+d),C*/__z80_microcode_sra(&b, &r->h.F); r->h.C = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x2A: /* SRA (IX+d),D*/__z80_microcode_sra(&b, &r->h.F); r->h.D = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x2B: /* SRA (IX+d),E*/__z80_microcode_sra(&b, &r->h.F); r->h.E = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x2C: /* SRA (IX+d),H*/__z80_microcode_sra(&b, &r->h.F); r->h.H = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x2D: /* SRA (IX+d),L*/__z80_microcode_sra(&b, &r->h.F); r->h.L = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x2E: /* SRA (IX+d)  */__z80_microcode_sra(&b, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x2F: /* SRA (IX+d),A*/__z80_microcode_sra(&b, &r->h.F); r->h.A = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0x30: /* SLL (IX+d),B*/__z80_microcode_sll(&b, &r->h.F); r->h.B = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x31: /* SLL (IX+d),C*/__z80_microcode_sll(&b, &r->h.F); r->h.C = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x32: /* SLL (IX+d),D*/__z80_microcode_sll(&b, &r->h.F); r->h.D = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x33: /* SLL (IX+d),E*/__z80_microcode_sll(&b, &r->h.F); r->h.E = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x34: /* SLL (IX+d),H*/__z80_microcode_sll(&b, &r->h.F); r->h.H = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x35: /* SLL (IX+d),L*/__z80_microcode_sll(&b, &r->h.F); r->h.L = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x36: /* SLL (IX+d)  */__z80_microcode_sll(&b, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x37: /* SLL (IX+d),A*/__z80_microcode_sll(&b, &r->h.F); r->h.A = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0x38: /* SRL (IX+d),B*/__z80_microcode_srl(&b, &r->h.F); r->h.B = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x39: /* SRL (IX+d),C*/__z80_microcode_srl(&b, &r->h.F); r->h.C = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x3A: /* SRL (IX+d),D*/__z80_microcode_srl(&b, &r->h.F); r->h.D = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x3B: /* SRL (IX+d),E*/__z80_microcode_srl(&b, &r->h.F); r->h.E = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x3C: /* SRL (IX+d),H*/__z80_microcode_srl(&b, &r->h.F); r->h.H = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x3D: /* SRL (IX+d),L*/__z80_microcode_srl(&b, &r->h.F); r->h.L = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x3E: /* SRL (IX+d)  */__z80_microcode_srl(&b, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	case 0x3F: /* SRL (IX+d),A*/__z80_microcode_srl(&b, &r->h.F); r->h.A = b; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0x40:
	case 0x41:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x45:
	case 0x46:
	case 0x47: /* BIT 0,(IX+d)*/__z80_microcode_bit(b, 0x01, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 20; break;

	case 0x48:
	case 0x49:
	case 0x4A:
	case 0x4B:
	case 0x4C:
	case 0x4D:
	case 0x4E:
	case 0x4F: /* BIT 1,(IX+d)*/__z80_microcode_bit(b, 0x02, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 20; break;

	case 0x50:
	case 0x51:
	case 0x52:
	case 0x53:
	case 0x54:
	case 0x55:
	case 0x56:
	case 0x57: /* BIT 2,(IX+d)*/__z80_microcode_bit(b, 0x04, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 20; break;

	case 0x58:
	case 0x59:
	case 0x5A:
	case 0x5B:
	case 0x5C:
	case 0x5D:
	case 0x5E:
	case 0x5F: /* BIT 3,(IX+d)*/__z80_microcode_bit(b, 0x08, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 20; break;

	case 0x60:
	case 0x61:
	case 0x62:
	case 0x63:
	case 0x64:
	case 0x65:
	case 0x66:
	case 0x67: /* BIT 4,(IX+d)*/__z80_microcode_bit(b, 0x10, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 20; break;

	case 0x68:
	case 0x69:
	case 0x6A:
	case 0x6B:
	case 0x6C:
	case 0x6D:
	case 0x6E:
	case 0x6F: /* BIT 5,(IX+d)*/__z80_microcode_bit(b, 0x20, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 20; break;

	case 0x70:
	case 0x71:
	case 0x72:
	case 0x73:
	case 0x74:
	case 0x75:
	case 0x76:
	case 0x77: /* BIT 6,(IX+d)*/__z80_microcode_bit(b, 0x40, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 20; break;

	case 0x78:
	case 0x79:
	case 0x7A:
	case 0x7B:
	case 0x7C:
	case 0x7D:
	case 0x7E:
	case 0x7F: /* BIT 7,(IX+d)*/__z80_microcode_bit(b, 0x80, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 20; break;

	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
	case 0x84:
	case 0x85:
	case 0x86:
	case 0x87: /* RES 0,(IX+d)*/b &= 0xfe; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0x88:
	case 0x89:
	case 0x8A:
	case 0x8B:
	case 0x8C:
	case 0x8D:
	case 0x8E:
	case 0x8F: /* RES 1,(IX+d)*/b &= 0xfd; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97: /* RES 2,(IX+d)*/b &= 0xfb; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0x98:
	case 0x99:
	case 0x9A:
	case 0x9B:
	case 0x9C:
	case 0x9D:
	case 0x9E:
	case 0x9F: /* RES 3,(IX+d)*/b &= 0xf7; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0xA0:
	case 0xA1:
	case 0xA2:
	case 0xA3:
	case 0xA4:
	case 0xA5:
	case 0xA6:
	case 0xA7: /* RES 4,(IX+d)*/b &= 0xef; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0xA8:
	case 0xA9:
	case 0xAA:
	case 0xAB:
	case 0xAC:
	case 0xAD:
	case 0xAE:
	case 0xAF: /* RES 5,(IX+d)*/b &= 0xdf; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0xB0:
	case 0xB1:
	case 0xB2:
	case 0xB3:
	case 0xB4:
	case 0xB5:
	case 0xB6:
	case 0xB7: /* RES 6,(IX+d)*/b &= 0xbf; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0xB8:
	case 0xB9:
	case 0xBA:
	case 0xBB:
	case 0xBC:
	case 0xBD:
	case 0xBE:
	case 0xBF: /* RES 7,(IX+d)*/b &= 0x7f; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0xC0:
	case 0xC1:
	case 0xC2:
	case 0xC3:
	case 0xC4:
	case 0xC5:
	case 0xC6:
	case 0xC7: /* SET 0,(IX+d)*/b |= 0x01; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0xC8:
	case 0xC9:
	case 0xCA:
	case 0xCB:
	case 0xCC:
	case 0xCD:
	case 0xCE:
	case 0xCF: /* SET 1,(IX+d)*/b |= 0x02; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0xD0:
	case 0xD1:
	case 0xD2:
	case 0xD3:
	case 0xD4:
	case 0xD5:
	case 0xD6:
	case 0xD7: /* SET 2,(IX+d)*/b |= 0x04; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0xD8:
	case 0xD9:
	case 0xDA:
	case 0xDB:
	case 0xDC:
	case 0xDD:
	case 0xDE:
	case 0xDF: /* SET 3,(IX+d)*/b |= 0x08; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0xE0:
	case 0xE1:
	case 0xE2:
	case 0xE3:
	case 0xE4:
	case 0xE5:
	case 0xE6:
	case 0xE7: /* SET 4,(IX+d)*/b |= 0x10; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0xE8:
	case 0xE9:
	case 0xEA:
	case 0xEB:
	case 0xEC:
	case 0xED:
	case 0xEE:
	case 0xEF: /* SET 5,(IX+d)*/b |= 0x20; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0xF0:
	case 0xF1:
	case 0xF2:
	case 0xF3:
	case 0xF4:
	case 0xF5:
	case 0xF6:
	case 0xF7: /* SET 6,(IX+d)*/b |= 0x40; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;

	case 0xF8:
	case 0xF9:
	case 0xFA:
	case 0xFB:
	case 0xFC:
	case 0xFD:
	case 0xFE:
	case 0xFF: /* SET 7,(IX+d)*/b |= 0x80; __z80_mbwrite(cpu, idx, b); r->x.T = 23; break;
	}
}

#ifdef __cplusplus
}	// extern "C"
#endif
