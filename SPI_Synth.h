/******************************************************************************/
/* SPI_Synth.h
/* 
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

/******************************************************************************/
/* Variables and Pins                                                         */
/******************************************************************************/
// int SS=10; // using digital pin 10 for SPI slave select
int del=200; // used for various delays in microseconds
// int ld=9; // using digital pin 9 for Lock Detect

/******************************************************************************/
/* ADF4118 Control Bits                                                       */
/******************************************************************************/
/* ADF4118 Data Latch */
#define ADF4118_R_COUNTER			0
#define ADF4118_N_COUNTER			1
#define ADF4118_FUNCTION_LATCH			2
#define ADF4118_INITIALIZATION_LATCH		3

/******************************************************************************/
/* ADF4118 Function Latch / Initialization Latch                              */
/******************************************************************************/

/* ADF4118 Power Modes */
/* Note, these values (shifts>7) will need to be modified prior to use.       */
#define ADF4118_NORMAL_OPERATION		((0 << 19) | (0 << 3))
#define ADF4118_ASYNCHRONOUS_POWER_DOWN		((0 << 19) | (1 << 3))
#define ADF4118_SYNCHRONOUS_POWER_DOWN		((1 << 19) | (1 << 3))


/* ADF4118 Timer Counter Control */
/* 4-bits (0<=x<=15)             */
#define ADF4118_TIMER_COUNTER_CTRL(x)	((x) << 4)

/* ADF4118 Fastlock Modes */
#define ADF4118_FASTLOCK_DISABLED		((0 << 3) | (0 << 1))
#define ADF4118_FASTLOCK_MODE_1			((0 << 3) | (1 << 1))
#define ADF4118_FASTLOCK_MODE_2			((1 << 3) | (1 << 1))

/* ADF4118 Charge Pump Output */
#define ADF4118_CHARGE_PUMP_OUTPUT(x)		(x)

/* ADF4118 Phase Detector Polarity */
#define ADF4118_PHASE_DETECTOR_POLARITY(x)	((x) << 7)

/* ADF4118 Outputs */
#define ADF4118_THREE_STATE_OUTPUT		(0 << 4)
#define ADF4118_DIGITAL_LOCK_DETECT		(1 << 4)
#define ADF4118_N_DIVIDER_OUTPUT		(2 << 4)
#define ADF4118_AVDD				(3 << 4)
#define ADF4118_R_DIVIDER_OUTPUT		(4 << 4)
#define ADF4118_ANALOG_LOCK_DETECT		(5 << 4)
#define ADF4118_SERIAL_DATA_OUTPUT		(6 << 4)
#define ADF4118_DGND				(7 << 4)

/* ADF4118 Counter Operation */
/* Used to reset A, B & R Counters using F-Register */
#define ADF4118_COUNTER_OPERATION(x)		((x) << 2)

/* ADF4118 Lock Detect Precision */
#define ADF4118_LOCK_DETECT_PRECISION         1  // 1 - 5 cycles before LD set

/* ADF4118 CP Gain */
#define ADF4118_CP_GAIN                       1  // 1 - CP Gain = 1mA