#include <xc.h>
#define _XTAL_FREQ 20000000

#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF

#define C1 PORTBbits.RB0
#define C2 PORTBbits.RB1
#define C3 PORTBbits.RB2
#define R1 PORTBbits.RB4
#define R2 PORTBbits.RB5
#define R3 PORTBbits.RB6
#define R4 PORTBbits.RB7

#define HYSTERESIS 2      // hysteresis band
#define TEMP_INTERVAL 1 

int heating = 0;         // heating status
int cooling = 0;         // cooling status

int Address=0;
char k;
int a,b,c,e,f,addr,R,M,p=0;
int adc;
float adc1,temperature,temperature_reff=0,temperature_mesure,temperature_reference;
int tab[3]={0,0,0};

void __interrupt() external(void)
{
    if (PIR1bits.ADIF ==1)
    {
        adc=(ADRESH<<8);
        adc=adc+ADRESL;
        PIR1bits.ADIF =0;
    }
}

void buzzer()
{
    for(int p=0;p<3;p++)
    {
    PORTEbits.RE2=1;
    __delay_ms(5);
    PORTEbits.RE2=0;
    __delay_ms(5);
    }
}
void turnHeatingOn() {
 PORTEbits.RE0=1; // code to turn heating system on
}

void turnHeatingOff() {
  PORTEbits.RE0=0;// code to turn heating system off
}

void turnCoolingOn() {
 PORTEbits.RE1=1; // code to turn cooling system on
}

void turnCoolingOff() {
 PORTEbits.RE1=0; // code to turn cooling system off
}

void EEPROM_Write(int Address,char Data)
{
  while(EECON1bits.WR); // Waits Until Last Attempt To Write Is Finished
  EEADR = Address;      // Writes The Address To Which We'll Wite Our Data
  EEDATA = Data;        // Write The Data To Be Saved
  EECON1bits.EEPGD = 0; // Cleared To Point To EEPROM Not The Program Memory
  EECON1bits.WREN = 1;  // Enable The Operation !
  INTCONbits.GIE = 0;   // Disable All Interrupts Until Writing Data Is Done
  EECON2 = 0x55;        // Part Of Writing Mechanism..
  EECON2 = 0xAA;        // Part Of Writing Mechanism..
  EECON1bits.WR = 1;    // Part Of Writing Mechanism..
  INTCONbits.GIE = 1;   // Re-Enable Interrupts
  EECON1bits.WREN = 0;  // Disable The Operation
  EECON1bits.WR = 0;    // Ready For Next Writing Operation
}
char EEPROM_Read(int Address)
{
  char Data;
  EEADR = Address;      // Write The Address From Which We gonna Get Data
  EECON1bits.EEPGD = 0; // Cleared To Point To EEPROM Not The Program Memory
  EECON1bits.RD = 1;    // Start The Read Operation
  Data = EEDATA;        // Read The Data
  return Data; 
}
void Lcd_Port(void)
{
  // toggling Enable PIN is must for data to be displayed on screen
    PORTCbits.RC7 = 1;
    __delay_us(200);
    PORTCbits.RC7 = 0;
    __delay_us(200);
}
void Lcd_Cmd(unsigned char Cmd)
{
    PORTCbits.RC6   = 0;        // For command RS must be low (0)     
    PORTD  = Cmd;      //  write Command to data pins of LCD    
    Lcd_Port();
}
void Init_LCD(void)
{
    /*  making Pins as output   */
    PORTCbits.RC7  = 0;  //  Enable pin of LCD 
    PORTCbits.RC6  = 0;  //  RS pin of LCD 
    PORTD     = 0;       //  DATA pin FOR LCD 8bits  port D as output   
    TRISD   = 0;         // Direction for Port D
    TRISCbits.TRISC7    = 0;//Direction for Port C bit 7 EN
    TRISCbits.TRISC6       = 0; // Direction for Port C 6 Rs Pin
   __delay_ms(10);          /*  10ms delay   */
   /*   writing commands for initialization of LCD  */
   Lcd_Cmd(0x38);  //  Functions Set as Given in Datasheet 
   Lcd_Cmd(0x0F);  //  Display ON; Cursor ON; Blink ON     
  //  Display Shifting OFF                                      
}  

void Lcd_Write_Char(char Data)
{
    PORTCbits.RC6  = 1;        //  For data RS must be high (1)    
    PORTD = Data;     //  write data to data bus of LCD   
    Lcd_Port();
}
void Lcd_Write_String(const char *str, unsigned char num)
{
    unsigned char i;
    for(i=0;i<num;i++)
    {
        Lcd_Write_Char(str[i]);
    }
}

int Lecture_temperature_ref(){
while(Address<3)
	{
                C1=1;C2=0;C3=0;
            if (R1==1)
			{
				EEPROM_Write(Address, '1');
                __delay_ms(1);
                k = EEPROM_Read(Address);
                __delay_ms(1);
                Lcd_Write_Char(k);
                __delay_ms(1);
                tab[Address]=1;
                __delay_ms(1);
                Address++;
                while(R1==1);
                
			}
            if (R2==1)
			{
				EEPROM_Write(Address, '4');
                __delay_ms(1);
                k = EEPROM_Read(Address);
                __delay_ms(1);
                Lcd_Write_Char(k);
                __delay_ms(1);
                tab[Address]=4;
                __delay_ms(1);
                Address++;
                while(R2==1);
                
			}
            if (R3==1)
			{
				EEPROM_Write(Address, '7');
                __delay_ms(1);
                k = EEPROM_Read(Address);
                __delay_ms(1);
                Lcd_Write_Char(k);
                __delay_ms(1);
                tab[Address]=7;
                __delay_ms(1);
                Address++;
                while(R3==1);
			}
			if (R4==1)
			{
				EEPROM_Write(Address, '*');
                __delay_ms(1);
                k = EEPROM_Read(Address);
                __delay_ms(1);
                Lcd_Write_Char(k);
                __delay_ms(1);
                Address++;
                while(R4==1);
                
			}

        C1=0;C2=1;C3=0;
            
			if (R1==1)
			{
				EEPROM_Write(Address, '2');
                __delay_ms(1);
                k = EEPROM_Read(Address);
                __delay_ms(1);
                Lcd_Write_Char(k);
                __delay_ms(1);
                tab[Address]=2;
                __delay_ms(1);
                Address++;
                while(R1==1);
			}
			if (R2==1)
			{
				EEPROM_Write(Address, '5');
                __delay_ms(1);
                k = EEPROM_Read(Address);
                __delay_ms(1);
                Lcd_Write_Char(k);
                __delay_ms(1);
                tab[Address]=5;
                __delay_ms(1);
                Address++;
                while(R2==1);
			}
			if (R3==1)
			{
				EEPROM_Write(Address, '8');
                __delay_ms(1);
                k = EEPROM_Read(Address);
                __delay_ms(1);
                Lcd_Write_Char(k);
                __delay_ms(1);
                tab[Address]=8;
                __delay_ms(1);
                Address++;
                while(R3==1);
			}
			if (R4==1)
			{
                EEPROM_Write(Address, '0');
                __delay_ms(1);
                k = EEPROM_Read(Address);
                __delay_ms(1);
                Lcd_Write_Char(k);
                __delay_ms(1);
                tab[Address]=0;
                __delay_ms(1);
                Address++;
                while(R4==1);
			}

		C1=0;C2=0;C3=1; 
			if (R1==1)
			{
                EEPROM_Write(Address, '3');
                __delay_ms(1);
                k = EEPROM_Read(Address);
                __delay_ms(1);
                Lcd_Write_Char(k);
                __delay_ms(1);
                tab[Address]=3;
                __delay_ms(1);
                Address++;
                while(R1==1);
			}
			if (R2==1)
			{
                EEPROM_Write(Address, '6');
                __delay_ms(1);
                k = EEPROM_Read(Address);
                __delay_ms(1);
                Lcd_Write_Char(k);
                __delay_ms(1);
                tab[Address]=6;
                __delay_ms(1);
                Address++;
                while(R2==1);
			}
			if (R3==1)
			{
                EEPROM_Write(Address, '9');
                __delay_ms(1);
                k = EEPROM_Read(Address);
                __delay_ms(1);
                Lcd_Write_Char(k);
                __delay_ms(1);
                tab[Address]=9;
                __delay_ms(1);
                Address++;
                while(R3==1);
			}
			if (R4==1)//#
			{   
                switch (Address){
                    case 1:

            temperature_reff=tab[0];
            break;
                    case 2:

            temperature_reff=(float)(tab[0]*10+tab[1]);
            break;}
                

                
                //Lcd_Write_Char((char)tab[1]-0xd3);
                Lcd_Write_String(" C",2);
                Lcd_Cmd(0xC0);
                __delay_ms(1);
                Lcd_Write_String("ACTUAL TEMP:",12);
                goto jump;
			
            }
    }
temperature_reff=(float)(tab[0]*100+tab[1]*10+tab[2]);
Lcd_Write_String(" C",2);
jump:
return temperature_reff;
}

float lecture_temperature_mes(void){
     ADCON0 = ADCON0|(0x04);
       adc1 =(float)( adc * (5.0 / 1023.0));
       temperature = adc1*100;
       Lcd_Cmd(0xC0);
       Lcd_Write_String("ACTUAL TEMP: ",12);
       a=temperature/100;
       e=(int)temperature%10;
       b=temperature/10;
       f=b%10;
       c=a%10;
       if (a==1)
       {   
        Lcd_Write_Char('1');
        Lcd_Write_Char((char)f+0x30);
        Lcd_Write_Char((char)e+0x30);
        Lcd_Write_String(" C ",3);
       __delay_ms(50);
       }
       else
       {
       Lcd_Write_Char((char)f+0x30);
       Lcd_Write_Char((char)e+0x30);
       Lcd_Write_String(" C      ",8);
       __delay_ms(50);  
       }      
return temperature; 
}
void hysterisis(float R,float M)
{
    if (M <= (R - HYSTERESIS)) 
        {
            heating = 1;
            cooling = 0;
            
        }
         else if (M >= (R + HYSTERESIS)) {
            heating = 0;
            cooling = 1;
        }
  //checking for buzzer
        
  // turn heating and cooling on/off
        if (heating)    
        {
        turnHeatingOn();
        
        }
        else 
        {
        turnHeatingOff();
        }
        
        if (cooling)
        {
        turnCoolingOn();
        }
        else    
        {
         turnCoolingOff();
        }
  
  __delay_ms(50);
        
}

void main(void) {
    INTCONbits.GIE=1;
    INTCONbits.PEIE=1;
    PIE1bits.ADIE = 1; //analog to digital converter
    TRISD = 0;
    PORTD = 0;
    TRISC = 0;
    PORTC = 0;
    TRISB = 0xF0;
    ADCON0 = 0x41;
    ADCON1 = 0x8E; //configuration du capteur
    TRISE = 0;
    PORTE =0;
    Init_LCD();
    Lcd_Write_String("TEMP REF:",10);
    temperature_reference=Lecture_temperature_ref(); //lecture de la temperature reference
    Lcd_Cmd(0x0c);
    while(1)
    {   
       
        temperature_mesure=lecture_temperature_mes();
        if((int)temperature_mesure == (int)temperature_reference) 
        {while(p<1){
            buzzer();
            hysterisis(temperature_reference,temperature_mesure);
            p++;
        }
        }
        else
        {
            
               hysterisis(temperature_reference,temperature_mesure);
               p=0;
        }
}
}

