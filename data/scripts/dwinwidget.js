// Два запроса fetch, сделать один!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

import { helpFunctions } from "./helpfunctions.js";
import { config } from "./dwinconfig.js";
import { startDwin} from "./dwinsetting.js";

//-----------Основная функция-------------
export const diwiwidget = (websocket) => {
    //----Получаем данны от сервера и затем подключаем вебсокет-------------------------------------------------------------
    const boolgetData = helpFunctions.proxyObj({data : false},
        () => {
            if(boolgetData["data"] && document.querySelector("div[dwinwidget]")){
                websocket.onmessage = function(event){
                    const result = JSON.parse(event.data);
                    changeValueObject(result);
                };
            };
        }
    );
    //-------------------Вывод страницы настроек кнопока экрана------------------------------------------------------------
    if(document.title == "Настройки DWIN"){startDwin(websocket)};

    
    function changeValueObject (result) {
        const el = document.querySelectorAll(`[address="${result.address}"]`);
        el.forEach(i => {
            i.hasAttribute("btnbutton") || i.hasAttribute("textbutton")? i.value = result.data : "";
            if(i.hasAttribute("btnligth")){ 
                if(Number(result.data) > 0 && i.classList.contains("button-light-red")){
                    i.classList.remove("button-light-red");
                    i.classList.add("button-light-green");
                }
                if(Number(result.data) === 0 && i.classList.contains("button-light-green")){
                    i.classList.remove("button-light-green");
                    i.classList.add("button-light-red");
                }
            };
            if(i.hasAttribute("textbutton")){
                i.innerHTML = result.data;
            }  
        })

    }

    const request_settings = fetch(`${config.getdwinsetting}`,{'Cache-Control': 'no-cache'}); //Запрос к серверу, получение данных элементов (Исправить)
    request_settings
    .then(response=>response.json())
    .then(data=> {
        if(document.querySelector("#widgets")) 
            document.querySelector("#widgets").append(elementForm(data.elements));
            boolgetData.data = true;
    });
}

const elementForm = (arrData) => {
    const divElement = helpFunctions.parsingHTMLElements(`
    <div dwinwidget>
        <br>
        <hr style="min-width:244px">
        <div class="edit-form edit-form-position-end edit-form-align-position-center">
            <a href="settingdwin">
                <img src="./icon/dwinSetting.png" alt="Настройки">
            </a>
        </div>
        <div id="dwinform" class="edit-form edit-form-column">
            <div class="edit-form-width-100" pagecontent>
                <div class="edit-form edit-form-column">
                    <div class="edit-form edit-form-row edit-form-wrap" page="${config.page}" formcontent></div>
                </div>
            </div>
        </div>
    </div>
    `);

    arrData.forEach((element, id) =>{
        if(element.elcheckbox)
            divElement.querySelector("div[formcontent]").append(buttons[element.typebtn](config.page,id,element));
    })

    return divElement;
}

const buttons = {
    Button : (page, index, element) => {
        function setGreenLight (i) {
                    i.classList.remove("button-light-red");
                    i.classList.add("button-light-green"); 
        }
        function setRedLight (i) {
                i.classList.remove("button-light-green");
                i.classList.add("button-light-red"); 
        }
        function getValue(e, address){
            let dataValue = 0;
            const arrBtn = document.querySelectorAll(`[address="${address}"][btnligth]`);
            const arrDimmer = document.querySelectorAll(`[address="${address}"][btnbutton]`);
            const isRed = document.querySelector(`[address="${address}"][btnligth]`) ? document.querySelector(`[address="${address}"][btnligth]`).classList.contains("button-light-red") : false;

            arrDimmer.forEach(i => {
                if(e.target.hasAttribute("btnbox") || e.target.hasAttribute("btnligth")){
                    console.log(1);
                    if(isRed) {
                        i.value = element.maxValue;
                        dataValue = element.maxValue;
                    } 
                    else {
                        i.value = element.minValue;
                        dataValue = element.minValue;
                    } 
                }
                else {
                    i.value = e.target.value;
                    dataValue = e.target.value;
                }
                
            })
            arrBtn.forEach(i => {
                if(e.target.hasAttribute("btnbutton")) dataValue > 0 ? setGreenLight(i) : setRedLight(i);
                else isRed ? setGreenLight(i) : setRedLight(i);
            })

            const request = fetch(`/getdwinreq?address=${element.address}&data=${dataValue}`,{'Cache-Control': 'no-cache'});
            request.then(answ => console.log(answ));
        }

        const button = helpFunctions.parsingHTMLElements(`
        <div class="edit-form-element edit-form-column" id="dwinelement${page}${index}" index="${index}" >
            <div class="edit-form edit-form-column" style="align-items: center; flex-grow:1;">
                <label for="button${page}${index}" class="visually-hidden">${element.name}</label>
                ${element.likeDimmer ? `
                <hr style="width: 100%;">
                <input btnbutton page="${page}" index="${index}" style="flex-grow: 1;" address="${element.address}" class="dimmer-input" type="range" min="${element.minValue}" max="${element.maxValue}" value="${element.tempValue}" name="button${page}${index}" orient="vertical">
                ` : ""}
                ${element.likeBtn ? `
                <hr style="width: 100%;">
                <div btnbox name="btn${page}${index}" page="${page}" index="${index}" style="flex:1; display: flex; align-items: end; width: 100%; height: 100%; justify-content: center;">
                    <div btnbutton btnligth class="button-light ${Number(element.tempValue) > 0 ? "button-light-green" : "button-light-red" }" address="${element.address}" name="btnligth${page}${index}" page="${page}" index="${index}"></div>
                </div>
                ` : ""}
            </div>
        </div>
        `);

        if(button.querySelector(`input[name='button${page}${index}']`)){
            button.querySelector(`input[name='button${page}${index}']`).addEventListener("click",(e)=>getValue(e, element.address));
            button.querySelector(`input[name='button${page}${index}']`).addEventListener("touchend",(e)=>getValue(e, element.address));
        }
        if(button.querySelector(`div[name='btn${page}${index}']`)) button.querySelector(`div[name='btn${page}${index}']`).addEventListener("click",(e)=>getValue(e, element.address));
        return button;
    },
    Text : (page, index, element) => {
        const text = helpFunctions.parsingHTMLElements(`
        <div class="edit-form-element edit-form-column" id="dwinelement${page}${index}" index="${index}" >
            <div class="edit-form edit-form-column" style="align-items: center; flex-grow:1;">
                <label for="text${page}${index}" class="visually-hidden">${element.name}</label>
                <hr style="width: 100%;">
                <div class="edit-form" style="height: 100%;align-items: center;font-size: 60px;">
                    <p textbutton address="${element.address}">${element.tempValue}</p>
                </div>
            </div>
        </div>
        `);
        return text;
    }
}