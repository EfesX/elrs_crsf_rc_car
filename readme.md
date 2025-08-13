# RC CAR
[![build](https://github.com/EfesX/elrs_crsf_rc_car/actions/workflows/build.yml/badge.svg)](https://github.com/EfesX/elrs_crsf_rc_car/actions/workflows/build.yml)

## Project Overview

This project implements a remote-controlled car system using an STM32F1 microcontroller. The system receives control signals via CRSF (Crossfire) protocol from a radio transmitter and drives four DC motors.

## Key Features

- _CRSF Protocol Implementation_: Receives and processes RC channel data from a Crossfire-compatible transmitter
- _Differential Drive Control_: Implements tank-style steering by independently controlling two pairs of motors
- _PWM Motor Control_: Uses PWM generation to control motor speed
- Automatic motor stop when radio link is lost
- Arm/disarm functionality via a dedicated RC channel

## Hardware Configuration

- MCU: STM32F1 series (on Blue Pill  Development Board)
- USART2 for CRSF communication
- TIM2 for PWM generation (2 channels)
- GPIO Port A for motor control (4 pins for direction, 2 pins for PWM)
- GPIO Port C for status LED

## Pinout
- PA10 - GPS  UART TX (MCU RX)
- PB7  - GPS  I2C  SDA
- PB6  - GPS  I2C  SCL
- PB10 - CRSF UART RX (MCU TX)
- PB11 - CRSF UART TX (MCU RX)
- PA0  - MOTOR A PWM
- PA4  - MOTOR A IN 1 
- PA5  - MOTOR A IN 2
- PA1  - MOTOR B PWM
- PA6  - MOTOR B IN 1 
- PA7  - MOTOR B IN 2
