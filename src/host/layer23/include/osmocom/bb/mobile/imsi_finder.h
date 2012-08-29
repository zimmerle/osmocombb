#ifndef IMSI_FINDER_H
#define IMSI_FINDER_H

#include <sys/time.h>

struct imsi_finder {
	int on;
};


void imsi_finder_paging(struct osmocom_ms *ms, struct msgb *msg);

void start_imsi_finder(struct osmocom_ms *ms);

void stop_imsi_finder(struct osmocom_ms *ms);

#endif
