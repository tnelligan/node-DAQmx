var WebSocketServer = require('ws').Server,
    http = require('http'),
    express = require('express'),
    app = express(),
    ref = require('ref'),
    ArrayType = require('ref-array'),
    ffi = require('ffi');

app.use(express.static(__dirname));

var server = http.createServer(app);
server.listen(5000);

var wss = new WebSocketServer({
    server: server
});

// define C types to that will be used as arguments for FFI
// These are neccesary because there is no string representation of these by default
var Double = ref.types.double;
var DoubleArray = ArrayType(Double);

var EveryNCallback = ffi.Function('int32', ['void*', 'int32', 'uint32', 'void*']);


// Foreign Function Library

//TODO: add checking to see if they have the driver
var niDAQmx = ffi.Library('nicaiu', {
  'DAQmxGetDevAIPhysicalChans': ['int32', ['string', 'char**', 'ulong']],
  'DAQmxGetSysDevNames': ['int32', ['char**', 'int32']],
  'DAQmxGetSysNIDAQMajorVersion': ['int32', ['int*']],
  'DAQmxGetSysNIDAQMinorVersion': ['int32', ['int*']],
  'DAQmxCreateTask': ['int32', ['string', 'void*']],
  'DAQmxCreateAIVoltageChan': ['int32', ['void*', 'string', 'string', 'int32', 'double', 'double', 'int32', 'string']],
  'DAQmxCreateDOChan': ['int32', ['void*', 'string', 'string', 'int32']],
  'DAQmxCfgSampClkTiming': ['int32', ['void*', 'string', 'double', 'int32', 'int32', 'uint64']],
  'DAQmxStartTask': ['int32', ['void*']],
  'DAQmxReadAnalogF64': ['int32', ['void*', 'int32', 'double', 'int32', DoubleArray, 'uint32', 'int32*', 'uint32']],
  'DAQmxWriteDigitalScalarU32': ['int32', ['void*', 'bool', 'double', 'int32', 'bool*']],
  'DAQmxReadDigitalScalarU32': ['int32', ['void*', 'double', 'int32*', 'bool*']],
  'DAQmxRegisterEveryNSamplesEvent': ['int32', ['void*', 'int32', 'uint32', 'uint32', EveryNCallback, 'void*']],
  'DAQmxWaitForNextSampleClock': ['int32', ['void*', 'double', 'bool*']]
});

var dict = {};

wss.on('connection', function (ws) {
    console.log('client connected');
    ws.on('close', function () {
        console.log('client disconnected');
    });
    ws.on('message', function (msg) {
        try
        {
            msg = JSON.parse(msg);
            console.log(msg);
            switch(msg.type) {
                case 'getSysDevNames':
                    //TODO: tnelligan make buffers able to adapt to the size they need to be
                    var devices = new Buffer(1000);
                    devices.type = ref.types.char;
                    niDAQmx.DAQmxGetSysDevNames(devices, devices.length);
                    var deviceArray = ref.readCString(devices, 0).split(', ');
                    console.log(deviceArray);
                    var response = {
                        'type': 'getSysDevNamesResponse',
                        'data': deviceArray
                    }
                    ws.send(JSON.stringify(response));
                    break;
                case 'getDevAIPhysicalChans':
                    var buf = new Buffer(1000);
                    buf.type = ref.types.char;
                    niDAQmx.DAQmxGetDevAIPhysicalChans(msg.data.device, buf, buf.length);
                    var channelString = ref.readCString(buf, 0).split(', ');
                    console.log(channelString);
                    var response = {
                        'type': 'getDevAIPhysicalChansResponse',
                        'data': channelString
                    }
                    ws.send(JSON.stringify(response));
                    break;
                case 'createTask':
                    var task = new Buffer(1000);
                    task.type = ref.types.void;
                    niDAQmx.DAQmxCreateTask('', task);
                    newTask = ref.readPointer(task, 0);
                    dict[msg.data.taskID] = newTask;
                    console.log(dict[msg.data.taskID]);
                    var response = {
                        'type': 'createTaskResponse',
                        'data': msg.data.taskID
                    }
                    ws.send(JSON.stringify(response));
                    break;
                case 'createAIVoltageChan':
                    niDAQmx.DAQmxCreateAIVoltageChan(dict[msg.data.taskID], msg.data.device, msg.data.nameToAssignChannel, msg.data.terminalConfig, msg.data.minVal, msg.data.maxVal, msg.data.units, ref.NULL);
                    break;
                case 'cfgSampClkTiming':
                    niDAQmx.DAQmxCfgSampClkTiming(dict[msg.data.taskID], msg.data.source, msg.data.rate, msg.data.activeEdge, msg.data.sampleMode, msg.data.sampsPerChanToAcquire);
                    break;
                case 'startTask':
                    niDAQmx.DAQmxStartTask(dict[msg.data.taskID]);
                    break;
                case 'readAnalogF64':
                    var data = new DoubleArray(1000);
                    var read = new Buffer(1000);
                    read.type = ref.types.int32;

                    niDAQmx.DAQmxReadAnalogF64(dict[msg.data.taskID], msg.data.numSampsPerChan, msg.data.timeout, msg.data.fillMode, data, msg.data.arraySizeInSamps, read, ref.NULL);
                    var response = {
                        'type': 'readAnalogF64Response',
                        'data': data.toArray().toString()
                    }
                    ws.send(JSON.stringify(response));
                    break;
                case 'createDOChan':
                    niDAQmx.DAQmxCreateDOChan(dict[msg.data.taskID], msg.data.lines, msg.data.nameToAssignToLines, msg.data.lineGrouping);
                    break;
                case 'writeDigitalScalarU32':
                    niDAQmx.DAQmxWriteDigitalScalarU32(dict[msg.data.taskID], msg.data.autoStart, msg.data.timeout, msg.data.dataLayout, ref.NULL);
                    var response = {
                        'type': 'writeDigitalScalarU32Response',
                        'data': 'none'
                    }
                    ws.send(JSON.stringify(response));
                    break;
                case 'readDigitalScalarU32':
                    var read = new Buffer(1000);
                    read.type = ref.types.int32;
                    niDAQmx.DAQmxReadDigitalScalarU32(dict[msg.data.taskID], msg.data.timeout, read, ref.NULL);
                    var response = {
                        'type': 'readDigitalScalarU32Response',
                        'data': ref.deref(read).toString()
                    }
                    ws.send(JSON.stringify(response));
                    break;
                case 'waitForNextSampleClock':
                    var late = new Buffer(1000);
                    late.type = ref.types.bool;
                    niDAQmx.DAQmxWaitForNextSampleClock(dict[msg.data.taskID], -1, late);
                    var response = {
                        'type': 'waitForNextSampleClockResponse',
                        'data': 'none'
                    }
                    ws.send(JSON.stringify(response));
                    break;
            }
        }
        catch (err) 
        {
            console.log('message could not be interpreted');
        }
    });
});