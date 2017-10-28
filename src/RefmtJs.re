type refmtResult = {
  ocamlCode: option(string),
  reasonCode: option(string),
  errorMessage: option(string)
};

type astAndComments;

[@bs.module "reason/refmt"] external parseRE : string => astAndComments = "";

[@bs.module "reason/refmt"] external parseML : string => astAndComments = "";

[@bs.module "reason/refmt"] external printRE : astAndComments => string = "";

[@bs.module "reason/refmt"] external printML : astAndComments => string = "";

let refmtRE2ML = (code) =>
  try {
    let astAndComments = parseRE(code);
    let ocamlCode = printML(astAndComments);
    {ocamlCode: Some(ocamlCode), reasonCode: None, errorMessage: None}
  } {
  | Js.Exn.Error(e) => {ocamlCode: None, reasonCode: None, errorMessage: Js.Exn.message(e)}
  };

let refmtML2RE = (code) =>
  try {
    let astAndComments = parseML(code);
    let reasonCode = printRE(astAndComments);
    {ocamlCode: None, reasonCode: Some(reasonCode), errorMessage: None}
  } {
  | Js.Exn.Error(e) => {ocamlCode: None, reasonCode: None, errorMessage: Js.Exn.message(e)}
  };
