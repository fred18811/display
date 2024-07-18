// сделать функцию sendSavedData кнопка Сохранить. Собратьвсе данные элементов, отправить на сервер!
import { helpFunctions } from "./helpfunctions.js";
import {formElement} from "./dwinformelement.js";
import { config } from "./dwinconfig.js";


//Основаная функция запуска
export const startDwin = (websocket) => {
            const request_settings = fetch(`${config.getdwinsetting}`,{'Cache-Control': 'no-cache'});
            request_settings
            .then(response=>response.json())
            .then(data=> {
                if(document.querySelector("#dwinsettings")) 
                    document.querySelector("#dwinsettings").append(formContent(data));
            });
            //---------------------------------------
            websocket.onmessage = function(event){
                const resalt = JSON.parse(event.data);
                !searchElementForm(resalt) ? addNewElement(resalt) : null;
            };
}
//Кнопка добавления нового элемента
const buttonCreatNewElement = () => {
    function addingButtns (){
        if(!document.querySelector('div[addressform=""]')) addNewElement();
    };

    const divButton = helpFunctions.parsingHTMLElements(`
    <div class="edit-form edit-form-column edit-form-position-center" buttonCreat style="width: 265px; align-items: center; background: rgba(255, 255, 255, 0.27);">
        <div class="button-add">+</div>
    </div>
    `);
    divButton.querySelector("div.button-add").addEventListener("click", () => addingButtns());
    return divButton
}

// "Элементы кнопок"
const formElementButtns = ()=> {
    //Отправить данные элементов на сервер
    function sendSavedData (obj) {
        const arr = obj.elements;
        arr.length = 0;
        const arrEl = document.querySelectorAll("div [addressform]");
        const dwinOptions = document.querySelectorAll("input[dwinoptions]");
        dwinOptions.forEach(el => {
            obj[el.name] = el.value;
        });
        arrEl.forEach(el => {
            const obj = {}; //Создаем объекь, добавляем в него данные
            const childrenEl = el.querySelectorAll("[name]");
            childrenEl.forEach(chEl => {
                if(chEl.getAttribute("name") != "data") {
                    if(chEl.type === "checkbox" && chEl.checked) obj[chEl.getAttribute("name")]=true;
                    else if(chEl.type === "checkbox" ) obj[chEl.getAttribute("name")]=false;
                    else obj[chEl.getAttribute("name")]=chEl.value;
                }
            });
            if(obj.address != "") arr.push(obj); //Добовляем объекты в массив
        });

        obj.elements = arr
        const arrString = JSON.stringify(obj);
        const req = fetch(`/savedwinsetting`,{
            method: 'PUT',
            body: arrString,
            headers: {
              'Content-Type': 'application/json'
            }
        });
        req.then(answ => {
            if(answ.statusText === "OK"){
                document.querySelector("div[dialogsaveform]").classList.remove("disabled");
                setTimeout(()=>{
                    document.querySelector("div[dialogsaveform]").classList.add("disabled");
                },2000)
            }
            if(answ.statusText != "OK") console.log(answ);
            //Дописать что если сохранение прошло удачно, то вывести диалоговое окно об успешном сохранении, иначе неудача!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        });
    }

    const divElement = helpFunctions.parsingHTMLElements(`
    <div class="edit-form-element edit-form-row edit-form-position-center edit-form-element-margin">
     <a href="/" class="btnstyle">Назад</a>
     <input class="btnstyle" type="button" value="Сохранить">
    </div>
    `);
    divElement.querySelector("input.btnstyle").addEventListener("click",()=>sendSavedData(config.objSend));
    return divElement;
}

// Сюда приход запрос с сервера по data dwin
const formContent = (props)=> {
    const arrElemntsDwin = props && props.elements ? props.elements : [];
    const timer = props && props.timer ? props.timer : "";

    const divElement = helpFunctions.parsingHTMLElements(`
    <div>
        <h1>Настройки кнопок DWIN</h1>
        <div id="dwinform" class="edit-form edit-form-column">
            <div class="" pagecontent>
                <div class="edit-form edit-form-column">
                    <hr style="min-width:244px">

                    <div class="edit-form-element">
                        <label for="timer">Время опроса состояния (сек.)</label>
                        <input dwinoptions type="number" name="timer" id="timer${config.page}" page="${config.page}" index="${config.page}" value="${timer}">
                    </div>

                    <div class="edit-form edit-form-row edit-form-wrap" page="${config.page}" formcontent></div>
                </div>
            </div>
        </div>
        <div id="dwinformbuttons" class="edit-form edit-form-column">
        </div>
        <div dialogsaveform class="save-form disabled">
            <div>
                Сохранено!
            </div>
        </div>
    </div>
    `);

    //Добовляем элементы
    const formContent = divElement.querySelector('div[formcontent=""]');
    arrElemntsDwin.forEach((element, id) =>{
        formContent.append(formElement(config.page,id,element))
    })
    formContent.append(buttonCreatNewElement());

    //Добовляем кнопки
    divElement.querySelector("#dwinformbuttons").append(formElementButtns());

    return divElement;
}

//Поиск элемента формы и измененние параметров
const searchElementForm = (resalt) => {
    const element = document.querySelector(`div [addressform="${resalt.address}"]`);
    if(element){
        activateDiv(element);
        element.querySelector('input[name="tempValue"]').value = resalt.data;
        return true;
    }
    else{
        return false;
    }
}

const addNewElement = (resalt) => {
    const formElements = document.querySelector(`div [page="${config.page}"][formcontent]`);
    selsectingElement(formElements, config.page, resalt);
}

const selsectingElement = (el, page, resalt) => {
    if(el){
        const elements = el.querySelectorAll('div [addressform]');
        const index = elements[elements.length-1] ? elements[elements.length-1].getAttribute("index") : -1;
        const elForm = formElement(config.page,Number(index)+1, resalt);
        elForm.classList.add("edit-form-select");
        removingSelect();
        config.objselect.obj = elForm;
        const lastChildren = el.querySelector('div[buttoncreat]');
        el.insertBefore(elForm, lastChildren);
        //el.append(elForm);
    }
    else {
        const pageContent = document.querySelector(`div[pagecontent]`);
        const divPage = formElementPage(config.page);
        const formContent = divPage.querySelector('div[formcontent=""]');
        const elForm = formElement(config.page, 0, resalt);
        elForm.classList.add("edit-form-select");
        removingSelect();
        config.objselect.obj = elForm;
        formContent.appendChild(elForm);
        pageContent.append(divPage);
    }
}

//Снять выделение с объекта
function removingSelect() {
    if(config.objselect.obj)
        config.objselect.obj.classList.remove("edit-form-select");
}

//Выделить объект
function activateDiv(el) {
    if(!config.objselect.obj) {
        config.objselect.obj =  el;
        el.classList.add("edit-form-select");
    }
    else if(config.objselect.obj.getAttribute("addressform") != el.getAttribute("addressform")) {
        removingSelect()
        el.classList.add("edit-form-select");
        config.objselect.obj =  el;
    }
}