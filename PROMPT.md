Using the understanding of the keys used in a linux .desktop file as seen below...

[Desktop Entry]
Type=Application
Name=MyApp
Comment=This is my custom application
Exec=/usr/bin/myapp %F
Icon=myapp
Terminal=false
Categories=Utility;Development;

I want to write a tool in bash that will allow a user to be able to create .desktop file fora linux desktop environment.  This tool would be a bash script that would use zenity for the input of .desktop key data.  Zenity would use a form with multiple input fields to capture ALL of the key data.  Once the key is filled with data, that field must continue to display the key data, so it can be reviewed at anytime.  Below are the Key data Requirements & Functions.  Below that you will find the Button Functions & Requirements.  After all input key fields have data entered it would then offer 4 function below.  See Button Functions below.

Key data Requirements & Functions

* Type=Application            Offer Application, Link, and Directory as options from a drop down menu.
* Name=MyApp                Offer user to add their own name
* Comment=This is my custom application    Offer user to add their own comment
* Exec=/usr/bin/myapp-executable    Offer user to browse to an executable file of their choice.  You must include the full path to the executable file in the key field.
* Icon=myapp                Offer user to browse to an icon file of their choice.  You must include the full path to the icon file in the key field.
* Terminal=false            Offer false, true as options from a drop down menu.
* Categories=Utility;Development    Offer Accessories, Graphics, Internet, Office, Other, Programming, Sound & Video, System Tools, and Utilities options from a drop down menu.

Button Functions & Requirements

Button #1 Called Generate, This button has multiple functions when pressed.
    Generate function A.       Display a pop up window showing all contents of the .desktop file, so its independent config lines "Keys" can be reviewed to confirm proper formatting, awaiting user approval from the user which is satisfied with an Okay button click. 
    Generate function B.    Upon user approval it would generate the actual .desktop file.

Button #2 Called Download, This button has multiple functions when pressed.
    Download function A.    When pressed ask the user for the full file name of the file to be saved. (example:  test.desktop), and then Offer an Okay button.
    Download function B.    When User presses Okay button from function A it would save the file with its new filename to the user's desktop folder, which should be /home/%username%/ where username is the logged in user running the tool.
    Download function C.    After user saves the file to their desktop I need you to mark the .desktop file as trusted, so it can be run wihtout manaully trusting it.
    Download function D.    After saving the .desktop file to the users desktop I need you to save a copy to the location ~/.local/share/applications/, and mark it as trusted as well.

Button #3 Called Reset, This button has multiple functions when pressed.
    Reset function A.    This button when pressed will reset all zenity input fields with original information resetting all user entered data.  This will allow the user to reset the tool over again.

Button #4 Call Quit, This button has multiple functions when pressed.
    Quit function A.    This button when pressed will close the zenity portion of the tool, and exit the bash script underneath with a proper exit code of 1 
