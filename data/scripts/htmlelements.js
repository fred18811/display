const arrSend = [];
const objselect = {obj:undefined};
const page = 0;

//Здесь создаем элементы dwin
const formElement = (page, index, props)=> {
    const {
        address, 
        data
    } = props;
    
    const name = props.name ? props.name : "Новое имя";
    function getData (e) {
        const element = e.target;
        //Получаем свойста элемента
        const elDwinParams = document.querySelectorAll(`input[page="${element.getAttribute("page")}"][index="${element.getAttribute("index")}"]:not([type="button"])`);
        const objEl = {};
        elDwinParams.forEach(el => objEl[`${el.name}`] = el.value);
        //Проверяем наличие страницы в массиве или Создаем объект страницы
        const objPage = arrSend[element.getAttribute("page")] ? 
            arrSend[element.getAttribute("page")] : 
            {page: element.getAttribute("page"),data: []};
        // записываем эелемент в data массив по индексу
        objPage.data[element.getAttribute("index")] = objEl;
        //записываем объект страницы в массив
        arrSend[element.getAttribute("page")] = objPage;
    }
    function sendData (e) {
        //console.log(e.target);
    }

    const divElement = document.createElement("div");
    divElement.addEventListener("click",(e=>activateDiv(e.currentTarget)));
    divElement.classList.add("edit-form-element", "edit-form-column");
    divElement.id = `dwinelement${page}${index}`;
    divElement.setAttribute("addressform",address);
    divElement.setAttribute("index",index);

    //Создаем input name
    const inputName = document.createElement("input");
    inputName.type="text";
    inputName.name="name";
    inputName.id=`name${page}${index}`;
    inputName.setAttribute("page",page);
    inputName.setAttribute("index",index);
    inputName.value=`${name}`;
    inputName.classList.add("edit-form-element-header");
    inputName.addEventListener("input", getData);
    divElement.append(inputName);

    //Создаем input address
    const pAddress = document.createElement("p");
    pAddress.innerHTML = "Адрес элемента";
    divElement.append(pAddress);

    const inputAddress = document.createElement("input");
    inputAddress.type="number";
    inputAddress.name="address";
    inputAddress.id=`address${page}${index}`;
    inputAddress.setAttribute("page",page);
    inputAddress.setAttribute("index",index);
    inputAddress.value=`${address}`;
    inputAddress.addEventListener("input", getData);
    divElement.append(inputAddress);

    //Создаем input Data
    const pData = document.createElement("p");
    pData.innerHTML = "Данные";
    divElement.append(pData);

    const inputData = document.createElement("input");
    inputData.type="text";
    inputData.name="data";
    inputData.id=`data${page}${index}`;
    inputData.setAttribute("page",page);
    inputData.setAttribute("index",index);
    inputData.value=`${data}`;
    inputData.addEventListener("input", getData);
    divElement.append(inputData);

    //Создаем input button
    const inputButton = document.createElement("input");
    inputButton.type="button";
    inputButton.value=`Отправить`;
    inputButton.addEventListener("click", sendData);
    inputButton.setAttribute("page",page);
    inputButton.setAttribute("index",index);
    inputButton.classList.add("btnstyle");
    divElement.append(inputButton);

    return divElement;
    
}

// Здесь создаем разделы Страница dwin
const formElementPage = (page)=> {
    const divElement = document.createElement("div");
    divElement.classList.add("edit-form", "edit-form-column");
    divElement.innerHTML = `
            <hr style="min-width:244px">`;

    const divContent = document.createElement("div");
    divContent.classList.add("edit-form", "edit-form-row", "edit-form-wrap")
    divContent.setAttribute("page", page);
    divContent.setAttribute("formcontent", "");
    divElement.append(divContent);
    return divElement;
}

// Сюда приход запрос с сервера по data dwin
export const formContent = (arrElemntsDwin)=> {
    const divElement = document.createElement("div");
    divElement.classList.add("edit-form-width-100");
    divElement.setAttribute("pagecontent","");
    const elPage = formElementPage(page);
    const formContent = elPage.querySelector('div[formcontent=""]');
    arrElemntsDwin.forEach((element, id) =>{
        formContent.append(formElement(page,id,element))
    })
    divElement.append(elPage);
    return divElement;
}

// "Элементы кнопок"
export const formElementButtns = ()=> {
    const divElement = document.createElement("div");
    divElement.innerHTML = `
    <div class="edit-form-element edit-form-row edit-form-position-center edit-form-element-margin">
            <a href='/' class="btnstyle">Назад</a>
            <input type="button" value="Сохранить" class="btnstyle">
     </div>`;
    return divElement;
}

//Поиск элемента формы и измененние параметров
export const searchElementForm = (resalt) => {
    const element = document.querySelector(`div [addressform="${resalt.address}"]`);
    if(element){
        activateDiv(element);
        element.querySelector('input[name="data"]').value = resalt.data;
        return true;
    }
    else{
        return false;
    }
}

export const addNewElement = (resalt) => {
    const formElements = document.querySelector(`div [page="${page}"][formcontent]`);
    selsectingElement(formElements, page, resalt);
}

const selsectingElement = (el, page, resalt) => {
    if(el){
        const elements = el.querySelectorAll('div [addressform]');
        const index = elements[elements.length-1].getAttribute("index");
        const elForm = formElement(page,Number(index)+1, resalt);
        elForm.classList.add("edit-form-select");
        removingSelect();
        objselect.obj = elForm;
        el.append(elForm);
    }
    else {
        const pageContent = document.querySelector(`div[pagecontent]`);
        const divPage = formElementPage(page);
        const formContent = divPage.querySelector('div[formcontent=""]');
        const elForm = formElement(page, 0, resalt);
        elForm.classList.add("edit-form-select");
        removingSelect();
        objselect.obj = elForm;
        formContent.appendChild(elForm);
        pageContent.append(divPage);
    }
}
const removingSelect = () => {
    if(objselect.obj)
        objselect.obj.classList.remove("edit-form-select");
}

function activateDiv(el) {
    if(!objselect.obj) {
        objselect.obj =  el;
        el.classList.add("edit-form-select");
    }
    else if(objselect.obj.getAttribute("addressform") != el.getAttribute("addressform")) {
        removingSelect()
        el.classList.add("edit-form-select");
        objselect.obj =  el;
    }
}