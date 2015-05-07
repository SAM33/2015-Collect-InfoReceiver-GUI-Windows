collectl info receiver GUI-Version for windows users <br/>
For Linux or Mac users : https://github.com/SAM33/infoReceiver-guiversion
<br/>
<br/>
STEPS:<br/>
1.Install collectl in the system you want to keep watch on
<br/>
sudo apt-get install collectl
<br/><br/>
2.Make collectl info receiver GUI-Version in The monitor's computer
<br/>
cd collectl info receiver(GUI-Version) folder
<br/>
WindowsMakeClean.bat
<br/>
WindowsMake.bat
<br/><br/>
3.Start collectl info receiver GUI-Version
<br/>
main_release.exe
<br/><br/>
4.run collectl in the system you want to keep watch on, for example:
<br/>
collectl -A [ip you just got]:4000 -scmn -P
<br/>
don't forget -P option because we use plot format to commuicate.
<br/><br/>
5.this program can support 4 computers, for example:
<br/>
computer1:
<br/>
collectl -A [ip you just got]:4000 -scmn -P
<br/>
computer2:
<br/>
collectl -A [ip you just got]:4001 -scmn -P
<br/>
computer3:
<br/>
collectl -A [ip you just got]:4002 -scmn -P
<br/>
computer4:
<br/>
collectl -A [ip you just got]:4003 -scmn -P
<br/><br/>
6.look at terminal to control this program
<br/><br/>

