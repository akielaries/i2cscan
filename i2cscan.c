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

static void bus_scan() {
    printf("SCAN FOR I2C BUSSES\n");
}

//static void addr_scan(const char *bus,
//              unsigned char found_addrs[],
//              size_t *num_found_addrs) {
static void addr_scan(const char *bus, int min, int max) {
    int file;

    // Iterate through the specified I2C address range
    for (int address = min; address <= max; address++) {
        // Attempt to open the I2C bus
        file = open(bus, O_RDWR);
        if (file < 0) {
            perror("[!] Failed to open I2C bus");
            return;
        }

        // Set the slave address for the current iteration
        if (ioctl(file, I2C_SLAVE, address) < 0) {
            close(file);
            continue; // No device at this address
        }

        // Attempt to read one byte from the address to check if it responds
        unsigned char buffer;
        if (read(file, &buffer, 1) == 1) {
            // Device found and responded, print the address
            printf("Found device at I2C address 0x%02X\n", address);
        }

        // Close the file
        close(file);
    }
}

static void fetch_all() {
    printf("get every I2C bus and every address on them\n");
}

// i2cscan binary CLI usage
static void usage(const char *bin) {
    printf("Usage: %s [-v] [-b] BUS [-a | -r (FIRST LAST)]\n", bin);
    printf("BUS is an integer representing the I2C bus name\n");
    printf("FIRST LAST represents the MIN_ADDR/MAX_ADDR addresses to scan\n");
}

int main (int argc, char *argv[]) {
    int option;
    int bus = -1;
    //const char *bus_file;
    char bus_file[20];
    int flag_all_addrs, flag_range_addrs;
    int MIN_ADDR = 0, MAX_ADDR = 127;

    if (argc < 2) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((option = getopt(argc, argv, "vb:ar:")) != -1) {
        switch(option) {
            // version
            case 'v':
                printf("v%s\n", VERSION);
                break;

            // bus
            case 'b':
                bus = atoi(optarg);
                snprintf(bus_file, sizeof(bus_file), "/dev/i2c-%s", optarg);

                printf("Bus = %s\n", bus_file);
                break;

            // all addresses
            case 'a':
                printf("Scanning all addresses %d to %d\n", MIN_ADDR, MAX_ADDR);
                // set flag for all addrs
                flag_all_addrs = 1;
                break;

            // range of addresses
            case 'r':
                if (sscanf(optarg, "%d-%d", &MIN_ADDR, &MAX_ADDR) != 2) {
                    fprintf(stderr, "Invalid range format. Use -r MIN_ADDR-MAX_ADDR\n");
                    exit(EXIT_FAILURE);;
                }
                printf("Scanning addresses from %d to %d\n", MIN_ADDR, MAX_ADDR);
                // set flag for range of addrs
                flag_range_addrs = 1;
                break;

            case '?':
                usage(argv[0]);
                break;
            
            default:
                usage(argv[0]);
        }
    }

    // some error checking 
    if (bus == -1) {
        // FIXME
        fprintf(stderr, "FIXME Bus is required. Usage: %s -b BUS\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (flag_all_addrs || flag_range_addrs) {
        addr_scan(bus_file, MIN_ADDR, MAX_ADDR);
    }

    return 0;
}
