#include <assert.h>
#include "minimap.h"
#include "ksw2.h"

void mm_align_skeleton(void *km, const mm_mapopt_t *opt, const mm_idx_t *mi, int qlen, const char *qstr, int n_regs, mm_reg1_t *regs, mm128_t *a)
{
	extern unsigned char seq_nt4_table[256];
	int i, reg;
	uint8_t *qseq0[2];

	qseq0[0] = (uint8_t*)kmalloc(km, qlen);
	qseq0[1] = (uint8_t*)kmalloc(km, qlen);
	for (i = 0; i < qlen; ++i) {
		qseq0[0][i] = seq_nt4_table[(uint8_t)qstr[i]];
		qseq0[1][qlen - 1 - i] = qseq0[0][i] < 4? 3 - qseq0[0][i] : 4;
	}

	for (reg = 0; reg < n_regs; ++reg) {
		mm_reg1_t *r = &regs[reg];
		int32_t rid = a[r->as].x<<1>>33, rev = a[r->as].x>>63;
		uint8_t *tseq;
		tseq = (uint8_t*)kmalloc(km, r->re - r->rs);
		for (i = 0; i < r->cnt - 1; ++i) {
			uint8_t *qseq;
			int32_t rs, re, qs, qe, ret;
			rs = (int32_t)a[r->as + i].x + 1;
			re = (int32_t)a[r->as + i + 1].x + 1;
			qs = (int32_t)a[r->as + i].y + 1;
			qe = (int32_t)a[r->as + i + 1].y + 1;
			qseq = &qseq0[rev][qs];
			ret = mm_idx_getseq(mi, rid, rs, re, tseq);
			assert(ret > 0);
		}
		kfree(km, tseq);
	}

	kfree(km, qseq0[0]); kfree(km, qseq0[1]);
}
