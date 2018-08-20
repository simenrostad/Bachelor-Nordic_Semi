# Bachelor@Nordic_Semi

Energy Harvesting Stop Button (EHSB)

The group consists of:
Jonathan Lange Lundaas
Besart Olluri
Tor André Melheim
Simon Rømo
Simen S. Røstad

We wrote our Bachelor thesis at NTNU under the guidance of Nordic Semiconductor where the assignment was to make an energy harvesting stop button to replace todays stop buttons in buses, using the nRF52832 SoC and Bluetooth Low Energy (BLE).

The assignment was split into two main aspects, hardware and software.

The hardware aspect consists of the following:
 - Finding a sufficient energy source
 - Circuit design and energy storage
 - Setting up an energy budget
 - Power management
 - Antenna tuning
 - Mechanical consrutciton
 - Production (Component orders and soldering)
 - 3D-printing button chassis
  
  
  The software aspect consists of the following:
  - Firmware code for the central, relayer and stop button beacon units
  - Complete system communication
  - System and code optimization

  The final solution as of May 2018 was able to advertise five BLE events over the span of approximately 100ms by one push of the button, and is a proof of concept solution that can be utilized in the intended environment.
