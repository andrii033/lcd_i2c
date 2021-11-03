#include "main.h"

#define DHT11_PIN 6
uint8_t c = 0, I_RH, D_RH, I_Temp, D_Temp, CheckSum;

void Request() /* Microcontroller send start pulse/request */
{
	DDRD |= (1 << DHT11_PIN);
	PORTD &= ~(1 << DHT11_PIN); /* set to low pin */
	_delay_ms(20);				/* wait for 20ms */
	PORTD |= (1 << DHT11_PIN);	/* set to high pin */
}

void Response() /* receive response from DHT11 */
{
	DDRD &= ~(1 << DHT11_PIN);
	while (PIND & (1 << DHT11_PIN))
		;
	while ((PIND & (1 << DHT11_PIN)) == 0)
		;
	while (PIND & (1 << DHT11_PIN))
		;
}

uint8_t Receive_data() /* receive data */
{
	for (int q = 0; q < 8; q++)
	{
		while ((PIND & (1 << DHT11_PIN)) == 0)
			; /* check received bit 0 or 1 */
		_delay_us(30);
		if (PIND & (1 << DHT11_PIN)) /* if high pulse is greater than 30ms */
			c = (c << 1) | (0x01);	 /* then its logic HIGH */
		else						 /* otherwise its logic LOW */
			c = (c << 1);
		while (PIND & (1 << DHT11_PIN))
			;
	}
	return c;
}

int main(void)
{
	DDRD=0b00100000;

	I2C_Init(); //initialize TWI
	LCD_ini();	//initialize the display
	clearlcd(); //clear the display
	setpos(0, 0);

	wdt_enable(WDTO_8S);


	char data[5];
	while (1)
	{
		

		Request();				 /* send start pulse */
		Response();				 /* receive response */
		I_RH = Receive_data();	 /* store first eight bit in I_RH */
		D_RH = Receive_data();	 /* store next eight bit in D_RH */
		I_Temp = Receive_data(); /* store next eight bit in I_Temp */
		D_Temp = Receive_data(); /* store next eight bit in D_Temp */
		CheckSum = Receive_data();
		if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
		{
			setpos(0, 0);
			str_lcd("Error");
		}

		else
		{

			setpos(0, 0);
			str_lcd("Humidity = ");
			setpos(0, 1);
			str_lcd("Temp = ");

			itoa(I_RH, data, 10);
			setpos(11, 0);
			str_lcd(data);
			str_lcd(".");

			if(I_RH>65)
			{
				PORTD |= (1 << (PORTD5));
			}
			if(I_RH<70)
			{
				PORTD &= ~(1 << (PORTD5));
			}
				

			itoa(D_RH, data, 10);
			str_lcd(data);
			str_lcd("%");

			itoa(I_Temp, data, 10);
			setpos(7, 1);
			str_lcd(data);
			str_lcd(".");

			itoa(D_Temp, data, 10);
			str_lcd(data);
			str_lcd("C ");
		}
		
		wdt_reset();
		_delay_ms(5000);
		clearlcd();
	}
}
