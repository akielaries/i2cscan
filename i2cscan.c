#include "i2cscan.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <sys/ioctl.h>

#define LINUX_I2C_SLAVE 0x0703          // SLAVE ADDRESS 
#define VERSION         "1.0.0+git"     // VERSION

// address info struct?

// scan for available I2C busses
void bus_scan() {
    printf("SCAN FOR I2C BUSSES\n");
}

// find available addresses on a given I2C bus
void addr_scan(const char *bus, int min, int max) {
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
        if (ioctl(file, LINUX_I2C_SLAVE, address) < 0) {
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
void fetch_all() {
    printf("get every I2C bus and every address on them\n");
}

void display_device_contents(const char *dev_path) {
    FILE *dev_file = fopen(dev_path, "r");

    if (dev_file == NULL) {
        perror("Error opening device file");
        exit(EXIT_FAILURE);
    }

    char buffer[256];

    // Read and print contents until the end of file
    while (fgets(buffer, sizeof(buffer), dev_file) != NULL) {
        if (strstr(buffer, "No devices found") != NULL) {
            printf("No devices found!\n");
            exit(EXIT_FAILURE);
        }

        if (strstr(buffer, "Scanning I2C bus at 100 kHz") != NULL) {
            printf("SCANNING\n");
            continue;
        }
        if (strstr(buffer, "")) {
            printf("%s", buffer);
            exit(EXIT_SUCCESS);
        }
    }

    fclose(dev_file);
}

void compile_and_flash(const char *mmcu, const char *dev_path) {
    // Define the compilation and programming commands
    char compile_cmd[200];
    char flash_cmd[200];
    char obj_cpy[200];
    char cat_dev[200];

    snprintf(compile_cmd, sizeof(compile_cmd),
             "avr-gcc -Wno-cpp -Wall -Os -mmcu=%s -DF_CPU=16000000UL -DBAUD=115200 -o i2c.elf i2c.c", mmcu);

    snprintf(obj_cpy, sizeof(obj_cpy),
             "avr-objcopy -j .text -j .data -O ihex i2c.elf i2c.hex");

    snprintf(flash_cmd, sizeof(flash_cmd),
             "avrdude -D -c wiring -p %s -P %s -b 115200 -U flash:w:i2c.hex", mmcu, dev_path);

    // Compile the code
    if (system(compile_cmd) != 0) {
        fprintf(stderr, "Compilation failed\n");
        exit(EXIT_FAILURE);
    }

    // Program the microcontroller
    if (system(flash_cmd) != 0) {
        fprintf(stderr, "Programming failed\n");
        exit(EXIT_FAILURE);
    }

    snprintf(cat_dev, sizeof(cat_dev), "cat %s", dev_path);
}

// i2cscan binary CLI usage
void usage(const char *bin) {
    printf("Usage: %s [-h] [-v] [-mmcu MCU] [-l] [-f] [-b] BUS [-a | -r FIRST-LAST]\n", bin);
    printf("    - BUS is an integer representing the I2C bus name\n");
    printf("    - FIRST LAST represents the MIN/MAX addresses to scan\n");
    printf("    - MCU given AVR device\n");
}

void help() {
    printf("\nAvailable options:\n"
            "   -h help\n"
            "   -v version\n"
            "   -l lists all available I2C busses\n"
            "   -f fetch all busses & available addresses\n"
            "   -b [BUS] specify I2C bus\n"
            "   -a scan all addresses\n"
            "   -r [FIRST-LAST] scan addresses between FIRST & LAST (int)\n"
            "   ** For MCUs **\n"
            "   -mmcu [MCU]\n"
            "   Supported MCUs :\n"
            "       - atmega328\n"
            "       - atmega2560\n"
            "       - stm32?\n"
            "\n");
}

// main driver featuring cmd line parsing
int main (int argc, char *argv[]) {
    /* FLAG VARIABLES */
    int flag_bus = 0, 
        flag_all_addrs = 0, 
        flag_range_addrs = 0, 
        flag_list_busses =0,
        flag_fetch_all = 0,
        flag_mmcu = 0,
        flag_dev = 0;
    int option;
    char bus_file[20];
    char mmcu_option[20] = "";
    char dev_path[20] = "";
    int MIN_ADDR = 0, MAX_ADDR = 127;

    if (argc < 2) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((option = getopt(argc, argv, "hvb:ar:m:d:")) != -1) {
        switch(option) {
            // help
            case 'h':
                help();
                exit(EXIT_SUCCESS);
                break;

            // version
            case 'v':
                printf("v%s\n", VERSION);
                exit(EXIT_SUCCESS);
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

            // all addresses TODO maybe -a should be for fetch_all and -b used the default range
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

            case 'm':
                {
                    char *mmcu_arg = strchr(optarg, '=');
                    if (mmcu_arg != NULL) {
                        // Check if the argument after '=' is empty
                        if (strlen(mmcu_arg + 1) == 0) {
                            fprintf(stderr, "Error: No microcontroller specified after -mmcu=\n");
                            exit(EXIT_FAILURE);
                        }
                        // Copy the argument after '=' into mmcu_option
                        snprintf(mmcu_option, sizeof(mmcu_option), "%s", mmcu_arg + 1);
                    } else {
                        fprintf(stderr, "Invalid -mmcu option format\n");
                        exit(EXIT_FAILURE);
                    }
                }
                flag_mmcu = 1;
                break;

            case 'd':
                {
                    char *dev_arg = strchr(optarg, '=');
                    if (dev_arg != NULL) {
                        // Copy the argument after '=' into dev_path
                        snprintf(dev_path, sizeof(dev_path), "%s", dev_arg + 1);
                    } else {
                        // If no equal sign, assume the next argument is the device path
                        if (optind < argc) {
                            snprintf(dev_path, sizeof(dev_path), "%s", argv[optind]);
                            optind++;  // Move to the next argument
                        } else {
                            fprintf(stderr, "Invalid -dev option format\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
                flag_dev = 1;
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

