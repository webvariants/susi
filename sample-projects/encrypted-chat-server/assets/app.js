var susi = null,
	users = null,
	crypter = new JSEncrypt(),
	name = "anonymous";

var chat = function(msg){
	susi.publish("message",{msg: msg, name: name});
};

var whisper = function(msg,id){
	var key = users[id].key;
	var orig = crypter.getKey();
	crypter.setPublicKey(key)
	susi.publish("whisper@"+users[id].key,{msg: crypter.encrypt(msg), name: name});
	$('#message-output').append("<li class='list-group-item'>you whispered to "+users[id].name+": "+msg+"</li>");
	crypter.setKey(orig);
};

var updateUserlist = function(){
	susi.publish("get-users",null,function(res){
		users = res.payload;
		console.log(users);
		$('#user-output').empty();
		for(var idx in users){
			if(users[idx].key !== crypter.getPublicKey()){
				$('#user-output').append("<li class='list-group-item'>"+users[idx].name+"<button class='pull-right' id='"+idx+"' type='button'>Whisper</button></li>");
				$('#'+idx).on('click',function(idx){
					whisper($('#text-input').val(),idx);
					$('#text-input').val('');
				}.bind(this,idx));
			}
		}
	});
};


$(function(){
	crypter = new JSEncrypt();

	susi = new Susi(function(susi){
		susi.registerConsumer("message",function(evt){
			$('#message-output').append("<li class='list-group-item'>"+evt.payload.name+" says: "+evt.payload.msg+"</li>");
		});
		susi.registerConsumer("register-key",function(){
			updateUserlist();
		});
		susi.registerConsumer("register-name",function(){
			updateUserlist();
		});
		susi.publish("register-key",crypter.getPublicKey(),function(){
			susi.registerConsumer("whisper@"+crypter.getPublicKey(),function(evt){
				$('#message-output').append("<li  class='list-group-item'>"+evt.payload.name+" whispered: "+crypter.decrypt(evt.payload.msg)+"</li>");
			});
		});
	});

	$('#text-button').on('click',function(){
		chat($('#text-input').val());
		$('#text-input').val('');
	});

	$('#name-button').on('click',function(){
		name = $('#name-input').val();
		susi.publish("register-name",name);
	});

});
