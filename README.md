## Ship Battle Game - Client Server Application
#Description
The project consists of the development of a client/server. Both the server and the client must be mono-process and exploit the I/O multiplexing to handle multiple inputs and outputs simultaneously. The application to be developed is the game of SHIP BATTLE following the paradigm peer2peer. The server will be responsible for storing users connected and the ports.

The exchange of information between client and server will be through TCP sockets. This information will only control information that will be used to implement the communication peer2peer. Exchange text messages between clients will be via UDP socket.

##Client
The client must be started with the following syntax
```
./battaglia_navale_client <server address> <port>
```

The commands available to the user should be:
```
!help - shows the list of commands
!who - shows the list of connected users 
!quit - disconnects user from server
!connect user_name - starts a match against user_name 
!disconnect - disconnects user from current match
!show_enemy_map - shows enemy map 
!show_my_map - shows user map
!hit coordinates - hit enemy's map at coordinates
```
While connecting the client has to enter his username and listen port for UDP commands for the game.

##Server
The program battaglia_navale_server handles requests from clients. The server accepts new tcp connections , registers new users and manages the demands of various clients to open new matches.

The command syntax is as follows:

```
./battaglia_navale_server <host> <port>
```
