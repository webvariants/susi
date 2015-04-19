$(function(){
	susi = new Susi(function(susi){
		susi.registerConsumer("chat-channel",function(event){
			$('#output').append("<p>"+event.payload+"</p>");
		});
	});
	
	$('button').on('click',function(){
		susi.publish("chat-channel", $('input').val());
		$('input').val('');
	});
});
