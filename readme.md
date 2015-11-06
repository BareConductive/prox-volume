[![Bare Conductive](http://bareconductive.com/assets/images/LOGO_256x106.png)](http://www.bareconductive.com/)

# Bare Conductive Proximity to MP3 volume Demo Code

Code demonstrating mapping the proximity of your hand to an electrode connected to E0 of the [Bare Conductive Touch Board](http://www.bareconductive.com/shop/touch-board/). The closer your hand gets to the electrode, the softer the playback volume of MP3 files from the onboard microSD card. **E1** to **E11** play **TRACK001.MP3** to **TRACK011.MP3** from the microSD card.

Recommended electrode design is 85mmx85mm to 100mmx100mm square of [Electric Paint](http://www.bareconductive.com/shop/electric-paint-50ml/).

## Requirements
* You should make sure that you have followed our [Setting up Arduino with your Touch Board](http://www.bareconductive.com/make/setting-up-arduino-with-your-touch-board/) tutorial before using this (or any other) of our code examples


## Install

1. Close the Arduino IDE if you have it open.
1. Download the [.zip](https://github.com/BareConductive/prox-volume/archive/public.zip) or clone the repository to your local machine - if downloading the .zip, extract the contents somewhere that suits you.
1. Take the **prox_volume** folder and move it to **Arduino Sketchbook Folder**. This will be different for each operating system: 

	**Windows**
	
	Libraries\\Documents\\Arduino
	
	or
	
	My Documents\\Arduino	
	
	**Mac**
	
	Documents/Arduino
	
	**Linux (Ubuntu)**
	
	Home/Arduino


	If this folder does not exist, create it first.
1. Reopen the Arduino IDE - you should now be able to open the sketch in the **File -> Sketchbook** menu.
