var io = require('socket.io-client');
var socket = io( 'http://127.0.0.1:10000');
socket.on('connect',function()
          {
    console.log('[서버 연결됨]');
    socket.send('메시지 전송');
    }
);
socket.on( 'message', function(data)
          {
    console.log("[메시지 수신]");
    console.log(data);
    setTimeout(function()
               {
        socket.send('메시지 전송');
    },3000);
});