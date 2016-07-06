# Data Handler
This application serves as a centralised data handler for the RoomPi system.

All monitoring applications will connect to this application using ZeroMQ over an IPC socket. Each application will transmit the data that has been gathered using this IPC socket.

This application will then handle the reporting of the information to the web service so that it can be logged.