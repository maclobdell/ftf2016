var accessKey = '';

var MbedConnector = require('mbed-connector-api');
var EventEmitter = require('events');
var app = require('express')();
var server = require('http').Server(app);
var io = require('socket.io')(server);
var bodyParser = require('body-parser');

if (!accessKey) {
  throw 'No access key set! Go to https://connector.mbed.com/#accesskeys to get one!';
}

var connector = new MbedConnector({
  accessKey: accessKey
});

app.set('view engine', 'html');
app.engine('html', require('hbs').__express);
app.use(bodyParser.json());

// Get all our endpoints
app.get('/', function(req, res, next) {
  connector.getEndpoints(function(err, endpoints) {
    if (err) return next(err);

    res.render('endpoints.html', { endpoints: endpoints });
  });
});

// The instrument page
app.get('/instrument/:id', function(req, res, next) {
  // connector.getResourceValue(req.params.id, KNOCK_RESOURCE, function(err, value) {
  //   if (err) return next(err);

    res.render('instrument.html', { id: req.params.id });
  // });
});

app.put('/notification', function(req, res, next) {
  connector.handleNotifications(req.body);
  res.send('OK');
});

// Notifications are sent over a web socket
var notifications = new EventEmitter();
connector.on('notifications', function(data) {
  data.forEach(function(n) {
    notifications.emit(n.ep + '/' + n.path, n.payload);
  });
});

io.on('connection', function(socket) {
  socket.on('play', function(id) {
    console.log(id, 'gonna play');
    connector.postResource(id, '/buzzer/0/play', '', function(err) {
      console.log(id, 'play', err ? err : 'Success');
    });
  });
  
  socket.on('set-music', function(id, tones, duration) {
    console.log(id, 'Set music', tones.length, duration.length);
    
    connector.putResourceValue(id, '/buzzer/0/notes', tones.join(':'), function(err) {
      console.log(id, 'Set tones', err ? err : 'Success');
      if (err) return;
      
      connector.putResourceValue(id, '/buzzer/0/duration', duration.join(':'), function(err) {
        console.log(id, 'Set duration', err ? err : 'Success');
        
        socket.emit('set-music-success');
      });
    });
  });
  
  // socket.on('subscribe-knocks', function(id) {
  //   connector.putResourceSubscription(id, KNOCK_RESOURCE, function() {});
  //   notifications.on(id + '/' + KNOCK_RESOURCE, function(data) {
  //     socket.emit('knock', data);
  //   });
  //   socket.on('disconnect', function() {
  //     connector.deleteResourceSubscription(id, KNOCK_RESOURCE, function() {});
  //   });
});


server.listen(process.env.PORT || 8210, function() {
  console.log('Listening on port', process.env.PORT || 8210);

  var n = 'http://' + process.env.C9_HOSTNAME + '/notification';
  // console.log(n);

  connector.putCallback({ url: n }, function(err) {
    if (err) return console.error(err);

    console.log('Click this link: http://' + process.env.C9_HOSTNAME);
  });
});
