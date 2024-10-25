# Gesture Recognition based on STM32G0 Microcontroller and 16 ITR20001/T24 Infrared Sensors
## Intro
- This is about the design process of the course project in 'Comprehensive Design and Practice A' at Tongji.  
- This course design covers circuit schematic design, PCB layout, K-Means algorithm clustering, embedded language development, etc., 
so that the gesture recognizer can sense the rock✊, scissors✌, and paper✋ gestures placed close to the PCB, and display the judgment results in the form of LED lights.  
- You can also find more details in the Report.  

## Demo
<div align=center>
<img src="https://github.com/blackspiderrr/GestureRecognition/blob/master/Image/Test0_Nothing.jpg" width="400"> <img src="https://github.com/blackspiderrr/GestureRecognition/blob/master/Image/Test1_S.jpg" width="400"> <img src="https://github.com/blackspiderrr/GestureRecognition/blob/master/Image/Test2_R.jpg" width="400"> <img src="https://github.com/blackspiderrr/GestureRecognition/blob/master/Image/Test3_P.jpg" width="400"/>
</div>

## Design Process
### Circuit Schematic Design
- Use KiCad  
- Design:
<div align=center>
<img src="https://github.com/blackspiderrr/GestureRecognition/blob/master/Image/CircuitDiagram.png" width="600px">
</div>

### PCB layout
- Use KiCad
- Design:  
<div align=center>
<img src="https://github.com/blackspiderrr/GestureRecognition/blob/master/Image/PCB1.png" width="400"> <img src="https://github.com/blackspiderrr/GestureRecognition/blob/master/Image/PCB2.png" width="400">
</div>  

- 3D Model:  
<div align=center>
<img src="https://github.com/blackspiderrr/GestureRecognition/blob/master/Image/3D.png" width="400px">
</div>

### Welding and Collecting Data
<div align=center>
<img src="https://github.com/blackspiderrr/GestureRecognition/blob/master/Image/Collecting.jpg" width="500px">
</div>
  
### K-Means Clustering Algorithm
- 16-dimensional clustering  
- Look at Folder ‘K-Means’ for more details.  

### Embedded System Development
- Folder 'gesture' is about PC-side training and reasoning, and then pushing result to the board to display (by modifying code about LED).
- Folder 'gesture (2)' is about PC-side training and MCU-side reasoning and display.

## Limitation
- The infrared sensors are not very sensitive, so detection needs to be within a certain distance range.
- LED display has delay, and the interactive effect needs to be improved.
