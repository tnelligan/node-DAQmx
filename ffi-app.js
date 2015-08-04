var app = require("express")(),
  http = require("http").Server(app),
  io = require("socket.io")(http),
  request = require("request"),
  ref = require('ref'),
  ArrayType = require('ref-array'),
  ffi = require('ffi');

app.get("/", function (req, res) {
  res.sendfile("index.html");
});

http.listen(3000, function () {
  console.log("listening on *:3000");
});

var NIint32 = ref.refType('int');
var ptrChar = ref.refType(ref.types.CString);
var TaskHandle = ref.refType('void');
var ReadData = ref.refType('int32');

var Double = ref.types.double;
var DoubleArray = ArrayType(Double);

var EveryNCallback = ffi.Function('int32', [TaskHandle, 'int32', 'uint32', TaskHandle]);

var niDAQmx = ffi.Library('nicaiu', {
  'DAQmxGetDevAIPhysicalChans': ['int32', ['string', ptrChar, 'ulong']],
  'DAQmxGetSysDevNames': ['int32', [ptrChar, 'int32']],
  'DAQmxGetSysNIDAQMajorVersion': ['int32', [NIint32]],
  'DAQmxGetSysNIDAQMinorVersion': ['int32', [NIint32]],
  'DAQmxCreateTask': ['int32', ['string', TaskHandle]],
  'DAQmxCreateAIVoltageChan': ['int32', [TaskHandle, 'string', 'string', 'int32', 'double', 'double', 'int32', 'string']],
  'DAQmxCreateDOChan': ['int32', [TaskHandle, 'string', 'string', 'int32']],
  'DAQmxCfgSampClkTiming': ['int32', [TaskHandle, 'string', 'double', 'int32', 'int32', 'uint64']],
  'DAQmxStartTask': ['int32', [TaskHandle]],
  'DAQmxReadAnalogF64': ['int32', [TaskHandle, 'int32', 'double', 'int32', DoubleArray, 'uint32', ReadData, 'uint32']],
  'DAQmxWriteDigitalScalarU32': ['int32', [TaskHandle, 'bool', 'double', 'int32', 'bool*']],
  'DAQmxReadDigitalScalarU32': ['int32', [TaskHandle, 'double', 'int32*', 'bool*']],
  'DAQmxRegisterEveryNSamplesEvent': ['int32', [TaskHandle, 'int32', 'uint32', 'uint32', EveryNCallback, TaskHandle]],
  'DAQmxWaitForNextSampleClock': ['int32', [TaskHandle, 'double', 'bool*']]
});

var stop = false;
var running = false;

var newTask;
var dict = {};

io.on("connection", function (socket) {
  console.log("a user connected");

  socket.on("disconnect", function () {
    console.log("user disconnected");
  });

  socket.on('getDevNames', function (fn) {
    var devices = new Buffer(1000);
    devices.type = ref.types.char;
    niDAQmx.DAQmxGetSysDevNames(devices, devices.length);
    var deviceArray = ref.readCString(devices, 0).split(', ');
    fn(deviceArray);
  });

  socket.on('getPhysChans', function (device, fn) {
    var buf = new Buffer(1000);
    buf.type = ref.types.char;
    niDAQmx.DAQmxGetDevAIPhysicalChans(device, buf, buf.length);
    var channelString = ref.readCString(buf, 0).split(', ');
    fn(channelString);
  });

  socket.on('createTaskHandle', function (taskID, fn) {
    var task = new Buffer(1000);
    task.type = ref.types.void;
    niDAQmx.DAQmxCreateTask('', task);
    newTask = ref.readPointer(task, 0);
    dict[taskID] = newTask;
    console.log(dict[taskID]);
    fn(taskID);
  });

  socket.on('createAnalogVoltageChannel', function (conf, ack) {
    console.log(niDAQmx.DAQmxCreateAIVoltageChan(dict[conf.taskID], conf.device, conf.nameToAssignChannel, conf.terminalConfig, conf.minVal, conf.maxVal, conf.units, ref.NULL));
    ack();
  });

  socket.on('configSampleClock', function (conf, ack) {
    niDAQmx.DAQmxCfgSampClkTiming(dict[conf.taskID], conf.source, conf.rate, conf.activeEdge, conf.sampleMode, conf.sampsPerChanToAcquire); //10178 finite, 10123 cont
    ack();
  });

  socket.on('startTask', function (task, ack) {
    niDAQmx.DAQmxStartTask(dict[task]);
    ack();
  });

  socket.on('readAnalogData', function (conf, ack) {
    var data = new DoubleArray(1000);
    var read = new Buffer(1000);
    read.type = ref.types.int32;

    niDAQmx.DAQmxReadAnalogF64(dict[conf.taskID], conf.numSampsPerChan, conf.timeout, conf.fillMode, data, conf.arraySizeInSamps, read, ref.NULL);
    //console.log(data[0]);
    ack(data.toArray());
  });

  socket.on('createDigitalOutputChannel', function (conf, ack) {
    console.log(niDAQmx.DAQmxCreateDOChan(dict[conf.taskID], conf.lines, conf.nameToAssignToLines, conf.lineGrouping));
    ack();
  });

  socket.on('writeDigitalPort', function (conf, ack) {
    console.log(niDAQmx.DAQmxWriteDigitalScalarU32(dict[conf.taskID], conf.autoStart, conf.timeout, conf.dataLayout, ref.NULL));
    ack();
  });

  function decbin(dec, length) {
    var out = "";
    while (length--)
      out += (dec >> length) & 1;
    return out;
  }

  socket.on('readDigitalPort', function (conf, ack) {
    var read = new Buffer(1000);
    read.type = ref.types.int32;
    console.log(niDAQmx.DAQmxReadDigitalScalarU32(dict[conf.taskID], conf.timeout, read, ref.NULL));
    ack(decbin(ref.deref(read), 4));
  });

  socket.on('waitForNextClock', function (conf, ack) {
    var late = new Buffer(1000);
    late.type = ref.types.bool;
    var error = niDAQmx.DAQmxWaitForNextSampleClock(dict[conf.taskID], -1, late);
    //console.log(error);
    ack();
  });

});