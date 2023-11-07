#ifndef I2CSCAN_H
#define I2CSCAN_H

static void bus_scan();

static void addr_scan(const char *bus, int min, int max);

static void fetch_all();

static void usage(const char *bin);

static void help();

#endif
