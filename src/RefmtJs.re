type refmtResult = {
  ocamlCode: option(string),
  reasonCode: option(string),
  errorMessage: option(string)
};

[@bs.val] external refmt : (string, string, string, string) => array(string) = "";

[@bs.get_index] external get : (array(string), int) => string = "";

let refmtRE2ML = (code) => {
  let converted = refmt(code, "RE", "implementation", "ML");
  if (get(converted, 0) == "REtoML") {
    {ocamlCode: Some(get(converted, 1)), reasonCode: None, errorMessage: None}
  } else if (get(converted, 1) == "") {
    {ocamlCode: None, reasonCode: None, errorMessage: Some("Syntax error")}
  } else {
    {ocamlCode: None, reasonCode: None, errorMessage: Some(get(converted, 1))}
  }
};

let refmtML2RE = (code) => {
  let converted = refmt(code, "ML", "implementation", "RE");
  if (get(converted, 0) == "MLtoRE") {
    {ocamlCode: None, reasonCode: Some(get(converted, 1)), errorMessage: None}
  } else if (get(converted, 1) == "") {
    {ocamlCode: None, reasonCode: None, errorMessage: Some("Syntax error")}
  } else {
    {ocamlCode: None, reasonCode: None, errorMessage: Some(get(converted, 1))}
  }
};
