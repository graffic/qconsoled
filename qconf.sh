#!/bin/bash
# Script for configuring qconsoled

# This function will initialize configuration parameters
function init_conf(){
if [ -e "qconsoled.conf" ]; then
 cfg=qconsoled.conf
else
 cfg=/etc/qconsoled.conf
fi

if [ -f $cfg ]; then
 . $cfg
 if [ ! -n "$PROGRAM" ]; then $PROGRAM="/nec2/Quake2/quake2 +set dedicated 1"; fi
 if [ ! -n "$PASSWORD" ]; then $PASSWORD="quake"; fi
 if [ ! -n "$CONSOLES" ]; then $CONSOLES="2"; fi
 if [ ! -n "$PORT" ]; then $PORT="10108"; fi
 if [ ! -n "$ISSUE" ]; then $ISSUE="Hi, hellooo!"; fi
fi
}

# Initialize dialog variables
function init_dialog(){
BACKTITLE="Qconsoled V($(cat VERSION))"
TMPFILE="qconf.tmp"
}

# Dialog exit message
function d_salir(){
dialog --backtitle "$BACKTITLE" \
       --title Information \
       --msgbox "Thanks for using Qconsoled configuration program" \
       5 40
clear
exit
}

# Dialog Option Menu
function d_menu_opt(){
dialog --backtitle "$BACKTITLE" \
       --title "$1" \
       --inputbox "$2" 9 60 "$3" 2>$TMPFILE
RETVAL=$?
OPCION=$(cat $TMPFILE)
rm -f $TMPFILE
}

# Dialog Save Menu
function d_menu_save(){
echo "#
# Fichero de configuracion del Qconsoled.
# Qconsoled configuration file.
#
" > $cfg
echo "PROGRAM=\"$PROGRAM\"" >> $cfg
echo "PASSWORD=\"$PASSWORD\"" >> $cfg
echo "CONSOLES=\"$CONSOLES\"" >> $cfg
echo "PORT=\"$PORT\"" >> $cfg
echo "ISSUE=\"$ISSUE\"" >> $cfg
dialog --backtitle "$BACKTITLE" \
       --title "Save Configuration" \
       --msgbox "Saved in $cfg" \
       5 40
d_salir
}
# Dialog Menu loop
function d_menu(){
dialog --backtitle "$BACKTITLE" \
       --title "Select option" \
       --menu "Select a option to change and press ENTER key." 13 50 6 \
        "Program"  "Program to control" \
        "Password" "Keyword to access" \
        "Consoles" "Number of Connections" \
        "Issue" "Initial message" \
        "Port"  "Listening port" \
	"Save"  "Save configuration file" 2> $TMPFILE
RETVAL=$?
OPCION=$(cat $TMPFILE)
rm -f $TMPFILE
case $RETVAL in
 0)
  case "$OPCION" in
   Program)
    d_menu_opt "$OPCION" "Type the name of the program to control with full path and arguments" "$PROGRAM"
    if [ $RETVAL = 0 ]; then
     PROGRAM=$OPCION
    fi;;

   Password)
    d_menu_opt "$OPCION" "Enter the password to allow or deny access in Qconsoled" "$PASSWORD"
    if [ $RETVAL = 0 ]; then
     PASSWORD=$OPCION
    fi;;

   Consoles)
    d_menu_opt "$OPCION" "What number of clients can be conected at the same time?" "$CONSOLES"
    if [ $RETVAL = 0 ]; then
     CONSOLES=$OPCION
    fi;;

   Issue)
    d_menu_opt "$OPCION" "Enter a welcome message" "$ISSUE"
    if [ $RETVAL = 0 ]; then
     ISSUE=$OPCION
    fi;;

   Port)
    d_menu_opt "$OPCION" "Enter the port in wich qconsoled will be listening (10108 recommended)" "$PORT"
    if [ $RETVAL = 0 ]; then
     PORT=$OPCION
    fi;;

   Save)
    d_menu_save;;

  esac;;
 *)
  d_salir;;
esac
}

# Main function 

init_conf
init_dialog
dialog --backtitle "$BACKTITLE" \
       --title Information \
       --msgbox "Welcome to Qconsoled dialog based 
configuration program
       

   By: graffic <graffic@innocent.com>" 9 45

# Main loop
while true; do
 d_menu
done
