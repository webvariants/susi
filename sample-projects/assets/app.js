var send = null;

$(function(){
	console.log("on document ready!");
	var ws = new WebSocket("ws://"+window.location.host+"/ws");
	ws.onopen = function(){
		ws.send(JSON.stringify({
			type: "registerConsumer",
			data: {
				topic: "chat-channel"
			}
		}));
		setInterval(function(){
			ws.send(JSON.stringify({
				type: "publish",
				data: {
					topic: "session::update"
				}
			}));
		},2000);
	};

	ws.onmessage = function(msg){
		var packet = JSON.parse(msg.data);
		if( packet.type === 'consumerEvent' && packet.data.topic === 'chat-channel'){
			$('#output').append("<p>"+packet.data.payload+"</p>");
		}
	};

	ws.onclose = function(msg){
		console.log(msg);
	};

	chat = function(data){
		ws.send(JSON.stringify({
			type: "publish",
			data: {
				topic: "chat-channel",
				payload: data
			}
		}));
	};

	$('button').on('click',function(){
		chat($('input').val());
		$('input').val('');
	});


});
