export const helpFunctions = {
    checkChecked: (checkedId, param)=> {
        checkedId == "On" 
        ? param.forEach(el => document.getElementById(el).disabled = true) 
        : param.forEach(el => document.getElementById(el).disabled = false);
    },
    addEventCheckChecked: (el, param) => {
        if(el) el.addEventListener("change", ()=> helpFunctions.checkChecked(el.value,param))
    }
}