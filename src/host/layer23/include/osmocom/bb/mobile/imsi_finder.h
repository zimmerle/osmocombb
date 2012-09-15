#ifndef IMSI_FINDER_H
#define IMSI_FINDER_H

#include <sys/time.h>

struct imsi_finder {
	int on;
	struct osmo_timer_list timer;
};


void imsi_finder_paging_1(struct osmocom_ms *ms, struct msgb *msg);
void imsi_finder_paging_2(struct osmocom_ms *ms, struct msgb *msg);
void imsi_finder_paging_3(struct osmocom_ms *ms, struct msgb *msg);

void start_imsi_finder(struct osmocom_ms *ms);

void stop_imsi_finder(struct osmocom_ms *ms);

#endif
