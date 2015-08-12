# node-DAQmx
node-DAQmx enables Remote Procedure Calls (RPCs) from a node.js server to DAQmx driver functions. Using WebSockets, this is extended further to client-side javascript.

This project uses node-ffi to expose the driver functions to the node.js server. Then, the client uses WebSockets to connect to the server and make calls to the exposed functions.

# Running instructions
# Requirements
1. Make sure you have DAQmx drivers on the computer you plan to run the node server on
2. Also make sure you have DAQmx hardware plugged into theat same computer, or a simulated channel using NI MAX
3. Install node.js https://nodejs.org/

# Make it work
1. In the top directory, do an npm install to get the packages installed
2. Run node ffi-app.js
3. Go to localhost:3000

# Do continuous data aquisition
1. Select a channel from the dropdown menu
2. Adjust the sample rate, max voltage, and min voltage
3. Click setup for collection, then click run
4. At the moment, you will have to restart the node app and refresh the page to select a different channel or change any settings

# Do digital I/O
Somewhat hardcoded for now, to be improved upon
1. Click set up for digital I/O
2. Click read to read in the state of the digital port on the DAQ device
3. Click some of the checkbox buttons to change them, then click write to write those values to the DAQ device
