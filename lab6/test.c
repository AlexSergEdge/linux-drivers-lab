#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define HELLO_MAJOR 250

#define LEDCMD_RESET_STATE _IO(HELLO_MAJOR, 1)
#define LEDCMD_GET_STATE _IOR(HELLO_MAJOR, 2, unsigned char *)
#define LEDCMD_GET_LED_STATE _IOWR(HELLO_MAJOR, 3,  led_t *)
#define LEDCMD_SET_LED_STATE _IOW(HELLO_MAJOR, 4, led_t *)

/* define name of the drivers */
#define FILENAME "/dev/hello"

/* initial state of LEDs */
#define INITIAL_STATE 0x00

typedef struct led {
    int pin;
    unsigned char value;
} led_t;

#define STATE_ALL -1

//void print_usage(void);
//void led_reset(void);
//void led_state(int led_n);
//void led_off(int led_n);
//void led_on(int led_n);


void led_reset(void) {
    int file;
    /* open device /dev/hello */
    file = open(FILENAME, O_RDWR);
    if (file == -1) {
        fprintf(stderr, "Cannot open file %s\n", FILENAME);
        return;
    }

    if (ioctl(file, LEDCMD_RESET_STATE) == -1) {
        fprintf(stderr, "RESET ERROR!\n");
        return;
    }
    printf("Сброс индикаторов\n");
    close(file);
}

void led_state(int led_n) {
    int file;
    led_t led;
    /* state of all LEDs at once in one byte */
    char state;

    /* open device /dev/hello */
    file = open(FILENAME, O_RDWR);
    if (file == -1) {
        fprintf(stderr, "Cannot open file %s\n", FILENAME);
        return;
    }

    if (led_n == STATE_ALL) {
        if (ioctl(file, LEDCMD_GET_STATE, &state) == -1) {
            fprintf(stderr, "STATE ERROR\n");
            return;
        }
        for (led_n = 0 ; led_n < 8; led_n++) {
            printf("Светодиод %d %s\n", led_n,
                   (state & (1 << led_n)) ?
                   "включен" : "выключен");
        }
    } else {
        led.pin = led_n;
        if (ioctl(file, LEDCMD_GET_LED_STATE, &led) == -1) {
            fprintf(stderr, "LED STATE ERROR\n");
            return;
        }
        printf("Светодиод %d %s\n", led.pin,
               led.value ? "включен" : "выключен");
    }

    close(file);
}


void led_off(int led_n) {
    int file;
    led_t led;

    /* open device /dev/hello */
    if ((file = open(FILENAME, O_RDWR)) == -1) {
        fprintf(stderr, "Cannot open file %s\n", FILENAME);
        return;
    }

    led.pin = led_n;
    /* disable pin */
    led.value = 0;
    if (ioctl(file, LEDCMD_SET_LED_STATE, &led) == -1) {
        fprintf(stderr, "TURN OFF ERROR\n");
        return;
    } else {
        if (ioctl(file, LEDCMD_GET_LED_STATE, &led) == -1) {
            fprintf(stderr, "LED TURN OFF ERROR\n");
            return;
        }
        printf("Светодиод %d %s\n", led.pin, led.value ? "включен" : "выключен");
    }
    close(file);
}

void led_on(int led_n) {
    int file;
    led_t led;

    file = open(FILENAME, O_RDWR);
    if (file == -1) {
        fprintf(stderr, "Cannot open file %s\n", FILENAME);
        return;
    }

    led.pin = led_n;
    /* enable pin */
    led.value = 1;
    if (ioctl(file, LEDCMD_SET_LED_STATE, &led) == -1) {
        fprintf(stderr, "TURN ON ERROR\n");
        return;
    } else {
        if (ioctl(file, LEDCMD_GET_LED_STATE, &led) == -1) {
            fprintf(stderr, "TURN ON ERROR!\n");
            return;
        }
        printf("Светодиод %d %s\n", led.pin, led.value ?
                "включен" : "выключен");
    }
    close(file);
}

int main(int argc, char **argv) {
    char *reset = "reset";
    char *ledst= "ledstate";
    char *on = "on";
    char *off = "off";

    switch (argc) {

    case 2:
        if  (strcmp (argv[1], reset) == 0) {
            led_reset();
        } else if (strcmp (argv[1], ledst) == 0) {
            led_state(STATE_ALL);
        } else {
            printf("Usage: <reset> \n <ledstate/on/off> [N]\n");
        }
        break;

    case 3:
        if  (strcmp (argv[1], ledst) == 0) {
            led_state(atoi(argv[2]));
        } else if (strcmp (argv[1], off) == 0) {
            led_off(atoi(argv[2]));
        } else if (strcmp (argv[1], on) == 0) {
            led_on(atoi(argv[2]));
        }
        break;
    default:
        printf("Usage: <reset> \n <ledstate/on/off> [N]\n");
    }
    return 0;
}