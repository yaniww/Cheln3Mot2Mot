/*//��������� ������� �� ����� �����!!! 28,10,2021
//23.11.2021
	////�������� �� ���������� ��� ����� ������� ������� �������
	//PORTB|=1<<M3_POW;
	//PORTD|=(1<<M3_BRAKE);
//
02.12.2023
	������ ������ ������� ������� ������� �������


*/

#define F_CPU 14.7456E6

#include <avr/io.h>
#include <avr/interrupt.h>

//#include <util/delay.h>


#define END1 	PA0 //������ ������ ����� 	PINA0
#define END2 	PA1	//����� ������ �����	PINA1
#define	END3	PA2	//������ ������� �����	PINA2
#define END4	PA3	//����� ������� �����	PINA3

#define	END5	PA4	//������ ������� ������ �����		PINA4
#define	END6	PA5	//����� ������� ������ �����		PINA5
#define	END7	PA6	//������ ������� ������� �����		PINA6
#define	END8	PA7	//����� ������� ������� �����		PINA7
#define END9		PG2	//��������� ������ �����		PING2
#define END10		PC7	//������ ������ �����			PINC7
#define	END11		PC6	//��������� ������� �����		PINC6
#define	END12		PC5	//������ ������� �����			PINC5
#define	END13		PC4	//���������� ������ �����		PINC4
#define	END14		PC3	//���������� ������� �����		PINC3

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

#define PinEnd1	PINA	// ���� �������� END 1-8
#define PinEnd2 PINC	// ���� �������� END 10-14

#define EGGS_WIDTH	39	//���������� ����� ������ � ����
#define EGGS_START	11	//��������� �������� ��� ������ ������ ���� ���

unsigned int Counter=EGGS_START;	//������� ��������� ����� ������ � ����
unsigned char volatile Count_Egg=0;
unsigned int volatile count_temp=0;
unsigned char volatile stoping_flag=0;
ISR (INT0_vect)
{
	

	//	PORTB=0x01;
}


//���������� ���������� �� �������� ���3
//������������ ��������� ������ ������
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

//�������� ����������� ������� �� ������� ������
ISR (TIMER1_COMPB_vect)
{

	//unsigned int temp1;
	//temp1=OCR1B;
	if(OCR1B<ICR1-2)
	{
	
	OCR1B++;
	return;
	}
	//PORTA^=2;
	TIMSK&=~(1<<OCIE1B);
	TIFR=1<<OCF1B;
}

//�������� ����������� ������� �� ������� �������� �������
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
			//�������� � ���������� ����������� �� ������� ���� ������� �������
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
	
	//�������� � ���������� ����������� �� ������� ���� ������� �������
	ETIMSK&=~(1<<OCIE1C);
	ETIFR=1<<OCF1C;

}




//void	Mot1_Start(unsigned char 	*DirM1);
void 	Mot2_Start(void);						//unsigned char 	*DirM2
void 	Mot3_Start(unsigned char	*DirM3);

//void 	Mot1_Stop(void);
void 	Mot2_Stop(void);
void	Mot3_Stop(void);

void	Home(void);
void 	First_init(void);

void	Print_Row(void);


 
int main(void)
{
	//DDRC=((1<<M1_DIR)|(1<<M2_DIR)|(1<<M3_DIR));
	//PORTC=0xFF;
	//int i=0;
	//int* countPulse=&i;
	//unsigned char 		Dir3=0;	//Dir1=0,	Dir2=0,
	//unsigned char	Count_Egg1=0;//,	Count_Egg2=0;
	unsigned char	Count_Row1=0;//,	Count_Row2=0;
	//unsigned char		M2_Run=0,	M3_Run=0;//	M1_Run=0,		����� ������ ����������
	//unsigned char	*m2dir=0;
	//unsigned char 	tray1=0, tray2=0;//����� ������� ������
	//unsigned char 	tray1_printing=0, tray2_printing=0;//����� ��������� ������
	//unsigned int	tray1_Offset=0,	tray2_Offset=0;
	//unsigned char 	tray1_row_printing=0, tray2_row_printing=0;
	//unsigned char SF=0;
	//unsigned char* stoping_flag=&SF;//��������� �� ���� ��������� ��������� �������	
	//DDRB=0xFF;
	//PORTB=0xFF;
	//_delay_ms(1000);
	First_init();
	sei();
	Home();
	Mot2_Start();
	while(1)
	{
		if(PINC&(1<<END13))	//�������� �����������
		{
			Count_Row1++;	//������� ����� �����
			if( Count_Row1>1 && Count_Row1<7 )	//1-�� � 7-�� ��� ������������!!!
			{
				Mot2_Stop();
				Print_Row();
				Mot2_Start();

			}
			else if(Count_Row1>6)
				Count_Row1=0;
			while(PINC&(1<<END13));

		}





	}//while(1)-----------------------------------------------
	
	
}

void	Print_Row(void)
{
//	unsigned char temp1=0;
	unsigned char Dir3=0;
	count_temp=0;
	Counter=EGGS_START;
	Count_Egg=0;

	if(PINA&(1<<END5)&&(PINA&(~(1<<END6))))
	{
		//�������� ����� ��� ��������
		PORTG|=(1<<DIR);

		Dir3=1;
		
		Mot3_Start(&Dir3);
		//�������� �������� ������� �����
	while(PINA&(~(1<<END5)&&(PINA&(~(1<<END6)))));
		
		//�������� ����� ������������ ���������� ����� ����������
		
		//�������� ��������� ���������� INT2
		EICRA=(1<<ISC21);//||(1<<ISC20);
		EIFR=(1<<INTF2);
		EIMSK=(1<<INT2);
		while(!(PINA&(1<<END6)));
		
		
						
	}
	else if (PINA&(~(1<<END5))&&(PINA&(1<<END6)))
	{
		//�������� ����� ��� ��������
		PORTG&=~(1<<DIR);
		
		Dir3=0;
		
		Mot3_Start(&Dir3);
		//�������� �������� ������� ������
		while(PINA&(~(1<<END5))&&(PINA&(1<<END6)));
		
		
		//�������� ��������� ���������� INT2
		EICRA=(1<<ISC21);//|(1<<ISC20);
		//�������� ����� ������������ ���������� ����� ����������
		EIFR=(1<<INTF2);
		EIMSK=(1<<INT2);
		
		//temp1=!(PINA&(1<<END5));
		while(!(PINA&(1<<END5)));
		

	}
	
	
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
	PORTB&=~(1<<M2_POW);
	PORTD&=~(1<<M2_BRAKE);
	// ��������� �������� ����		***************************************
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
	//sei();

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
	unsigned char Dir1=1,temp1=0;	//Dir1 ��������� ������������ ����������� ��������� 
	//********************************************
	temp1^=PINA;
	temp1&=(1<<END6);	//����� �������� ������� �� � ��������� ���������
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

	TCCR1A&=~(1<<COM1B1|1<<COM1A0);//(1<<COM1A1) | (1<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (1<<COM1C1) | (1<<COM1C0) | (1<<WGM11) | (1<<WGM10);
	//TCCR1B&=(1<<ICNC1) | (1<<ICES1) | (1<<WGM13) | (1<<WGM12) | (1<<CS12) | (1<<CS11) | (1<<CS10);

	TIMSK&=~(1<<OCIE1B);//0xF7;
	TIFR=1<<OCF1B;
	OCR1B=0;
	PORTB&=~(1<<M2_PWM);
	PORTB|=1<<M2_POW;
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
	//�������� �� ���������� ��� ����� ������� ������� �������
	//PORTB|=1<<M3_POW;
	PORTD|=(1<<M3_BRAKE);
}


void 	First_init(void)
{
	DDRA=0x00;//PORTA ����� ��������
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


