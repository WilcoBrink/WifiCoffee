function GetLEDState(){
	var request = new XMLHttpRequest();
	request.onreadystatechange = function(){
		if(this.readyState == 4){
			if(this.status == 200){
				if(this.responseText != null){
					document.getElementById("led_txt").innerHTML = this.responseText;
				}
			}
		}
	}
	request.open("GET", "led_state", true);
	request.send(null);
	setTimeout('GetLEDState()', 5000);
}

function MakeCoffee(amount){
	var request = new XMLHttpRequest();
	/*request.onreadystatechange = function(){
		if(this.readyState == 4){
			if(this.status == 200){
				if(this.responseText != null){
					document.getElementById("brewing_status").innerHTML = this.responseText;
				}
			}
		}
	}*/
	request.open("POST", "make_coffee?value=" + amount);
	request.send(null);
}

function GetDebugData(){
	var request = new XMLHttpRequest();
	request.onreadystatechange = function(){
		if(this.readyState == 4){
			if(this.status == 200){
				if(this.responseXML != null){
					document.getElementById("water_level").innerHTML = this.responseXML.getElementsByTagName('water_level')[0].childNodes[0].nodeValue;
					document.getElementById("dispenser").innerHTML = this.responseXML.getElementsByTagName('dispenser')[0].childNodes[0].nodeValue;
					document.getElementById("mug_left").innerHTML = this.responseXML.getElementsByTagName('mug_left')[0].childNodes[0].nodeValue;
					document.getElementById("mug_middle").innerHTML = this.responseXML.getElementsByTagName('mug_middle')[0].childNodes[0].nodeValue;
					document.getElementById("mug_right").innerHTML = this.responseXML.getElementsByTagName('mug_right')[0].childNodes[0].nodeValue;
					document.getElementById("switch").innerHTML = this.responseXML.getElementsByTagName('switch')[0].childNodes[0].nodeValue;
				}
			}
		}
	}
	request.open("GET", "debug_data", true);
	request.send(null);
	setTimeout('GetDebugData()', 1000);
}

function GetBrewingStatus(){
	var request = new XMLHttpRequest();
	request.onreadystatechange = function(){
		if(this.readyState == 4){
			if(this.status == 200){
				if(this.responseText != null){
					document.getElementById("brewing_status").innerHTML = this.responseText;
				}
			}
		}
	}
	request.open("GET", "brewing_status", true);
	request.send(null);
	setTimeout('GetBrewingStatus()', 200);
}
