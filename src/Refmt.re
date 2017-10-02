type compilerResult = Js.t {. ocamlCode : string, errorMessage : string};

external refmt : string => string => string => string => array string = "" [@@bs.val];

external get : array string => int => string = "" [@@bs.get_index];

let refmt code => {
  let converted = refmt code "RE" "implementation" "ML";
  if (get converted 0 == "REtoML") {
    {"ocamlCode": get converted 1, "errorMessage": ""}
  } else if (
    get converted 1 == ""
  ) {
    {"ocamlCode": "", "errorMessage": "Syntax error"}
  } else {
    {"ocamlCode": "", "errorMessage": get converted 1}
  }
};
