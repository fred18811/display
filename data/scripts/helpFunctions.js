export const helpFunctions = {
    checkChecked: (checkedId, param)=> {
        checkedId == "On" 
        ? param.forEach(el => document.getElementById(el).disabled = true) 
        : param.forEach(el => document.getElementById(el).disabled = false);
    },
    addEventCheckChecked: (el, param) => {
        if(el) el.addEventListener("change", ()=> helpFunctions.checkChecked(el.value,param))
    },
    convertStringToHTML: (str) => {
        let res = document.createElement('template');
        res.innerHTML = str;
        return res.content;
     },
     parsingHTMLElements: (element) => {
        const parser = new DOMParser();
        return parser.parseFromString(element, 'text/html').body.firstElementChild; 
     },
     proxyObj: (obj, func) => {
        return new Proxy(obj, {
            set: function (target, key, value) {
                target[key] = value;
                func();
                return true;
            },
            get: function (target, prop)  {
                if (prop in target) {
                    return target[prop];
                  } else {
                    return 0;
                  }
            }
        })
     }
}