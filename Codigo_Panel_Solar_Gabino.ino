#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

// RTC_DS1307 rtc;
RTC_DS3231 rtc;

String daysOfTheWeek[7] = { "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" };
String monthsNames[12] = { "Enero", "Febrero", "Marzo", "Abril", "Mayo",  "Junio", "Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre" };
int dia, mes, anio, horas, minutos, segundos;
String dia_de_la_semana;

// Define sensibilidad en Voltios/Amperio para sensor de 5A
float Sensibilidad=0.183; 

// Define analog input
#define ANALOG_IN_VOLTAJE     A0
#define ANALOG_IN_CORRIENTE   A1
 
// Floats for ADC voltage & Input voltage
float adc_voltaje = 0.0;
float in_voltaje = 0.0;

float adc_corriente = 0.0;
float in_corriente = 0.0;

// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0; 
  
// Float for Reference Voltage
float ref_voltaje = 5.0;
float ref_corriente = 5.0;

// Integer for ADC value
int adc_value = 0;
int n_muestras = 500;

File myFile;
//---------------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(9600);
  Serial.print(F("Iniciando SD ..."));
  if (!SD.begin(8)) { // El número 8 representa el pin CS del módulo
    Serial.println(F("No se pudo inicializar"));
    return;
  }
  Serial.println("inicializacion exitosa");

  if (!rtc.begin()) {
    Serial.println(F("No se puedo endotnrar el módulo RTC"));
    while (1);
  }

  // Si se ha perdido la corriente, fijar fecha y hora
  if (rtc.lostPower()) {
    // Fijar a fecha y hora de compilacion
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    
    // Fijar a fecha y hora específica. En el ejemplo, 30 de Marzo del 2025 a las 03:00:00
    // rtc.adjust(DateTime(2025, 3, 30, 3, 0, 0));
  }
}
//---------------------------------------------------------------------------------------------
void loop()
{
  // Obtener fecha actual y mostrar por Serial
  DateTime now = rtc.now();
  RELOJ();
  MEDIR_VOLTAJE();
  MEDIR_CORRIENTE();  
  
}
//---------------------------------------------------------------------------------------------
void MEDIR_VOLTAJE()
  {
  // Read the Analog Input
  adc_value = analogRead(ANALOG_IN_VOLTAJE);
  
  // Determine voltage at ADC input
  adc_voltaje  = (adc_value * ref_voltaje) / 1024.0;
  
  // Calculate voltage at divider input
  in_voltaje = adc_voltaje*(R1+R2)/R2;
  
  // Print results to Serial Monitor to 2 decimal places
  Serial.print("Input Voltage = ");
  Serial.println(in_voltaje, 2);
  
  // Short delay
  delay(500);
  }
//---------------------------------------------------------------------------------------------  
void MEDIR_CORRIENTE()  {
  
  for(int i=0;i<n_muestras;i++)
  {
    // Read the Analog Input
    adc_value = analogRead(ANALOG_IN_CORRIENTE);
  
    // Determine voltage at ADC input
    adc_corriente  = (adc_value * ref_corriente) / 1023.0;
     
    in_corriente = in_corriente + (adc_corriente - 2.5)/Sensibilidad; //Ecuación  para obtener la corriente
  }
  in_corriente = in_corriente / n_muestras + 0.04; //offset
  if (in_corriente<0){
    in_corriente = 0.0;
  }
}
//--------------------------------------------------------------------------------------------- 
void RELOJ()
{
  DateTime date = rtc.now(); 
  anio = date.year();
  mes = date.month();
  dia = date.day();
  dia_de_la_semana = daysOfTheWeek[date.dayOfTheWeek()];
  horas = date.hour();
  minutos = date.minute();
  segundos = date.second();
}
//--------------------------------------------------------------------------------------------- 
void GUARDAR_SD()  {
  
  myFile = SD.open("datalog.txt", FILE_WRITE);//abrimos  el archivo
  
  if (myFile) { 
    Serial.print("Escribiendo SD: ");
    myFile.print(dia, DEC);
    myFile.print("/");
    myFile.print(mes, DEC);
    myFile.print("/");
    myFile.print(anio, DEC);
    myFile.print(" ");
    myFile.print(horas, DEC);
    myFile.print(":");
    myFile.print(minutos, DEC);
    myFile.print(":");
    myFile.print(segundos, DEC);
    myFile.print(",");
    myFile.print(in_voltaje);
    myFile.print(",");
    myFile.print(in_corriente);
    myFile.println("");
    myFile.close(); //cerramos el archivo      
  } 
  else {
    Serial.println("Error al abrir el archivo");
  }
  delay(100);
  }
