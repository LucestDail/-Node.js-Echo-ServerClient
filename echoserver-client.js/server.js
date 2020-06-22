// 모듈 socket.io 패키지 import, port : 10000 으로 통신 예정
var io = require('socket.io').listen(10000)
// 소켓 이벤트"connection" 경우 callback 통하여 socket 변수에 정보 가져옴
io.on('connection', function(socket)
             {
    // 정상적 callback 받아 connection 이벤트 성공할 경우 다음 log 출력
    console.log('someone connect.');
    // 소켓 지정(SOCKET s;)
    socket.on('message', function(data)
              {
        console.log('[메시지 수신]\n');
        console.log(data);
        socket.broadcast.send(data); // send to everyone EXCEPT 'socket' 
        socket.send(data);
        console.log('[메시지 에코함]');
    });
    socket.on('disconnect', function()
          {
    console.log('someone disconnect.');
    socket.broadcast.send('someone disconnect.');
    });
});


