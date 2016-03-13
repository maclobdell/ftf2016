var accessKey = '';

var MbedConnector = require('mbed-connector-api');
var EventEmitter = require('events');
var express = require('express');
var app = express();
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
app.use(express.static('public'));

// Get all our endpoints
app.get('/', function(req, res, next) {
  connector.getEndpoints(function(err, endpoints) {
    if (err) return next(err);

    res.render('endpoints.html', { endpoints: endpoints });
  });
});

// The instrument page
app.get('/instrument/:id', function(req, res, next) {
  connector.getResourceValue(req.params.id, 'buzzer/0/recorded', function(err, v) {
    if (err) return next(err);
    
    v = v.split(':').filter(function(f, ix) {
      return ix !== 0 && !!f;
    }).map(function(f) {
      return f.split(',').map(function(k) { return Number(k); });
    });

    res.render('instrument.html', { id: req.params.id, recorded: JSON.stringify(v) });
  });
});

app.put('/notification', function(req, res, next) {
  connector.handleNotifications(req.body);
  res.send('OK');
});

// Notifications are sent over a web socket
var notifications = new EventEmitter();
connector.on('registration', function(n) {
  connector.putResourceSubscription(n.ep, 'buzzer/0/recorded', function(err) {
    console.log(n.ep, 'putResourceSubscription', err ? err : 'Success');
  });
});

connector.on('notification', function(n) {
  console.log('notification', n.ep + n.path);
  notifications.emit(n.ep + n.path, n.payload);
});

io.on('connection', function(socket) {
  socket.on('subscribe-recorded', function(id) {
    notifications.on(id + '/' + 'buzzer/0/recorded', function(v) {
      console.log('hi there', v);
      v = v.split(':').filter(function(f, ix) {
        return ix !== 0 && !!f;
      }).map(function(f) {
        return f.split(',').map(function(k) { return Number(k); });
      });
      
      socket.emit('recorded', v);
    });
  });
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
