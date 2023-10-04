# cpp-client

<h2>How to Set Up</h2>
We have prepared a make file to simplify the building process. If you are using any editor or IDE, please build using the make file. First, you should install `make`. If you are on macOS or Linux, there is a good chance you already have `make` installed.
notice :you might run to some errors saying some dll file doesnt exist so install them :)
if you have trouble executing make beacuse if dlls download libintl3.dll and libiconv2.dll and place them next to make.exe in the maked bin folder
notice2: make sure to install g++ compiler(you need to worry about it when you have always used visual studio only(withh pretty much other IDEs it automaticly wil ber installed but make sure of that and make sure that its address is in the path(again,it is most probably there)))

first, clone this repository

#### Linux
1- <code>sudo apt-get update</code><br>
2- <code>sudo apt-get install make</code><br>
3- <code>sudo apt-get install libcurl4-openssl-dev</code><br>



#### macOS
1- (if you don't have HomeBrew) <code>/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"</code><br>
You can also install it similarly to Windows, but look for the macOS version on this <a href="https://www.gnu.org/software/make/">link</a>. Just look for binaries, not the source.
<code>brew install make</code>

2- <code>brew install curl-openssl</code>
(If these two commands didn't work, use the instructions for Windows).

<strong>if you got errors using an IDE or editor do the things described in the last paragraph of the windows section</strong>



#### Windows
1- for 64-bit Windows: Download <a href="https://drive.google.com/file/d/1YPL0A9RLq2qBH5X195ZGw0H11tnJg3td/view?usp=sharing">this</a> (it already has the dlls) and
extract it.

&nbsp;&nbsp;&nbsp;&nbsp;for 32-bit Windows: Download <a href="https://sourceforge.net/projects/gnuwin32/files/make/3.81/make-3.81-bin.zip/download?use_mirror=netactuate&download=&failedmirror=deac-riga.dl.sourceforge.net">this</a>, extract it and then download and these dlls: <a href="https://www.dll-files.com/libintl3.dll.html">libintl3.dll</a> and <a href="https://www.dll-files.com/libiconv2.dll.html">libiconv2.dll</a> . place the dlls in the bin folder of the extracted file next to the make.exe file.
then you have to add the bin folder of the extracted file to the system environment variables, or skip this part and run the make.exe file later using the full path like C:/Users/..../bin/make.exe (not recommended)

2- for adding the path to system environment variables:<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1- press the win key and type `edit the system environment variables` and hit enter.<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2- at the bottem of the opened window click environment "variables".<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;3- in the "User variavles" find and select "Path", then click edit.<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;4- click "New" and paste the path to the bin folder or alternatively click browse and navigate to the bin folder  and click ok.<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;5- click ok in the previous windows and close them.<br>


3- Download a binary version of <a href="https://curl.se/windows/">libcurl</a> suitable for you. then extract the downloaded file anywhere.
  
<em>Optional:</em> Add its `include` and `lib` directories to the system environment variables path like step 2.

If you are using a code editor like VS Code and it doesn't recognize `<curl/curl.h>`:
1- hover over the error
2- click on Quick Fix, and go to Include Path settings
3- There you should see a line like <code>One include path per line.</code> Write the path below it, and it will be recognized.
Of course, this is not necessary because you are using `make` command to build, and it's just to prevent annoying editor errors.

4- after cloning this repo, there is a MakeFile in it, edit it and change the `path:` to your own libcurl directory that you downloaded in step 3 (the path that contains include, lib ,etc files). For example, for me, it looks like (path := C:/libcurl).


#### run the program
Now, just run the `make` command in the root folder (if it's not recognized, run it using the full path), and the executable the file named `game` will be built.
if you couldnt run with make use <code>g++ -std=c++17 -Wall -IC:/libcurl/include  resources/Game.o resources/GameHandler.o resources/MainProgram.o client.o -o game -lcurl -LC:/libcurl/lib </code>
if you have windows run this  <code>g++ -std=c++17 -Wall -IC:/libcurl/include  resources/Game.o resources/GameHandler.o resources/MainProgram.o client.o -o game -lcurl -LC:/libcurl/lib -lws2_32</code>and replace  two C:/libcurl parts with your own libcurl path but it will take much more time
notice: for faster compilation you can use multiple cores of cpu for that use  <code> make -j4 </code>and replace 4 with your max cores you want to use