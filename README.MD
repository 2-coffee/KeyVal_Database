# Redis-like database

The goal of this project is to explore how features are implemented.

## Core Features:

In-memory key-value store (strings only for now)  
Basic commands: SET key value, GET key, DEL
TCP server that accepts client connections (like Redis)  
Command parsing  
RESP protocol (optional, for realism)  

## Plan:

1) Start with a basic server where multiple can connect to one instance  

2) Parse simple commands: GET, SET, DELETE

3) Store data in-memory

4) Respond to commands and ask for replications

