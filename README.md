# node-DAQmx
node-DAQmx enables RPC's from a node.js server to DAQmx driver functions. Using WebSockets, this is extended further to client-side javascript.

This project uses node-ffi to expose the driver functions to the node.js server. Then, the client uses WebSockets to connect to the server and make calls to the exposed functions.
