One of the assignments that we had to do for our Systems Programming class during our 3rd year of our Bachelor's.

What made this assignment particularly challenging was that we had to use the DirectSound API, which was originally 
released in 1995. Surprisingly there was not much documentation concerning this API, so it was mostly an assignment 
of trial and error and try to figure why some methods worked and why other did not. This was even more of a challenge since many
aspects of C++ and IDEs have changed since the mid 90s, which resulted in certain methods outright braking the program.
In the end I was able to make it work, with a very peculiar way of using two different visual studio projects. Details on the operation
of the program will follow. 
Another point that should be made is that the use of Visual Stuio made the whole situation even more difficult
due to it being an awkward IDE to use. The whole program screams Microsoft, and not in a good way...

As you will understand by reading or using the application, there is a lot of room for improvement for this particular project, but
there was never enough time to improve it after submitting it. Eventually it will get the time it deserves.

----------------------------------------------------
----This is a guide on how the application works----
----------------------------------------------------

- There are two folders in the zip file. This was the only way I could make it work. First the user must open the audioPlayer and run the .sln. Then the user
in Visual studio has to right click on the Solution 'audioMediaPlayer', click add, existing project and select the audioPlayerClient.


- Once the user has opened the project folder in Visual Studio Community (the 2022 version would be prefereable to avoid any issues) they must make sure
that their compiler is using C++ 17 or later, since the default is C++14. C++ 14 does not work with the application. In order to do that in Visual Studio, the user 
must click the audioMediaPlayer (on the right side, underneath the Solution) and click on properties. In the General Configuration section, click on General
and then select the C++ Language standard and choose, C++ 17.

- The user must then right-click on the Solution on the right side of the IDE, and click on properties.Go to Startup Project and select multiple startup projects and make sure that
audioMediaPlayer and audioMediaClient are set to "Start". Then click apply and close the properties window.

- Once that is done, the user must also make sure that they are running the application in "Release" (x64 NOT x86) mode and not "Debug" mode. For some reason
the program will not launch if it is set to release mode. I spent many days trying to fix it, unfortunately to no avail.

- The user must add their .wav audio files to the project folder in order for the audio to play. So, they must first run the program without an audio file in order for the 
release folder to be created and then the user can add their .wav audio file.

- Once an audio file has been inserted, the user can then proceed to launch the applicaiton, either via the IDE (still in "Release" mode). Or they can run the
.exe in the "Release" folder

- Once these steps are done, the user can run the application, and two windows will appear, one for the server and one for the client. The server will state of eith a successful or a failed connection.
If the connection was successful, the audio file should play immediately. Once one audio file has finished playing, the next audio file (if there is one) will start to play.

- Unfortunately, the user cannot choose which song they play. Furthermore, they cannot pause or start playing the next song, until the currently playing song has finsihed playing.
