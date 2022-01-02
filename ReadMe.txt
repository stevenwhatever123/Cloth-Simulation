For more information, please take a look at "assignment 1(1).pdf"

This program is created for Windows only

Library used: glut, glew, glm

The current program only have icosphere and cube collision detection

Press f to select an obj file for the floor
Press l to select an obj file for the cloth
Press v to start recording
Press 5 to perform task 5 to hold two corners of the cloth
Press 6 to perform task 7 to add positive wind force from the x-axis

Command line code for creating video in "Output" file:
"ffmpeg -r 60 -f image2 -s 1920x1080 -i test%d.tga -vcodec libx264 -crf 25 -pix_fmt yuv420p video.mp4"

It is recommended to delte all files in the "Output" before recording a new video
