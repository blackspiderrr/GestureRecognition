# Gesture Recognition based on STM32G0 Microcontroller and 16 ITR20001/T24 Infrared Sensors
## Intro
- This is about the design process of the course project in 'Comprehensive Design and Practice A' at Tongji.  
- This course design covers circuit schematic design, PCB layout, K-Means algorithm clustering, embedded language development, etc., 
so that the gesture recognizer can sense the rock✊, scissors✌, and paper✋ gestures placed close to the PCB, and display the judgment results in the form of LED lights.  

## Demo

## Design Process
### Circuit Schematic Design
- Use KiCad  
- Design:  
Image1 hered

### PCB layout
- Use KiCad  
Image234 here

### Welding
Image5 here

### K-Means Clustering Algorithm
- Look at ‘K-Means’ Folder.    
- The data used was collected after welding and placed into the txt files. 

### Embedded System Development
- Folder 'gesture' is about PC-side training and reasoning, and then pushing result to the board to display (by modifying code about LED).
- Folder 'gesture (2)' is about PC-side training and MCU-side reasoning and display.

## Limitation
- The infrared sensors are not very sensitive, so detection needs to be within a certain distance range.
- LED display has delay, and the interactive effect needs to be improved.
