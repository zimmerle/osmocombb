#include <osmocom/bb/common/osmocom_data.h>
#include <osmocom/bb/mobile/vty.h>

#if 0
static float time_diff(struct timeval *t1, struct timeval *t2)
{
        float delta;

        delta = (t2->tv_sec - t1->tv_sec);
        delta += 0.000001 * (t2->tv_usec - t1->tv_usec);

        return delta;
}

static void timeout_tcatcher(void *arg)
{
        struct osmocom_ms *ms = arg;
        struct catcher_status *cs = &ms->catch_stat;
	float delta;

        gettimeofday(&cs->stop_time, 0);

	delta = time_diff(&cs->start_time, &cs->stop_time);

	vty_notify(ms, "delta %f current %d\n", delta, cs->current);

	switch(cs->current) {
	case CATCH_IDLE:
		return;
	case CATCH_TRY:
		if (delta > 2.0) {
			if (cs->flag < 2)
				cs->flag = 2;
		}
		break;
	case CATCH_EST:
		if (delta > 2.0) {
			if (cs->flag < 2)
				cs->flag = 2;
		}
		if (delta > 10.0) {
			if (cs->flag < 3)
				cs->flag = 3;
		}
		break;
	case CATCH_DATA:
		if (delta > 2.0) {
			if (cs->flag < 1)
				cs->flag = 1;
		}
		if (delta > 10.0) {
			if (cs->flag < 2)
				cs->flag = 2;
		}
		break;
	case CATCH_SERV:
			return;
		}

        osmo_timer_schedule(&cs->dcch_time, 0, 100000);
}

void start_tcatcher(struct osmocom_ms *ms)
{
        struct catcher_status *cs = &ms->catch_stat;

        cs->dcch_time.cb = timeout_tcatcher;
        cs->dcch_time.data = ms;

        gettimeofday(&ms->catch_stat.start_time, 0);

        osmo_timer_schedule(&cs->dcch_time, 0, 100000);
}

void stop_tcatcher(struct osmocom_ms *ms)
{
        struct catcher_status *cs = &ms->catch_stat;

        if (osmo_timer_pending(&cs->dcch_time)) {
                osmo_timer_del(&cs->dcch_time);
        }
}

#endif

#define IMSI_FINDER_FILE "/tmp/imsi_finder.txt"

void imsi_finder_look_mi(struct osmocom_ms *ms, uint8_t *mi, struct gsm48_paging1 *pa)
{
	uint8_t mi_type;
	char imsi[16];
	uint32_t tmsi;
	FILE *fp;

	

	if (mi[0] < 1)
		return;


	fp = fopen(IMSI_FINDER_FILE, "a");

	mi_type = mi[1] & GSM_MI_TYPE_MASK;

	switch (mi_type) {
	case GSM_MI_TYPE_TMSI:
		if (mi[0] < 5)
			return;
		memcpy(&tmsi, mi+2, 4);
//		if (pa->cneed1)
//		{
			vty_notify(ms, "1 - TMSI %08x %d\n", ntohl(tmsi), pa->cneed1);
			fprintf(fp, "%08x\n", ntohl(tmsi));
//		}
		break;
	case GSM_MI_TYPE_IMSI:
		gsm48_mi_to_string(imsi, sizeof(imsi), mi + 1, mi[0]);
//		if (pa->cneed1)
//		{
			vty_notify(ms, "1 - IMSI %s %d\n", imsi, pa->cneed1);
			fprintf(fp, "%s\n", imsi);
//		}
		break;
//	default:
//		vty_notify(ms, "FELIPE Paging with unsupported MI type %d.\n", mi_type);
	
	}

	fclose(fp);

}

void imsi_finder_paging_1(struct osmocom_ms *ms, struct msgb *msg)
{
	uint8_t *mi;
	struct imsi_finder *imf = &ms->imsi_finder;
	struct gsm48_paging1 *pa = msgb_l3(msg);

	if (imf->on == 0)
		return;

	mi = pa->data;
	imsi_finder_look_mi(ms, mi, pa);

	mi = pa->data + mi[0] + 1;
	imsi_finder_look_mi(ms, mi + 1, pa);

}

/* 9.1.23 PAGING REQUEST 2 message received */
void imsi_finder_paging_2(struct osmocom_ms *ms, struct msgb *msg)
{
	struct gsm48_paging2 *pa = msgb_l3(msg);
	struct imsi_finder *imf = &ms->imsi_finder;
	uint8_t *mi, mi_type;
	FILE *fp;

	fp = fopen(IMSI_FINDER_FILE, "a");

	if (imf->on == 0)
		return;
/*
	if (payload_len < 0) {
		short_read:
		return;
	}
*/
	/* first MI */

//	if (pa->cneed1)
//	{
		vty_notify(ms, "2 - TMSI %08x %d\n", ntohl(pa->tmsi1), pa->cneed1);
		fprintf(fp, "%08x\n", ntohl(pa->tmsi1));
//	}

	/* second MI */
//	if (pa->cneed2)
//	{
		vty_notify(ms, "2 - TMSI %08x %d\n", ntohl(pa->tmsi2), pa->cneed2);
		fprintf(fp, "%08x\n", ntohl(pa->tmsi2));
//	}

	/* third MI */
	mi = pa->data;
/*
	if (payload_len < 2)
		return;
*/

/*
	if (mi[0] != GSM48_IE_MOBILE_ID)
		return;

	if (payload_len < mi[1] + 2 + 1) /* must include "channel needed"
		return;
*/
	/*
	chan_3 = mi[mi[1] + 2] & 0x03; /* channel needed
	if ((mi_type = gsm_match_mi(ms, mi + 1)) > 0)
		return gsm48_rr_chan_req(ms, gsm48_rr_chan2cause[chan_3], 1,
			mi_type);
	*/

	fclose(fp);

	return;
}

/* 9.1.24 PAGING REQUEST 3 message received */
void imsi_finder_paging_3(struct osmocom_ms *ms, struct msgb *msg)
{
	struct gsm48_paging3 *pa = msgb_l3(msg);
	struct imsi_finder *imf = &ms->imsi_finder;
	FILE *fp;

	fp = fopen(IMSI_FINDER_FILE, "a");

	if (imf->on == 0)
		return;


//	if (payload_len < 0) { /* must include "channel needed", part of *pa */
//		return;
//	}

	/* first MI */
//	if (pa->cneed1)
//	{
		vty_notify(ms, "3 - TMSI %08x %d\n", ntohl(pa->tmsi1), pa->cneed1);
		fprintf(fp, "%08x\n", ntohl(pa->tmsi1));
//	}

	/* second MI */
//	if (pa->cneed2)
//	{
		vty_notify(ms, "3 - TMSI %08x %d\n", ntohl(pa->tmsi2), pa->cneed2);
		fprintf(fp, "%08x\n", ntohl(pa->tmsi2));
//	}

	/* thrid MI */
//	if (pa->cneed3)
//	{
		vty_notify(ms, "3 - TMSI %08x %d\n", ntohl(pa->tmsi3), pa->cneed3);
		fprintf(fp, "%08x\n", ntohl(pa->tmsi3));	
//	}

	/* fourth MI */
//	if (pa->cneed4)
//	{
		vty_notify(ms, "3 - TMSI %08x %d\n", ntohl(pa->tmsi4), pa->cneed4);
		fprintf(fp, "%08x\n", ntohl(pa->tmsi4));
//	}

	fclose(fp);

	return;
}

void timeout_stop(struct osmocom_ms *ms)
{
	struct imsi_finder *imf = &ms->imsi_finder;

	vty_notify(ms, "imsi_finder stopping...\n");

        if (osmo_timer_pending(&imf->timer)) {
                osmo_timer_del(&imf->timer);
        }

	imf->on = 0;
}


void start_imsi_finder(struct osmocom_ms *ms)
{
	struct imsi_finder *imf = &ms->imsi_finder;

	if (imf->on == 1)
		goto already_on;

	vty_notify(ms, "imsi_finder starting...\n");

        imf->timer.cb = timeout_stop;
        imf->timer.data = ms;

	//osmo_timer_schedule(&imf->timer, 10, 0);


	imf->on = 1;
	return;

already_on:
	vty_notify(ms, "imsi_finder is already working, please wait\n");
	imf->on = 0;
}

void stop_imsi_finder(struct osmocom_ms *ms)
{
	struct imsi_finder *imf = &ms->imsi_finder;

        if (osmo_timer_pending(&imf->timer)) {
                osmo_timer_del(&imf->timer);
        }

	imf->on = 0;
}


