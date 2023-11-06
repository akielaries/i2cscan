#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <getopt.h>

#define VERSION     "0.1.0+git"     // VERSION

// if linux
#if defined(__linux__)
#define I2C_SLAVE	0x0703	        // SLAVE ADDRESS
#endif

// address info struct?

// scan for available I2C busses
static void bus_scan() {
    printf("SCAN FOR I2C BUSSES\n");
}

// find available addresses on a given I2C bus
static void addr_scan(const char *bus, int min, int max) {
    int file;

    // iterate through the specified I2C address range
    for (int address = min; address <= max; address++) {
        // attempt to open the I2C bus
        file = open(bus, O_RDWR);
        if (file < 0) {
            perror("Failed to open I2C bus");
            return;
        }

        // set the slave address for the current iteration
        if (ioctl(file, I2C_SLAVE, address) < 0) {
            close(file);
            continue; // no device at this address
        }

        // attempt to read one byte from the address to check if it responds
        unsigned char buffer;
        if (read(file, &buffer, 1) == 1) {
            // device found and responded, print the address
            printf("Found device at I2C address 0x%02X\n", address);
        }

        // close the file
        close(file);
    }
}

// fetch all I2C busses and their addresses
static void fetch_all() {
    printf("get every I2C bus and every address on them\n");
}

// i2cscan binary CLI usage
static void usage(const char *bin) {
    printf("Usage: %s [-h] [-v] [-l] [-f] [-b] BUS [-a | -r FIRST-LAST]\n", bin);
    printf("BUS is an integer representing the I2C bus name\n");
    printf("FIRST LAST represents the MIN_ADDR/MAX_ADDR addresses to scan\n\n");
}

static void help() {
    printf("\nAvailable options:\n"
            "   -h help\n"
            "   -v version\n"
            "   -l lists all available I2C busses\n"
            "   -f fetch all busses & available addresses\n"
            "   -b specify I2C bus\n"
            "   -a scan all addresses\n"
            "   -r scan addresses between FIRST & LAST\n"
            "\n");
}

// main driver featuring cmd line parsing
int main (int argc, char *argv[]) {
    int flag_bus = 0, 
        flag_all_addrs = 0, 
        flag_range_addrs = 0, 
        flag_list_busses =0,
        flag_fetch_all = 0;
    int option;
    char bus_file[20];
    int MIN_ADDR = 0, MAX_ADDR = 127;

    if (argc < 2) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((option = getopt(argc, argv, "hvb:ar:")) != -1) {
        switch(option) {
            // help
            case 'h':
                help();
                exit(EXIT_FAILURE);
                break;

            // version
            case 'v':
                printf("v%s\n", VERSION);
                exit(EXIT_FAILURE);
                break;

            case 'l':
                printf("list something?\n");
                flag_list_busses = 1;
                break;
            
            case 'f':
                printf("get everything!\n");
                flag_fetch_all = 1;
                break;
            
            // bus
            case 'b':
                snprintf(bus_file, sizeof(bus_file), "/dev/i2c-%s", optarg);
                // set flag for bus
                flag_bus = 1;
                break;

            // all addresses
            case 'a':
                if (!flag_bus) {
                    printf("an I2C bus must be specified with -b\n");
                    usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                // set flag for all addrs
                flag_all_addrs = 1;
                break;

            // range of addresses
            case 'r':
                if (!flag_bus) {
                    printf("an I2C bus must be specified with -b\n");
                    usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                if (sscanf(optarg, "%d-%d", &MIN_ADDR, &MAX_ADDR) != 2) {
                    fprintf(stderr, "Invalid range format. Use -r MIN_ADDR-MAX_ADDR\n");
                    exit(EXIT_FAILURE);
                }
                // set flag for range of addrs
                flag_range_addrs = 1;
                break;

            case '?':
                usage(argv[0]);
                exit(EXIT_FAILURE);
                break;
            
            default:
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // some error checking 
    if (flag_bus) {
        if (flag_all_addrs || flag_range_addrs) {
            printf("Scanning all addresses %d to %d\n", MIN_ADDR, MAX_ADDR);
            addr_scan(bus_file, MIN_ADDR, MAX_ADDR);
        }
        else {
            printf("-b requires -a for all addresses or -r for a range\n");
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    else {
        // FIXME
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;
}
