/*//Программа челнока на Захід Птиця!!! 28,10,2021
 і для Молдови Сергія Solar Nord

25.09.26 Створена гілка feature для лотків без 1 та 7-го конуса

//23.11.2021
	////Зроблено на виробництві для чіткої зупинки двигуна каретки
	//PORTB|=1<<M3_POW;
	//PORTD|=(1<<M3_BRAKE);
//
02.12.2021
	Додано плавну зупинку двигуна каретки головки
05.12.2021
	Додано плавну зупинку двигуна стрічки і плавний старт двигуна стрічки
	додано тестовий режим при замикнні END1 на спільний провід
	додано скидання лічильника рядів по перевищенню кроків двигуна стрічки
11.01.2022 
	Змінив відстань між яйцями в ряду з 39 на 41

*/

#define F_CPU 14.7456E6

#include <avr/io.h>
#include <avr/interrupt.h>

//#include <util/delay.h>


#define END1 	PA0 //Начало левого лотка 	PINA0
#define END2 	PA1	//Конец левого лотка	PINA1 Заведена кнопка перемикання режимів
#define	END3	PA2	//Начало правого лотка	PINA2
#define END4	PA3	//Конец правого лотка	PINA3

#define	END5	PA4	//Начало каретки левого лотка		PINA4
#define	END6	PA5	//Конец каретки левого лотка		PINA5
#define	END7	PA6	//Начало каретки правого лотка		PINA6
#define	END8	PA7	//Конец каретки правого лотка		PINA7
#define END9		PG2	//установка левого лотка		PING2
#define END10		PC7	//снятие левого лотка			PINC7
#define	END11		PC6	//установка правого лотка		PINC6
#define	END12		PC5	//снятие правого лотка			PINC5
#define	END13		PC4	//фотодатчик левого лотка		PINC4
#define	END14		PC3	//фотодатчик правого лотка		PINC3

#define	M1_POW		PB2	//
#define	M2_POW		PB3	//
#define	M3_POW		PB4	//

#define	M1_BRAKE	PD5	//
#define	M2_BRAKE	PD6	//
#define	M3_BRAKE	PD7	//

#define	M1_DIR		PC0//
#define	M2_DIR		PC1	//
#define	M3_DIR		PC2	//

#define	M1_PWM		PB5	//
#define	M2_PWM		PB6	//
#define	M3_PWM		PB7	//

#define	M1_ENC		PD0	//
#define	M2_ENC		PD1	//
#define	M3_ENC		PD2	//

#define	PRN			PG1	//
#define	DIR			PG0	//

#define PinEnd1	PINA	// Порт датчиков END 1-8
#define PinEnd2 PINC	// Порт датчиков END 10-14

#define EGGS_WIDTH	41	//расстояние между яйцами в ряду
#define EGGS_START	11	//начальное значение при начале печати ряда яиц

unsigned int Counter=EGGS_START;	//счетчик импульсов между яйцами в ряду
unsigned char volatile Count_Egg=0;
unsigned int volatile count_temp=0;
unsigned char volatile stoping_flag=0;
unsigned int countStep=0;
unsigned char temp11=0;

void 	Mot2_Start(void);						//unsigned char 	*DirM2
void 	Mot3_Start(unsigned char	*DirM3);

//void 	Mot1_Stop(void);
void 	Mot2_Stop(void);
void	Mot3_Stop(void);

void	Home(void);
void 	First_init(void);

void	Print_Row(void);

ISR (INT0_vect)
{
	

	//	PORTB=0x01;
}

ISR (INT1_vect)
{
	
	countStep++;
	if(countStep>310)
		countStep=0;
	

	//	PORTB=0x01;
}



//обработчик прерывания по энкодеру МОТ3
//формирование импульсов старта печати
ISR(INT2_vect)
{
	count_temp++;
	Counter--;
	
	if(Counter==0)
	{
		PORTG|=(1<<PRN);
		Counter=EGGS_WIDTH;
	}
	else if(Counter==1)
	{
		Count_Egg++;
		if(Count_Egg<7)
		{
			PORTG&=~(1<<PRN);
		}
		
		
	}
	

}
//----------------------


//----------------------


ISR	(TIMER1_COMPA_vect)
{
	//unsigned int	temp1;//temp2;//temp2;
	//temp1=OCR1A;
	if(OCR1A<ICR1-2)//ICR-1
	{
	//temp1++;
	OCR1A++;
	return;
	}
	//PORTA^=1;
	TIMSK&=~(1<<OCIE1A);
	//TIFR=1<<OCF1B;

}

//обробник переривання таймеру по двигуну стрічки
ISR (TIMER1_COMPB_vect)
{

	if(stoping_flag)
	{
		if(OCR1B>10)
		{
			OCR1B--;
			return;
		}
		else
		{
			OCR1B--;
			//скидання і відключення переривання по таймеру після зупинки двигуна
			ETIMSK&=~(1<<OCIE1B);
			ETIFR=1<<OCF1B;

			return;

		}
	}
	else if(OCR1B<ICR1-2)
		{
			OCR1B++;
			return;
		}	
	
	//скидання і відключення переривання по таймеру після розгону двигуна
	ETIMSK&=~(1<<OCIE1B);
	ETIFR=1<<OCF1B;

}

//обробник переривання таймеру по двигуну друкуючої головки
ISR	(TIMER1_COMPC_vect)	
{
	if(stoping_flag)
	{
		if(OCR1C>10)
		{
			OCR1C--;
			return;
		}
		else
		{
			OCR1C--;
			//скидання і відключення переривання по таймеру після зупинки двигуна
			ETIMSK&=~(1<<OCIE1C);
			ETIFR=1<<OCF1C;

			return;

		}
	}
	else if(OCR1C<ICR1-2)
		{
			OCR1C++;
			return;
		}	
	
	//скидання і відключення переривання по таймеру після розгону двигуна
	ETIMSK&=~(1<<OCIE1C);
	ETIFR=1<<OCF1C;

}




//void	Mot1_Start(unsigned char 	*DirM1);



 
int main(void)
{
	//DDRC=((1<<M1_DIR)|(1<<M2_DIR)|(1<<M3_DIR));
	//PORTC=0xFF;
	//int i=0;
	//int* countPulse=&i;
	//unsigned char 		Dir3=0;	//Dir1=0,	Dir2=0,
	//unsigned char	Count_Egg1=0;//,	Count_Egg2=0;
	unsigned char	Count_Row1=0;//,	Count_Row2=0;
	//unsigned char		M2_Run=0,	M3_Run=0;//	M1_Run=0,		флаги роботы двигателей
	//unsigned char	*m2dir=0;
	//unsigned char 	tray1=0, tray2=0;//флаги наличия лотков
	//unsigned char 	tray1_printing=0, tray2_printing=0;//флаги печатания лотков
	//unsigned int	tray1_Offset=0,	tray2_Offset=0;
	//unsigned char 	tray1_row_printing=0, tray2_row_printing=0;
	//unsigned char SF=0;
	//unsigned char* stoping_flag=&SF;//указатель на флаг остановки двигателя каретки	
	//DDRB=0xFF;
	//PORTB=0xFF;
	//_delay_ms(1000);
	First_init();
	sei();
	Home();
	Mot2_Start();
	while(1)
	{
		if(PINC&(1<<END13))	//перевірка фотодатчика
		{
			//обнулення лічильника кроків двигуна стрічки 
			cli();
			countStep=0;
			sei();
			
			Count_Row1++;	//счетчик рядов лотка
			if( Count_Row1>1 && Count_Row1<7 )	//1-ый и 7-ой ряд пропускаются!!!
			{
				Mot2_Stop();
				Print_Row();
				
				Mot2_Start();

			}
			else if(Count_Row1>6)
				Count_Row1=0;
			while(PINC&(1<<END13));

		}
		if(PINA&(1<<END1))
		{
			if(countStep>100)
			{
				Mot2_Stop();
				cli();
				countStep=0;
				sei();
				Print_Row();
				Mot2_Start();	
			}
		}
		if(countStep>300)
		{
			Count_Row1=0;
		}




	}//while(1)-----------------------------------------------
	
	
}

void	Print_Row(void)
{
//	unsigned char temp11=0;
	unsigned char Dir3=0;
	count_temp=0;
	Counter=EGGS_START;
	Count_Egg=0;

	if(PINA&(1<<END5)&&(PINA&(~(1<<END6))))
	{
		//напрямок друку для принтера
		PORTG|=(1<<DIR);

		Dir3=1;
		
		Mot3_Start(&Dir3);
		//ожидание открытия датчика Холла
		temp11=0;
		
		while(PINA&(1<<END5)||PINA&(1<<END6));	//&&(PINA&(~(1<<END6)))));
		
		//ожидание открытия датчика Холла
		/*while((!(PINA&(1<<END5)|(1<<END6)))&&temp11<10)//антидребезг концевиков
		{
			temp11++;
			if(PINA&(1<<END5))
				temp11=0;
		}
*/

		//очистить флаги срабатывания прерывания перед включением
		
		//включить обработку прерывания INT2
		EICRA=(1<<ISC21);//||(1<<ISC20);
		EIFR=(1<<INTF2);
		EIMSK=(1<<INT2);

//ver_END6: проверка концевика
		
		while(temp11<10)
		{
			temp11++;
			if(!(PINA&(1<<END6)))
				temp11=0;
		}
		
		
		
		
						
	}
	else if (PINA&(~(1<<END5))&&(PINA&(1<<END6)))
	{
		//напрямок друку для принтера
		PORTG&=~(1<<DIR);
		
		Dir3=0;
		
		Mot3_Start(&Dir3);
		//ожидание открытия датчика Холлла
		while(!((~PINA&(1<<END5))&&(~PINA&(1<<END6)))	);
		//while(!(~PINA&((1<<END5)|(1<<END6))));
	/*	temp1=0;
		while(PINA&(1<<END6)	&&	temp1<10 )
		{
			temp1++;
			if( PINA&(1<<END6)	)
				temp1=0;
		}
		*/
		
		//включить обработку прерывания INT2
		EICRA=(1<<ISC21);//|(1<<ISC20);
		//очистить флаги срабатывания прерывания перед включением
		EIFR=(1<<INTF2);
		EIMSK=(1<<INT2);
		
		//temp1=!(PINA&(1<<END5));
		temp11=0;
		while(temp11<10)
		{
			temp11++;
			if(!(PINA&(1<<END5)))
				temp11=0;
		}

	}
	else Home();
	
	
	Mot3_Stop();
	EIMSK=0;
}


//*********************


//**********************

/*void	Mot1_Start(unsigned char *DirM1)
{
	PORTB&=~1<<M1_POW;
	if(*DirM1)
	{
		PORTC&=~(1<<M1_DIR);
	}	
	else	PORTC|=1<<M1_DIR;
	//unsigned int temp1,temp2;

	// Timer/Counter 1 initialization


	TCCR1A|=(1<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<COM1C1) | (0<<COM1C0) | (1<<WGM11) | (0<<WGM10);
	TCCR1B|=(0<<ICNC1) | (0<<ICES1) | (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);

	//TCNT1H=0xEE;
	//TCNT1L=0xB8;
	ICR1=0x02A4;
	//ICR1L=0xA4;
	OCR1A=0x0001;
	//OCR1AL=0x6F;
	//OCR1BH=0x00;
	//OCR1BL=0x00;
	//OCR1CH=0x00;
	//OCR1CL=0x00;
	TIFR|=1<<OCF1A;
	TIMSK|=1<<OCIE1A;
	sei();

}*/

//***********************
void	Mot2_Start(void)	//unsigned char *Dir_M2
{
	stoping_flag=0;

	PORTB&=~(1<<M2_POW);
	PORTD&=~(1<<M2_BRAKE);
	// виставити напрямок руху		***************************************
	//								***************************************
		PORTC|=1<<M2_DIR;			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	//PORTB|=(1<<M2_POW);

	TCCR1A|=(0<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (0<<COM1C1) | (0<<COM1C0) | (1<<WGM11) | (0<<WGM10);
	TCCR1B|=(0<<ICNC1) | (0<<ICES1) | (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
	//TCNT1H=0xEE;
	//TCNT1L=0xB8;
	ICR1=0x02A4;
	//ICR1L=0xA4;
	//OCR1AH=0x00;
	//OCR1AL=0x00;
	OCR1B=0x0001;
	//OCR1BL=0x52;
	//OCR1CH=0x00;
	//OCR1CL=0x00;
	TIFR|=1<<OCF1B;
	TIMSK|=1<<OCIE1B;
	cli();
	EICRA=(1<<ISC11);
	EIMSK=(1<<INT1);
	EIFR=(1<<INTF1);
	sei();	
}


//***********************
void	Mot3_Start(unsigned char *DirM3)
{

	stoping_flag=0;
	PORTB&=~(1<<M3_POW);
	PORTD&=~(1<<M3_BRAKE);
	if(*DirM3)
		PORTC|=1<<M3_DIR;
	else	PORTC&=~(1<<M3_DIR);	

	TCCR1A|=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (1<<COM1C1) | (0<<COM1C0) | (1<<WGM11) | (0<<WGM10);
	TCCR1B|=(0<<ICNC1) | (0<<ICES1) | (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
	//TCNT1H=0xEE;
	//TCNT1L=0xB8;
	ICR1=0x02A4;
	//ICR1H=0x08;
	//ICR1L=0xA4;
	//OCR1AH=0x00;
	//OCR1AL=0x00;
	//OCR1BH=0x00;
	//OCR1BL=0x00;
	OCR1C=0x0001;
	//OCR1CL=0xDD;
	ETIFR|=1<<OCF1C;
	ETIMSK=1<<OCIE1C;
	sei();
}

//***********************
void	Home(void)
{
	unsigned char Dir1=1,temp1=0;	//Dir1 проверить правильность направления двигателя 
	//********************************************
	temp1^=PINA;
	temp1&=(1<<END6);	//маска проверки каретки ПГ в начальном положении
	if(!temp1)
	{
		Mot3_Start(&Dir1);
		while(!temp1)
		{
			temp1=(PINA&(1<<END6));
		}
		Mot3_Stop();
	}
	
	//Mot2_Start();



}

//***********************
/*void	Mot1_Stop(void)
{
	TCCR1A&=~(1<<COM1A1|1<<COM1A0);//(0<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (1<<COM1B0) | (1<<COM1C1) | (1<<COM1C0) | (1<<WGM11) | (1<<WGM10);
	//TCCR1B&=(0<<ICNC1) | (0<<ICES1) | (1<<WGM13) | (1<<WGM12) | (1<<CS12) | (1<<CS11) | (1<<CS10);
	TIMSK&=~(1<<OCIE1A);//0xEF;
	TIFR=1<<OCF1A;
	OCR1A=0;
	PORTB&=~(1<<M1_PWM);
	PORTB|=1<<M1_POW;

}*/


//***********************
void	Mot2_Stop(void)
{
	cli();
	EIMSK&=(~(1<<INT0));
	EIFR&=(~(1<<INTF0));
	sei();
	stoping_flag=1;
	ETIFR|=1<<OCF1C;
	ETIMSK=1<<OCIE1C;
	
	while(OCR1B>=10);

	stoping_flag=0;	

	TCCR1A&=~(1<<COM1B1|1<<COM1A0);//(1<<COM1A1) | (1<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (1<<COM1C1) | (1<<COM1C0) | (1<<WGM11) | (1<<WGM10);
	//TCCR1B&=(1<<ICNC1) | (1<<ICES1) | (1<<WGM13) | (1<<WGM12) | (1<<CS12) | (1<<CS11) | (1<<CS10);

	TIMSK&=~(1<<OCIE1B);//0xF7;
	TIFR=1<<OCF1B;
	OCR1B=0;
	PORTB&=~(1<<M2_PWM);
	PORTD|=(1<<M2_BRAKE);
	//PORTB|=1<<M2_POW;
}

//***********************
void	Mot3_Stop(void)
{
	
	stoping_flag=1;
	ETIFR|=1<<OCF1C;
	ETIMSK=1<<OCIE1C;
	
	while(OCR1C>=10);

	stoping_flag=0;
	
	TCCR1A&=~(1<<COM1C1|1<<COM1C0);
	ETIMSK&=~(1<<OCIE1C);
	ETIFR&=~(1<<OCF1C);
	OCR1C=0;
	PORTB&=~(1<<M3_PWM);
	//Зроблено на виробництві для чіткої зупинки двигуна каретки
	//PORTB|=1<<M3_POW;
	PORTD|=(1<<M3_BRAKE);
}


void 	First_init(void)
{
	DDRA=0x00;//PORTA Входы датчиков
	PORTA=0xFF;

	DDRB=(1<<M3_PWM)|(1<<M2_PWM)|(1<<M1_PWM)|(1<<M3_POW)|(1<<M2_POW)|(1<<M1_POW);
	PORTB=0xFF;
	PORTB&=~((1<<M3_PWM)|(1<<M2_PWM)|(1<<M1_PWM)|(0<<M3_POW)|(0<<M2_POW)|(0<<M1_POW));

	DDRC=(0<<END10)|(0<<END11)|(0<<END12)|(0<<END13)|(0<<END14)|(1<<M3_DIR)|(1<<M2_DIR)|(1<<M1_DIR);
	PORTC=0xFF;
	
	DDRD=(1<<M3_BRAKE)|(1<<M2_BRAKE)|(1<<M1_BRAKE);
	PORTD=0xFF;

	DDRE=0x00;
	PORTE=0xFF;

	DDRF=0x00;
	PORTF=0xFF;

	DDRG=(1<<PRN)|(1<<DIR);
	PORTG=0xFF;


}


