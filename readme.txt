To compile use 'make'. 


To run the master, use: ./listen <port> <total-players> <hop-count>
Note: port range is limited between 65420 to 65500. This number has been arrived at by intensive testing.

To run the player, use: ./speak <master-hostname> <master-port>
Note: if the player can not find ports in the range 65420 to 65500 to bind to, it exits.
