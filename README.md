# Door_Lock_System

A password based door locking system designed for a set of IDs and corresponding passwords. A user should enter the ID (maximum 3 attempts) followed by the password (maximum 3 attempts) onto a number keypad. A correct ID followed by the corresponding password opens the door. If the number of attempts exceeds three for each ID or Password, the system freezes for 30 seconds. A kill switch is implemented to unlock the door from the inside of the room.

In the door lock, a screw mechanism driven by a servo motor is used to lock/unlock the door. An array of LEDs and a buzzer are used to indicate whether the entered IDs and passwords are accepted and a separate green LED is used to indicate the door suddenly opening if the kill switch is pressed.

LED states:
• Red LED – Incorrect ID or Password
• Yellow LED – Correct ID (waiting for correct password)
• Green LED – Access granted
• Green LED in Unlocking system- suddenly opened from inside

Buzzer will beep once if an incorrect ID or password is entered and will beep continuously when the system is frozen for 30 seconds.

The system times out and returns to initial standby state, if it is left to idle for a long time. The reset switch can be used to reset the system in case of system failure.
