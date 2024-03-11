import { helpFunctions } from "./helpfunctions.js";
import { diwiwidget } from "./dwinwidget.js";

window.addEventListener("load",loader);
function loader() {
    // ----------------------------------------
    const host_name = window.location.hostname;
    const gateway = "ws://" + host_name + "/ws";
    const websocket = new WebSocket(gateway);
 
//------------------Виджеты на главную страницу------------------------------------------------------------
diwiwidget(websocket);
//-------------------Получаем настройки wifi от сервера-------------------------------------------------------
    if(document.getElementById("ethsettings") || document.getElementById("main")){
        let request_settings = fetch("/getethsetting",{'Cache-Control': 'no-cache'});
        request_settings
        .then(response=>response.json())
        .then(data=>{
            for (let key in data) {
                    // if(key == "wifimode" && document.getElementById("wifimode")) helpFunctions.checkChecked(data[key],["dhsp","host_name","ssid","pswd","ip","gateway","subnet"]);
                    if(key == "dhsp" && document.getElementById("dhsp")) helpFunctions.checkChecked(data[key],["ip","gateway","subnet"]);
                    if(key == "mqtton"&& document.getElementById("mqtton")) helpFunctions.checkChecked(data[key],["ip_mqtt","port_mqtt","id_mqtt"]);
                    if(key == "versionProsh" && data[key] && document.getElementById(key)) document.getElementById(key).innerHTML = data[key];
                    if(key == "host_name" && data[key] && document.getElementById(key)) document.getElementById(key).innerHTML = data[key];
                    if(document.getElementById(key)) document.getElementById(key).value = data[key];
            }
        })
        .catch(error => console.log(error));
    }

    // helpFunctions.addEventCheckChecked(document.getElementById("wifimode"), ["dhsp","host_name","ssid","pswd","ip","gateway","subnet"]);
    helpFunctions.addEventCheckChecked(document.getElementById("dhsp"), ["ip","gateway","subnet"]);
    helpFunctions.addEventCheckChecked(document.getElementById("mqtton"), ["ip_mqtt","port_mqtt","id_mqtt"]);
    //----------------------------------------------------------------------------
    if(document.querySelector("form[method='POST']"))document.querySelector("form[method='POST']").addEventListener("submit",(e)=>{
        e.preventDefault();
        const data = new FormData(e.target);
        const response = fetch('/saveether', {
            method: 'POST',
            body: data
          });
        response.then(res => res.statusText === "OK"? document.querySelector("h3.savemessage").classList.remove("disabled"):"");
    })
}