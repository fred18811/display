import {helpFunctions} from "./helpfunctions.js";
import {config} from "./dwinconfig.js";

//Здесь создаем элемент формы dwin
export const formElement = (page, index, props)=> {
    const urlLink = props && props.urllink ? props.urllink : "";
    const ports = props && props.ports ? props.ports : "";
    const elcheckbox = props && props.elcheckbox ? props.elcheckbox : false;
    const typebtn = props && props.typebtn ? props.typebtn : "Text";
    const likeBtn = props && props.likeBtn ? props.likeBtn : false;
    const likeDimmer = props && props.likeDimmer ? props.likeDimmer : false;
    const name =  props && props.name ? props.name : "Новое имя";
    const address = props && props.address ? props.address : "";
    const dataDeafult = props && props.dataDeafult ? props.dataDeafult : "";
    const tempValue = props && props.tempValue ? props.tempValue : "";
    const minValue = props && props.minValue ? props.minValue : 0;
    const maxValue = props && props.maxValue ? props.maxValue : 0;
    const timer = props && props.timer ? props.timer : 0;
    const statePort = props && props.statePort ? props.statePort : "";

    function getPageIndex (e) {
        const index = e.target.getAttribute("index");
        const page = e.target.getAttribute("page");
        return String(page) + String(index);
    }

    function deletingObj (e) {
        document.querySelector(`#dwinelement${getPageIndex(e)}`).remove();
    }

    function editAddressForm (e) {
        const el = document.querySelector(`#dwinelement${getPageIndex(e)}`);
        el.setAttribute("addressform", e.target.value);
    }

    function addingOptionElement (e) {
        let strHtml ="";
        for(let i in config.btnInput){
            if(typebtn == i) strHtml += `<option selected value="${i}">${i}</option>`;
            else strHtml += `<option value="${i}">${i}</option>`;
        }
        return strHtml
    }
    
    function selectingTypeInput (e) {
        const arrEl = divElement.querySelectorAll("input[changeInput]");
        arrEl.forEach(i => {
            i.type = config.btnInput[e.target.value].type;
            i.setAttribute("max", config.btnInput[e.target.value].max);
        })
    }

    function checkingPattern (e) {
        if(e.target.type === "number") {
            e.preventDefault();
            if(Number(String(e.target.value)+String(e.key)) <= Number(e.target.max)) e.target.value += e.key;
        }
    }
    
    function getParamForLink() {
        const strLine = {
            "urllink" : "",
            "ports": ""
        };
        
        divElement.querySelectorAll("[linkPart]").forEach(i => {
            if(i.hasAttribute("portsData")) strLine["portsData"] = i.value;
            else strLine[i.name] = i.value;
        })
        return strLine;
    }
    function getStringPortsData (arrPorts, arrPortsData) {
        let data = "";
        for (let i in arrPorts) {
            if(arrPorts.length == arrPortsData.length && arrPorts[i] != ""){
                data += arrPorts[i] + ":" + arrPortsData[i] + ";";
            }
            else {
                if(arrPorts[i] != "") data += arrPorts[i] + ":" + arrPortsData[0] + ";";
            }
        };
        return data;
    }
    function insertLinkToInput () {
        const link = getParamForLink();
        let data = getStringPortsData (link.ports.split(";"), link.portsData.split(";"));
        divElement.querySelector("#lineRequest").value = `${link.urllink}${data}`;
    }

    function sendLinkToController () {
        const link = getParamForLink();
        link.portsData = getStringPortsData (link.ports.split(";"), link.portsData.split(";"));
        const req = fetch(`/sendlinkfromdwin`,{
            method: 'POST',
            body: JSON.stringify(link),
            headers: {
              'Content-Type': 'application/json'
            }
        });
        req.then(answ => console.log(answ));
    }

    const divElement = helpFunctions.parsingHTMLElements(`
    <div class="edit-form-element edit-form-column" id="dwinelement${page}${index}" addressform="${address}" index="${index}" >
        <div style="display: flex; justify-content: space-between; align-items: center;">
            <div style="display: flex;">
                <input name="elcheckbox" type="checkbox" ${elcheckbox ? "checked" : ""}>
                <label for="elcheckbox">Отображать</label>
            </div>
            <p deletelement style="text-align:end;cursor: pointer;font-size: 16px;" page="${page}" index="${index}">
                x
            </p>
        </div>
        <input type="text" name="name" id="name${page}${index}" page="${page}" index="${index}" value="${name}" class="edit-form-element-header">
        <hr>
        <h3>Данные экрана</h3>
        <hr>
        <label for="typebtn" class="visually-hidden">Тип элемент</label>
        <div style="margin:0;padding:0; display:flex;">
            <select name="typebtn" id="typebtn" value="${typebtn}" style="flex-grow:1;"></select>
            ${typebtn == config.btnInput.Button.name ? `
            <label for="likeBtn" class="visually-hidden">Кнопка</label>
            <input name="likeBtn" type="checkbox" ${likeBtn ? "checked" : ""}>
            <label for="likeDimmer" class="visually-hidden">Диммер</label>
            <input name="likeDimmer" type="checkbox" ${likeDimmer ? "checked" : ""}>
            `:""}
        </div>

        <label for="address">Адрес элемента</label>
        <input type="number" name="address" id="address${page}${index}" page="${page}" index="${index}" value="${address}">
         
        <hr>
        <div class="edit-form" style="margin:0;">
            <div class="edit-form">
                <label for="minValue">Мин</label>
                <input type="number" name="minValue" id="minValue${page}${index}" page="${page}" index="${index}" value="${minValue}" style="width: 50px;">
            </div>
            <div class="edit-form">
                <label for="maxValue">Макс</label>
                <input type="number" name="maxValue" id="maxValue${page}${index}" page="${page}" index="${index}" value="${maxValue}" style="width: 50px;">
            </div>
        </div>
    
        <label for="dataDeafult">Значение по умоланию</label>
        <input changeInput type="${config.btnInput[typebtn].type}" max="${maxValue}" min="${minValue}" name="dataDeafult" id="dataDeafult${page}${index}" page="${page}" index="${index}" value="${dataDeafult}">

        ${typebtn !== "Text"?`
        <hr>
        <h3>Данные контроллера</h3>
        <hr>

        <label for="urllink">URL адрес</label>
        <input linkPart type="text" name="urllink" id="urllink${page}${index}" page="${page}" index="${index}" value="${urlLink}">

        <label for="ports">Порты</label>
        <input linkPart type="text" name="ports" id="ports${page}${index}" page="${page}" index="${index}" value="${ports}">

        <label for="portsData">Данные порт</label>
        <input linkPart type="text" name="tempValue" portsData id="portsData${page}${index}" page="${page}" index="${index}" value="${tempValue}">

        <label for="timer">Время опроса состояния (сек.)</label>
        <input linkPart type="number" name="timer" id="timer${page}${index}" page="${page}" index="${index}" value="${timer}">

        <label for="statePort">Получить состояние порта</label>
        <input linkPart type="text" name="statePort" id="statePort${page}${index}" page="${page}" index="${index}" value="${statePort}">

        <label for="lineRequest">Полученная строка запроса</label>
        <div style="margin:0;padding:0;display: flex;height: 31px;align-items: center;">
            <input type="text" id="lineRequest" class="edit-form-element-header" readonly style="font-size: 10px;flex-grow: 1;" value="${urlLink}${ports}">
            <input senttestreq type="button" name="data" page="${page}" index="${index}" value="\u25BA" class="btnstyle">
        </div>
        `:""}
    </div>
    `);
    divElement.querySelector("#typebtn").innerHTML = addingOptionElement();
    divElement.querySelector("#typebtn").addEventListener("click", (e) => selectingTypeInput(e));

    divElement.querySelector("p[deletelement]").addEventListener("click", (e) => deletingObj(e));
    divElement.querySelector("input[name='address']").addEventListener("input", (e)=> editAddressForm(e));

    divElement.querySelectorAll("input[changeInput]").forEach(i => i.addEventListener("keypress", (e)=>checkingPattern(e)));
    divElement.querySelectorAll("[linkPart]").forEach(i => i.addEventListener("input", ()=>insertLinkToInput()));

    divElement.querySelector("[senttestreq]") ? divElement.querySelector("[senttestreq]").addEventListener("click", sendLinkToController) : "";

    return divElement;
}