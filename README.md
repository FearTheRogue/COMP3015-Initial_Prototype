# Initial Prototype

### Foggy Park Scene

---
<p align = center>
<img align = "centre" src= "https://raw.githubusercontent.com/FearTheRogue/COMP3015-Initial_Prototype/main/readme%20images/ip-scene.png" width = "600">
</p>

---

Visual Studios Version:
> Microsoft Visual Studio Community 2019
> 
> Version 16.11.9

Operating System:
> Windows 10 Home

## Starting Point

* The "Project Starter Template" that Marius supplied was used to initially setup this project

## How does the project work?

* The project does not require any user input in order to work.

  Executable Project
  * Just double click Project_Templete.exe to run

  Visual Studios
  * Click "Local Windows Debugger" or F5 to run

### Nothing appearing when running project?

If you run the project and nothing appears in the window, please download the textures and model files, [here](https://liveplymouthac-my.sharepoint.com/:f:/g/personal/james_bridgman_students_plymouth_ac_uk/ElCYqxI7hwtKjD7utluDQOwBOTMpgHxd37Vt9Tmk8f1E9w?e=5avT6c). 
Then Unzip the downloaded folder and place the contents in the media folder.

## How does the code fit together, and how should a programmer navigate it?

scene_initial_prototype.cpp
> * The constructor loads all the models used in the project
>
> initScene()
>
> * Where the values are set for the fog (MinDistance, MaxDistance and Colour), and setting the majority of values for the spotlight (ambient, diffuse, exponent and cutoff)
> * Loading and binding the textures
>
> render()
>
> * Set the other spotlight values (position and direction)
> * Setting material values, changing the scale, rotation or transform, setting the texture and finally rendering each model 

sceneShader.vert
> * Pretty basic file
> * Calculates the Position and Normals to eye coordinates
> * Then passes them to the .frag, along with the texture coordinates

sceneShader.frag
> main()
> * Calculate the fog, and uses blinnPhongShot() for colour
> 
> blinnPhongSpot()
> * Uses the RGB values from the texture colour to calculate the ambient and diffuse. Cutoff the spotlight using the cutoff value
> * Finally, returns the combined colour value  

## YouTube Video

Click the image below to view the video

<p align = center>

  [<img src= "https://raw.githubusercontent.com/FearTheRogue/COMP3015-Initial_Prototype/main/readme%20images/yt-image.png" width = "500">](https://youtu.be/f7TIqJrXlVs)

</p>
