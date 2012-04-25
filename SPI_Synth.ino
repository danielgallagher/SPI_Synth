/******************************************************************************/
/* SPI_Synth.ino                                                              */
#define ver   0.1
/******************************************************************************/
/* Copyright 2012 (C) Daniel Gallagher under MIT License
/* 
/* Created April 15, 2012
/* By Daniel Gallagher
/* 
/* http://github.com/danielgallagher/SPI_Synth
/* 
/* An Arduino sketch to initialze a MiniCircuits Synthesizer with 
/* Analog Devices (ADI) PLL over the SPI bus. 
/* 
/* Target device for this sketch:
/* * MiniCircuits KSN-900A-119+ with ADF4118 PLL
/* * Potentially all KSN series synthesizers with ADI ADF4118 PLL
/* 
/* The sketch uses the SPI.h library to initialize the PLL registers
/* upon powerup to function as a fixed CW local oscillator (LO).
/* 
/* A future goal is to transition to a standalone LO PCB with AVR.
/* 
/* Default pins used for Arduino UNO SPI interface:
/* * MOSI = digital 11;
/* * MISO = digital 12; - not used, but part of builtin SPI
/* * SCK  = digital 13;
/* * SS   = digital 10; // Initialized below
/* 
/* For ADF4118 PLL: (See datasheets for details)
/* f_out = (f_ref / R) {B*P + A} 
/* 
/* B >= A 
/* A < P 
/* N = BP + A
/* f_c = f_ref / R	
/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include <SPI.h>
#include "SPI_Synth.h"

/******************************************************************************/
/* Enter counter values below for desired output frequency                    */
/* f_out = f_c * N                                                            */
/* f_out = (f_ref / R) {B*P + A}                                              */
/*                                                                            */
/* Where:                                                                     */
/*        B >= A                                                              */
/*        A < P                                                               */
/*        N = BP + A                                                          */
/*        f_c = f_ref / R	                                              */
/******************************************************************************/
#define PLL_R_COUNTER	50
#define PLL_A_COUNTER	30
#define PLL_B_COUNTER	135

/******************************************************************************/
/* Setup Arduino (Runs once)                                                  */
/******************************************************************************/
void setup()
{ 
  delay(del);  // Delay just in case needed at initialization
  Serial.begin(9600); // Enabel debug terminal	
  pinMode(SS, OUTPUT); // SS pin
  //  pinMode(ld, INPUT); // LD pin
  SPI.begin(); 
  SPI.setBitOrder(MSBFIRST); // From ADF4118 Datasheet
//  SPI.setDataMode(SPI_MODE0); // Mode0 = Default
//  SPI.setClockDivider(SPI_CLOCK_DIV4); //SPI_CLOCK_DIV4 = Default 
  Serial.print("SPI_Synth, v"); 
  Serial.println(ver);
  Serial.println("(C) Daniel Gallagher 2012");
  
  /******************************************************************************/
  /*  Register load sequence: (Per MiniCircuits datasheet)
  /*  I-Register, F-Register, R-Register, N-Register
  /******************************************************************************/
  Serial.println("=== Writing PLL Registers ===");
  I_Register();           // Initialization Register
  F_Register();           // Write F Register to PLL
  R_Register(PLL_R_COUNTER);	  // Write R Register to PLL	
  N_Register(PLL_B_COUNTER, PLL_A_COUNTER); // Write N Register to PLL
  Serial.println("=== Done Writing PLL ===");
}

/******************************************************************************/
/* Main loop (Runs Continuously)                                           */
/******************************************************************************/
void loop()
{
  // TODO: Add lock detect monitor 
  Serial.println("=== Writing PLL Registers ===");
  I_Register();           // Initialization Register
  F_Register();           // Write F Register to PLL
  R_Register(PLL_R_COUNTER);	  // Write R Register to PLL	
  N_Register(PLL_B_COUNTER, PLL_A_COUNTER); // Write N Register to PLL
  Serial.println("=== Done Writing PLL ===");
}

/******************************************************************************/
/*  Register Setup Functions
/******************************************************************************/
void I_Register(void)
{  
  Serial.println("- Writing I Register -");
  WritePLL(
  0x00, 
  0x00, 
  ADF4118_INITIALIZATION_LATCH
    );
}

void F_Register(void)
{
  Serial.println("- Writing F Register -");
  WritePLL(
  0x00,
  (ADF4118_TIMER_COUNTER_CTRL(1) | ADF4118_FASTLOCK_DISABLED | ADF4118_CHARGE_PUMP_OUTPUT(0)),
  (ADF4118_PHASE_DETECTOR_POLARITY(1) | ADF4118_DIGITAL_LOCK_DETECT | ADF4118_FUNCTION_LATCH)
    );
}

void R_Register(int R_Count)
{
  Serial.println("- Writing R Register -");
  WritePLL(
  (ADF4118_LOCK_DETECT_PRECISION << 4), // Shift by Test mode bits
  ((R_Count >> 6) & 0xFF), // 8 bits of R-counter
  (R_Count &  0x3F) << 2 | ADF4118_R_COUNTER // 6 bits of R-counter
  );
}

void N_Register(int B_Count, int A_Count)
{
  Serial.println("- Writing N Register -");
  WritePLL(
  (ADF4118_CP_GAIN  << 4) | ((B_Count >> 9) & 0x0F), // CP Gain & 4 B-counter bits
  ((B_Count >> 1) & 0xFF), // next 8 bits of B-counter
  ((B_Count &  0x01) << 7) | (A_Count << 2) | ADF4118_N_COUNTER // 1 bit of B-counter + A-counter
  );
}			

/******************************************************************************/
/* Write PLL Shift Register SPI in 3 bytes                                    */
/******************************************************************************/
void WritePLL(byte msb, byte isb, byte lsb)
{ 
  digitalWrite(SS, LOW); // Start to latch
  SPI.transfer(msb); 
  SPI.transfer(isb);
  SPI.transfer(lsb);
  digitalWrite(SS, HIGH);
  delay(del);    // delay befor next call (just in case)  
  
  /*
  // Serial out for for Debug
  Serial.print("MSB: ");
  Serial.println(msb, BIN);
  
  Serial.print("ISB: ");
  Serial.println(isb, BIN);
  
  Serial.print("LSB: ");
  Serial.println(lsb, BIN);
  */
}
