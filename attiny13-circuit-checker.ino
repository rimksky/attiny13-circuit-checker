/*-------------------------------------------------*/
/* In-circuit checker   (C)ChaN, 2008              */
/*     http://elm-chan.org/works/cch/report.html   */
/*     importing Arduino IDE by rimksky@gmail.com  */
/*-------------------------------------------------*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define  SYSCLK    128000    /* CKSEL[1:0]=11, CKDIV8=1 */

EMPTY_INTERRUPT(PCINT0_vect);

void delay_ms (uint16_t dly)
{
  uint8_t n, d;

  do {
    n = 31;
    do d = PINB; while (--n);
  } while (--dly);
}

void beep (uint8_t t)
{
  if (t) {
    OCR0A = t;
    OCR0B = t / 2;
    TCCR0A = _BV(COM0B1)|_BV(WGM01)|_BV(WGM00);
    t = _BV(WGM02)|0b001;
  }
  TCCR0B = t;
}

void setup() {
  uint16_t ad, sdt;
  uint8_t lvd;
  static const int8_t tone[] = {
    SYSCLK / 4000,  /* 0..5 ohm */
    SYSCLK / 2640,  /* 5..15 ohm */
    SYSCLK / 1742,  /* 15..25 ohm */
    SYSCLK / 1150,  /* 25..35 ohm */
    SYSCLK / 760  /* 35..45 ohm */
  };

  PORTB = 0b00101;    /* Initialize I/O port */
  DDRB  = 0b01011;    /* BP0=LED, PB1=BZ, PB2=SW, PB3=Bias, BP4=ADC */

  ACSR = _BV(ACD);    /* Disable analog comp. */

  PCMSK = _BV(PCINT2);  /* Enable PCINT2 */
  GIMSK = _BV(PCIE);
  sei();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    beep(tone[1]);          /* Startup beep */
    delay_ms(1000);
    beep(0);
    beep(tone[2]);          /* Startup beep */
    delay_ms(1000);
    beep(0);
    beep(tone[3]);          /* Startup beep */
    delay_ms(1000);
    beep(0);
    
  for (;;) {
    PORTB = 0b00101;        /* LED OFF, Disable bias circuit */
    ADCSRA = 0;           /* Stop ADC */
    while (bit_is_clear(PINB,PB2))  /* Wait for button released */
    delay_ms(20);
    sleep_mode();         /* Enter Power-Down mode */
    delay_ms(50);         /* After leaving Power-Down mode, delay 50ms */
    if (bit_is_set(PINB,PB2))   /* When it is accidental wakeup, re-enter Power-Down mode */
    continue;
    PORTB = 0b01100;        /* LED ON, Enable bias circuit */
    ADMUX = _BV(REFS0)|0b10;    /* ADC Ch=2, Vref=1.1V */
    sdt = 0;            /* Initialize shutdown timer */
    while (bit_is_clear(PINB,PB2))  /* Wait for button released */
    delay_ms(20);

    ADCSRA = _BV(ADEN)|_BV(ADSC); /* Get Vbat (voltage on open circuit) */
    delay_ms(1);
    ad = ADC;
    if (ad < 320) continue;     /* When Vbat < 2.0V or probe shorted, shutdown */
    if (ad < 384) lvd = 1;      /* When Vbat < 2.4V, indicate LBT state (blink LED). */
    else lvd = 0;
    beep(tone[0]);          /* Startup beep */
    delay_ms(100);
    beep(0);

    do {
      ADCSRA = _BV(ADEN)|_BV(ADSC); /* Start conversion */
      delay_ms(10);         /* 10ms */
      ad = ADC;           /* Get input voltage */
      if (ad < 5) {
        beep(tone[ad]);       /* Start beep at frequency corresponds to the resistance */
        sdt = 0;          /* Clear shutdown timer */
        } else {
        beep(0);          /* Sto beep */
      }
      if (lvd && ++lvd > 20) {    /* Blink LED if needed */
        lvd = 1;
        PINB = _BV(PB0);
      }
    } while (bit_is_set(PINB,PB2) && ++sdt < 30000);

    if (bit_is_set(PINB,PB2)) {   /* Beep on auto power off */
      beep(tone[0]);
      delay_ms(500);
    }
    beep(0);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}


