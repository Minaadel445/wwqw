
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

//These values are in the datasheet thermal resistor

#define RT0 10000   // Ω
#define B 3950      // K
#define VCC 5    //Supply voltage
#define R 10000  //R=10KΩ

#define MinCoils 1
#define MinTurns 0
#define MinSpeed 1
#define MinDim 20
#define MinSPD 14
#define MinOutput 1
#define MinLength 1
#define MinTrans 1

#define MaxCoils 10
#define MaxTurns 1000
#define MaxSpeed 100   //speed of motor
#define MaxSPD   255
#define MaxDim 1000
#define MaxOutput 10
#define MaxLength 100
#define MaxTrans 1000

#define InitSpeed 50
#define InitTurns 50
#define InitCoils 50
#define InitSPD 127
#define InitDim 50 // = 50/100 = 0.5
#define InitLength 50
#define InitOutput 1
#define InitTrans 50


#define TempFan 40
#define Distance 200
#define PITCH 20
#define CW 0
#define CCW 1

// digital output
#define FAN 8
#define SPEED 9
#define InfRed 10
#define RELAY 11 
#define stpPin 12
#define dirPin 13



/*****************************************LCD VOLUME & Address********************************/

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display.

/*********************************************************************************************/

/*****************************************KEYPAD INIT*****************************************/
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {0, 1, 2, 3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 5, 6, 7}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
/*********************************************************************************************/


/**********************************************Definition*************************************/
unsigned char Coils = InitCoils ;
unsigned char SPD = InitSPD ;
unsigned char Speed = InitSpeed ;
unsigned char Outputs = InitOutput ;
float Dim_Wire = InitDim ;
float Length = InitLength ;
double Turns[MaxCoils] = {InitTurns,0,0,0,0} ;
double Tturns[MaxOutput] = {InitTurns,0,0,0,0} ;
char z = 1 ;
bool x = true ;
bool q = true ;
bool q2 = true ;
bool st2 = true ;
bool T = true ;
bool T2 = true ;
char k;


  
char key ;

typedef enum {
  
  state_0 = 0 , 
  state_1  ,
  state_2  ,
  state_3  ,
  state_4

}StatE ;

StatE state = state_0 ;

typedef enum {
  
  Data_0 = 0 , 
  Data_1  ,
  Data_2  ,
  Data_3  ,
  Data_4
  
}DATA ;

DATA Data = Data_0 ;

typedef enum {
  
  Trans_0 = 0 , 
  Trans_1  ,
  Trans_2  ,
  Trans_3  ,
  Trans_4  ,
  Trans_5
  
}TRANS ;

TRANS Trans = Trans_0 ;

/*************************************************************************************************/


/**********************************************FUNC Prototype*************************************/
void DaTa_0 ();
void DaTa_1 ();
void DaTa_2 ();

void STATE_0();
void STATE_1();
void STATE_2();
void STATE_3();
void STATE_4();

void TRANS_0();
void TRANS_1();
void TRANS_2();
void TRANS_3();
void TRANS_4();
void TRANS_5();

float Get_Value();
//void Buzzer(unsigned char BZR);
void CHECK_TEMP();
void Step_M(unsigned char S,unsigned char D); // stepper motor func. input (displacement by (mm * 10) , direction (CW Or CCW))
void ZP(); //zero position
/*************************************************************************************************/



/**************************************SetUp******************************************************/

void setup()
{
  for(char F = InitCoils ; F > 1 ; F-- )          
        {
          Turns[z] = InitTurns ;
          z++ ;
        }
  
  z = 1 ;
  for(char F = InitOutput ; F > 1 ; F-- )          
        {
          Tturns[z] = InitTrans ;
          z++ ;
        }
  pinMode(InfRed,INPUT);
  pinMode(A0,INPUT);
  pinMode(FAN,OUTPUT);
  pinMode(SPEED,OUTPUT);
  pinMode(RELAY,OUTPUT);
  pinMode(stpPin,OUTPUT);
  pinMode(dirPin,OUTPUT);
  lcd.init();                      // initialize the lcd 
  lcd.backlight();                 // ON BackLight
  lcd.clear();
  lcd.setCursor(7,0);
  lcd.print("Hello");
  delay(1000);
}
/**************************************************************************************************/


/*****************************************MainLoop*****************************************************/

void loop()
{ 
  switch(Data){

    case Data_0 :
          CHECK_TEMP();
          DaTa_0();
          break;
         
    case Data_1 :
          CHECK_TEMP();
          switch(state){
                case state_0 : 
                    STATE_0();
                    break;
                case state_1 : 
                 //  STATE_1();
                    break;
                case state_2 : 
                    STATE_2();
                    break;
                case state_3 : 
                    STATE_3();
                    break;
                case state_4 : 
                    STATE_4();
                    break;
              }
              CHECK_TEMP();
              break;
              
    case Data_2 :
              CHECK_TEMP();
              DaTa_1 ();
              break;

    case Data_3 : 
    
              CHECK_TEMP();
              switch(Trans){
   
                  case Trans_0 : 
                      TRANS_0();
                      break;
                  case Trans_1 : 
                     //TRANS_1();
                      break;
                  case Trans_2 : 
                      TRANS_2();
                      break;
                  case Trans_3 : 
                      TRANS_3();
                      break;
                  case Trans_4 : 
                      TRANS_4();
                      break;
                  case Trans_5 : 
                      TRANS_5();
                      break;
              }
         
          CHECK_TEMP();
              break;
              
              
    case Data_4 :
              CHECK_TEMP();
              DaTa_2();
              break;
    }
 
  
    
  
}

/**********************************************************************************************/



/*********************************Keypad get decimal number*************************************/

   // berif : in this function get numbers from 0 to 9 to get any decimal number from keypad
   // input : keypad
   // output : decimal number  
float Get_Value()
{
  long value = 0;                                // the number accumulator
  long keyvalue;                                     // the key pressed at current moment
  int isnum;
  
  do
  {
    key = keypad.getKey();                          // input the key
    isnum = (key >= '0' && key <= '9');         // is it a digit?
    if (isnum)
    {
      
      value = value * 10 + key - '0';               // accumulate the input number
    }
    if(key == 'A')
        {
           T=true ;
           Trans = Trans_0;
           x = true;
           state = state_0;
           break ;  
        }
        lcd.setCursor(12,1);
        lcd.print(value);
  
  }while (!(key == 'D'));
  // while ((isnum  || !keyvalue)||keyvalue == 'D');                          // until not a digit or while no key pressed
  //
  
  return value;

}//getKeypadInteger


/*************************************************Data_0**********************************************************/
//in this state show the processes
    
   void DaTa_0 ()
    { 
     // CHECK_TEMP();
      if(q){
       
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("1:Automatic");
  lcd.setCursor(0,1);
  lcd.print("2:Manual");
  lcd.setCursor(0,2);
  lcd.print("3:TransFormer"); 
  lcd.setCursor(0,3);
  lcd.print("4:Speed");
  q=false;
 CHECK_TEMP();
 
  }
  char k;
  do{k=keypad.getKey();}while(k==0);
  if (k == '1'){
      Data = Data_1 ;
      q2=true;
     // lcd.setCursor(9,1);
    //  lcd.print("state_1");
     } 

  else if (k == '2'){
      Data = Data_2 ;
       
      //lcd.setCursor(9,1);
      //lcd.print("state_2");
     } 
      
 else if (k == '3'){
      Data = Data_3 ;
      T=true;
     // lcd.setCursor(9,1);
    //  lcd.print("state_3");
     } 
 else if (k == '4'){
      Data = Data_4 ;
     // lcd.setCursor(9,1);
    //  lcd.print("state_3");
     }     
  //else{ state = state_0 ; }
      }


      
/******************************************************************************************************************/


/*****************************************************DATA_1*******************************************************/

void DaTa_1 ()
{
  int w = 0;
  lcd.clear();
  while(q2){
    lcd.setCursor(7,0);
    lcd.print("Manual");
    lcd.setCursor(0,1);
    lcd.print("Count :");
    lcd.setCursor(11,2);
    lcd.print(w);
    bool count = false;
    while ( digitalRead( InfRed ) )
            { count = true ; }
            
    if ( count == true )
           {
            w++;
            count = false ;
            delay(20);
           }
         
   char  E = keypad.getKey();
 if( E == 'B' )
        {
           Step_M( Distance ,CCW ); 
         }
 else if( E == 'C' && analogRead(A1) < 512 )
        {
           Step_M( Distance ,CW ); 
         }
 else if( E == '0' )
        {
           w = 0; 
           lcd.clear();
         }
          
 else if( E == 'A' )
         {
            q=true ;
            Data = Data_0 ;
            q2 = false ;
            break;
         }
  }
  }
  
/******************************************************************************************************************/


/*****************************************************DATA_2*******************************************************/

void DaTa_2()
{
               RET :
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Enter SPD 1-100%");
          //getvalue and D
          unsigned char Speed = Get_Value();
          if (Speed < MinSpeed || Speed > MaxSpeed )
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Out Of Range");
            CHECK_TEMP();
            delay(500);
            goto RET ;
          }
          SPD = (Speed/100)*255 ;
          CHECK_TEMP();
          if(Data==Data_0){return;}
          q=true ;
          Data = Data_0;
          
      char k;
      do{k=keypad.getKey();}
      while(k==0);
     if(k == 'A') 
     {
      q=true ;
      Data = Data_0;
      
     }   
  }
  
/******************************************************************************************************************/

 

/*************************************************TRANS_0**********************************************************/
  void TRANS_0()
  {
      if(T){
       
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("1:RUN");
    lcd.setCursor(0,1);
    lcd.print("2:Dimater Of Wire");
    lcd.setCursor(0,2);
    lcd.print("3:Num Of Output"); 
    lcd.setCursor(0,3);
    lcd.print("4:Num Of Turns");
    lcd.setCursor(8,0);
    lcd.print("5:Length");
    T=false;
  CHECK_TEMP();
  
    }
    
    do{k=keypad.getKey();}while(k==0);
    if (k == '1'){
        Trans = Trans_1 ;
        T2=true;
      // lcd.setCursor(9,1);
      //  lcd.print("state_1");
      } 

    else if (k == '2'){
        Trans = Trans_2 ;
        
        //lcd.setCursor(9,1);
        //lcd.print("state_2");
      } 
        
  else if (k == '3'){
        Trans = Trans_3 ;
      // lcd.setCursor(9,1);
      //  lcd.print("state_3");
      } 
  else if (k == '4'){
        Trans = Trans_4 ;
      // lcd.setCursor(9,1);
      //  lcd.print("state_3");
      }     
  else if (k == '5'){
        Trans = Trans_5 ;}
      // lcd.setCursor(9,1);
      //  lcd.print("state_3");
  else if (k == 'A'){
      Data = Data_0 ;
      q = true ;
      }    
 // else{;}
        }
                    
  /******************************************************************************************************************/



  /*************************************************TRANS_1**********************************************************/
    void TRANS_1()
    { 
      
        unsigned char m = 0 ; 
    for(unsigned char k = Coils ; k > 0 ; k-- ){
      unsigned long i = Turns[m];
      lcd.clear();
     
         while( i > 0 )
         {
           lcd.setCursor(0,0);
           lcd.print("WORK Coil  ");
           lcd.print(m+1);
           lcd.setCursor(0,1);
           lcd.print("WORK Turn  ");
           lcd.print(i);
           delay(50);
           analogWrite(SPEED , SPD);
           digitalWrite(RELAY,1);
          if( keypad.getKey() == 'A' )
        {
            x=true ;
            state = state_0 ;
            break;
         }
         bool count = false ;
     while( digitalRead(InfRed)&& i > 0 ) 
         {
            count = true ;  
        } 
     if( count == true )
         {
           i-- ;
           count = false ;
           delay(23);  //
           lcd.clear();
         }
      
     }
          
           if( k > 1 ){
            digitalWrite(RELAY,0);
            Step_M(Distance ,CCW);}//(displacement (mm*10) , direction (CCW or CW))
            m++;
            if( keypad.getKey() == 'A' )
                {
                    x=true ;
                    state = state_0 ;
                    break;
                 } 
            }
      digitalWrite(RELAY,0);      
      CHECK_TEMP();                                                        
      lcd.clear();
      lcd.setCursor(8,0);
      lcd.print("DONE");
      delay(1000);
       //t
        T=true ;
        Trans = Trans_0 ;

    }
    
  /******************************************************************************************************************/



  /*************************************************TRANS_2**********************************************************/ 
    void TRANS_2()
      {
          
            RET :
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Dim Of Wire ");
            lcd.print(MinDim);
            lcd.print(":");
            lcd.print(MaxDim);
            //getvalue and D
            Dim_Wire = Get_Value();
            if( Dim_Wire < MinDim || Dim_Wire > MaxDim )
            {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Out Of Range");
              delay(500);
              CHECK_TEMP();
              goto RET ;
            }
            Dim_Wire /= 100;  // Dim_Wire = Dim_Wire / 100
            T=true ;
            Trans = Trans_0;
            if(Trans = Trans_0){return;}
            
    
        }

/******************************************************************************************************************/



/*************************************************TRANS_3**********************************************************/
void TRANS_3()
 {
            RET :
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Num Of Outputs ");
            lcd.print(MinOutput);
            lcd.print(":");
            lcd.print(MaxOutput);
            //getvalue and D
            Outputs = Get_Value();
            if (Outputs < MinOutput || Outputs > MaxOutput )
            {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Out Of Range");
              delay(500);
              CHECK_TEMP();
              goto RET ;
            }
            T=true ;
            Trans = Trans_0;
              CHECK_TEMP();
              char K=1;
              do{K=keypad.getKey();}
              while(K==0);
            if(K == 'A') 
            {
              T=true ;
              Trans = Trans_0;
            }   
         
}
/******************************************************************************************************************/



/*************************************************TRANS_4**********************************************************/
void TRANS_4()
{
      lcd.clear();
      unsigned char y = 0;
      for(unsigned char i = Outputs ; i > 0 ; i--  ){
        RET :
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Num_Turns_");
      lcd.print(MinTrans);
      lcd.print(":");
      lcd.print(MaxTrans);
      lcd.print("_");
      lcd.print(y+1);
      //getvalue and D
      Tturns[y] = Get_Value();
      if (Tturns[y] < MinTrans || Tturns[y] > MaxTrans )
              {
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Out Of Range");
                delay(500);
                CHECK_TEMP();
                goto RET ;
              }
               y++;     
              }
   T=true ;
   Trans = Trans_0 ;    
   if(Trans==Trans_0){return;}    
}

/******************************************************************************************************************/



/*************************************************TRANS_5**********************************************************/
void TRANS_5()
{
            RET :
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Length ");
            lcd.print(MinLength);
            lcd.print(":");
            lcd.print(MaxLength);
            //getvalue and D
            Length = Get_Value();
            if (Length < MinLength || Length > MaxLength )
            {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Out Of Range");
              delay(500);
              CHECK_TEMP();
              goto RET ;
            }
            T=true ;
            Trans = Trans_0;
            if(Trans = Trans_0){return;}
            
}

/******************************************************************************************************************/



/*************************************************State_0**********************************************************/
//in this state show the processes
    
   void STATE_0 ()
    { 
      ZP();
     // CHECK_TEMP();
      if(x){
       
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("1:RUN");
  lcd.setCursor(0,1);
  lcd.print("2:Set Coils");
  lcd.setCursor(0,2);
  lcd.print("3:Set Turns"); 
  lcd.setCursor(0,3);
  lcd.print("4:Speed");
  x=false;
 CHECK_TEMP();
 
  }
  char k;
  do{k=keypad.getKey();}while(k==0);
  if (k == '1'){
      state = state_1 ;
     // lcd.setCursor(9,1);
    //  lcd.print("state_1");
     } 

  else if (k == '2'){
      state = state_2 ;
      st2=true; 
      //lcd.setCursor(9,1);
      //lcd.print("state_2");
     } 
      
 else if (k == '3'){
      state = state_3 ;
     // lcd.setCursor(9,1);
    //  lcd.print("state_3");
     } 
 else if (k == '4'){
      state = state_4 ;
     // lcd.setCursor(9,1);
    //  lcd.print("state_3");
     }     
 else if (k == 'A'){
      Data = Data_0 ;
      q = true ;
  //else{ state = state_0 ; }
      }
    }


      
/******************************************************************************************************************/


/**************************************************State_1**********************************************************/

// in this state initialization for system the system run 3 min
// output : the solution ready and buzzer on
// 
      
   void STATE_1 ()
   {
    unsigned char m = 0 ; 
    for(unsigned char k = Coils ; k > 0 ; k-- ){
      unsigned long i = Turns[m];
      lcd.clear();
     
         while( i > 0 )
         {
           lcd.setCursor(0,0);
           lcd.print("WORK Coil  ");
           lcd.print(m+1);
           lcd.setCursor(0,1);
           lcd.print("WORK Turn  ");
           lcd.print(i);
           delay(50);
           analogWrite(SPEED , SPD);
           digitalWrite(RELAY,1);
          if( keypad.getKey() == 'A' )
        {
            x=true ;
            state = state_0 ;
            break;
         }
         bool count = false ;
     while( digitalRead(InfRed)&& i > 0 ) 
         {
            count = true ;  
        } 
     if( count == true )
         {
           i-- ;
           count = false ;
           delay(23);  //
           lcd.clear();
         }
      
     }
          
           if( k > 1 ){
            digitalWrite(RELAY,0);
            Step_M(Distance ,CCW);}//(displacement (mm*10) , direction (CCW or CW))
            m++;
            if( keypad.getKey() == 'A' )
                {
                    x=true ;
                    state = state_0 ;
                    break;
                 } 
            }
      digitalWrite(RELAY,0);      
      CHECK_TEMP();                                                        
      lcd.clear();
      lcd.setCursor(8,0);
      lcd.print("DONE");
      delay(1000);
       //to show state_0 only one
       x=true ; 
       state = state_0 ;
      }

/******************************************************************************************************************/



/*************************************************State_2**********************************************************/ 
  void STATE_2()
    {
      
      if(st2){  
          RET :
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Num_Coils ");
          lcd.print(MinCoils);
          lcd.print(":");
          lcd.print(MaxCoils);
          //getvalue and D
          Coils = Get_Value();
          if (Coils < MinCoils || Coils > MaxCoils )
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Out Of Range");
            delay(500);
            CHECK_TEMP();
            goto RET ;
          }
          if(state==state_0){return;}
          x=true ;
          state = state_0;
      }
      CHECK_TEMP();
      char K=1;
      do{K=keypad.getKey();}
      while(K==0);
     if(K == 'A') 
     {
      x=true ;
      state=state_0;
     }   
   
      }

/******************************************************************************************************************/


/******************************************************state_3*******************************************************/

     // berif : in this state get the values required to calculate the time .
     // input : from keypad
     // output : return value in varibles 
      
  void STATE_3()
  {
    
  lcd.clear();
  unsigned char y = 0;
  for(unsigned char i = Coils ; i > 0 ; i--  ){
    RET :
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("N_Turns_");
  lcd.print(MinTurns);
  lcd.print(":");
  lcd.print(MaxTurns);
  lcd.print("_");
  lcd.print(y+1);
  //getvalue and D
  Turns[y] = Get_Value();
  if (Turns[y] < 0 || Turns[y] > MaxTurns )
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Out Of Range");
            delay(500);
            CHECK_TEMP();
            goto RET ;
          }
  if(state==state_0){return;}
  y++;
  }
  CHECK_TEMP();
  if(state==state_0){return;}
  x=true ;
  state = state_0;
  }
/******************************************************************************************************************/

/**************************************************SPEED MOTOR*****************************************************/
void STATE_4 ()
{
          RET :
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Enter Speed 1-100%");
          //getvalue and D
          unsigned char Speed = Get_Value();
          if (Speed < MinSpeed || Speed > MaxSpeed )
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Out Of Range");
            CHECK_TEMP();
            delay(500);
            goto RET ;
          }
          SPD = (Speed/100)*255 ;
          CHECK_TEMP();
          if(state==state_0){return;}
          x=true ;
          state = state_0;
          
      char k;
      do{k=keypad.getKey();}
      while(k==0);
     if(k == 'A') 
     {
      x=true ;
      state=state_0;
      
     }   
 }
  
/******************************************************************************************************************/

/**********************************************check_temperature***************************************************/

 
  void CHECK_TEMP()
  {
// breif : in this function check the temperature and on or off fan to cooler control and power unit
// input : ntc / negative temperature resistor 
// output : on or off fan 
// temp fan on : 40 C
// temp fan off : 30 C


/*thermistor parameters:
 * RT0: 10 000 Ω
 * B: 3950 K +- 1%
 * T0:  25 C
 * RT = R0 e^B( 1/T - 1/T0)
 */

float RT, VR, ln, TX, T0, VRT,VA0;

  T0 = 25 + 273.15;                 //Temperature T0 from datasheet, conversion from Celsius to kelvin


  VA0 =  analogRead(A0) ;              //Acquisition analog value of VRT
  VRT = (5.00 / 1023.00) * VA0;      //Conversion to voltage
  VR = VCC - VRT;
  RT = VRT / (VR / R);               //Resistance of RT

  ln = log(RT / RT0);
  TX = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor

   TX = TX - 273.15;                 //Conversion to Celsius
  
   
  
 if( TX >= TempFan )
  {
    //fan on
    digitalWrite(FAN,HIGH);
  }
 else
  {
    //fan off
    digitalWrite(FAN,LOW);
  }
 
  }

/******************************************************************************************************************/
void Step_M(unsigned char S,unsigned char D)
{
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Stepper Run");
     CHECK_TEMP();
     digitalWrite(dirPin,D);
     long int stp = (S*320)/PITCH;
     for(long int i = stp ; i > 0 ; i--){
          digitalWrite(stpPin,1);
          delayMicroseconds(50);
          digitalWrite(stpPin,0);
          delayMicroseconds(50);
      }

    lcd.clear();
  
}



/*****************************************************Buzzer****************************************************/
// Berif : in this function on buzzer
  /*
 void Buzzer(unsigned char BZR )
   {
         for(int j = BZR ; j > 0 ; j--)
            {//buzzer on
           digitalWrite(BUZZER,HIGH); 
           //delay
           delay(500);
           //buzzer off
           digitalWrite(BUZZER,LOW);
           //delay
           delay(500);}
    }*/
    
/******************************************************************************************************************/


/*************************************************zero postion*****************************************************/
void ZP()
{ 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Stepper Run");
  CHECK_TEMP();
  digitalWrite(dirPin,0); 
  while(analogRead(A1) < 516)
  {
  for(long int i = 0 ; analogRead(A1) < 516 && i < 16000 ; i++ ){
      digitalWrite(stpPin,1);
      delayMicroseconds(100);
      digitalWrite(stpPin,0);
      delayMicroseconds(100);
  }}
  lcd.clear();
  x=true ; 
  state = state_0 ;
  }
/******************************************************************************************************************/
