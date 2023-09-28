import { helpFunctions } from "./helpFunctions.js";

window.addEventListener("load",loader);
function loader() {
    // let mass_setting_controller = document.querySelectorAll('input[type="number"');
    // let timerId = null;
    // let dataJson ={}
    // let host_name = window.location.hostname;
    // let gateway = "ws://" + host_name + "/ws";
    // let websocket = new WebSocket(gateway);
 
//-------------------Принимаем данные от сервера------------------------------------------------------------
    // if(document.title == "Главное меню"){
    //     websocket.onmessage = function(event){
    //         for (i of event.data.split(";")){
    //             let val =  i.split(":");
    //             if(document.getElementById(val[0]))
    //                 document.getElementById(val[0]).value = parseFloat(val[1]).toFixed(2);
    //         }
    //     };
    // }
//------------------------------------------------------------------------------------------------------------
//-------------------Отправляем измененные данные на сервер---------------------------------------------------
    // for(i in mass_setting_controller){
    //     if(mass_setting_controller[i].type == "number"){
    //         mass_setting_controller[i].addEventListener("input",(e)=>{
    //             dataJson[e.target.id] = e.target.value;
    //             if(timerId){
    //                 clearTimeout(timerId);
    //                 timerId = null;
    //             }
    //             if(e.target.value){
    //                 let stateBool = true;
    //                 for (key in dataJson) {
    //                     if(!dataJson[key]){
    //                         stateBool = false;
    //                         break;
    //                     }
    //                 }
    //                 if(stateBool){
    //                     timerId = setTimeout(sentData, 3000, dataJson);
    //                 }
    //             }
    //         });
    //     }
    // }
//-------------------Получаем настройки wifi от сервера-------------------------------------------------------
    if(document.getElementById("ethsettings") || document.getElementById("main")){
        let request_settings = fetch("/getethsetting",{'Cache-Control': 'no-cache'});
        request_settings
        .then(response=>response.json())
        .then(data=>{
            for (let key in data) {
                    // if(key == "wifimode" && document.getElementById("wifimode")) helpFunctions.checkChecked(data[key],["dhsp","ssdp_name","ssid","pswd","ip","gateway","subnet"]);
                    if(key == "dhsp" && document.getElementById("dhsp")) helpFunctions.checkChecked(data[key],["ip","gateway","subnet"]);
                    if(key == "mqtton"&& document.getElementById("mqtton")) helpFunctions.checkChecked(data[key],["ip_mqtt","port_mqtt","id_mqtt"]);
                    if(key == "versionProsh" && data[key] && document.getElementById(key)) document.getElementById(key).innerHTML = data[key];
                    if(document.getElementById(key)) document.getElementById(key).value = data[key];
            }
        })
        .catch(error => console.log(error));
    }

    // helpFunctions.addEventCheckChecked(document.getElementById("wifimode"), ["dhsp","ssdp_name","ssid","pswd","ip","gateway","subnet"]);
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


// function sentData(jsonData){
//     let strdata = "";
//     if(document.getElementById("controllersettings")){
//         strdata = "controlsetting=1&";
//     }
//     for(i in jsonData){
//         strdata += i;
//         strdata += "=";
//         strdata += parseInt(jsonData[i]);
//         strdata += "&";
//     }

//     fetch('/sentdata', {
//         method: 'POST',
//         body: strdata
//     })
//       .catch(error => console.error(error));}