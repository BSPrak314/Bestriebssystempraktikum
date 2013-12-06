#define PIOB 0xfffff600
#define PIOC 0xfffff800

#define YELLOW_LED (1 << 27)
#define RED_LED 1
#define GREEN_LED 2

struct  pio {
	unsigned int per;
	unsigned int pdr;
	unsigned int psr;
	unsigned int unused0[1];
	unsigned int oer;
	unsigned int odr;
	unsigned int osr;
	unsigned int unused1[5];
	unsigned int sodr;
	unsigned int codr;
};

static volatile
struct pio * const piob = (struct pio *)PIOB;
struct pio * const pioc = (struct pio *)PIOC;

void yellow_on(void)
{
	piob->per = YELLOW_LED;
	piob->oer = YELLOW_LED;
	piob->sodr = YELLOW_LED;
}

void red_on(void)
{
	pioc->per = RED_LED;
	pioc->oer = RED_LED;
	pioc->sodr = RED_LED;
}

void green_on(void)
{
	pioc->per = GREEN_LED;
	pioc->oer = GREEN_LED;
	pioc->sodr = GREEN_LED;
}

void yellow_off(void)
{
	piob->per = YELLOW_LED;
	piob->oer = YELLOW_LED;
	piob->codr = YELLOW_LED;
}

void red_off(void)
{
	pioc->per = RED_LED;
	pioc->oer = RED_LED;
	pioc->codr = RED_LED;
}

void green_off(void)
{
	pioc->per = GREEN_LED;
	pioc->oer = GREEN_LED;
	pioc->codr = GREEN_LED;
}