type refmtResult = {
  ocamlCode: option(string),
  reasonCode: option(string),
  errorMessage: option(string)
};

type astAndComments;

type location = {
  .
  "startLine": Js.nullable(int),
  "startLineStartChar": Js.nullable(int),
  "endLine": Js.nullable(int),
  "endLineEndChar": Js.nullable(int)
};

type parseError = {
  .
  "message": Js.nullable(string),
  "location": Js.nullable(location)
};

external toParseError : Js.Exn.t => parseError = "%identity";

[@bs.module "reason/refmt"] external parseRE : string => astAndComments = "";

[@bs.module "reason/refmt"] external parseML : string => astAndComments = "";

[@bs.module "reason/refmt"] external printRE : astAndComments => string = "";

[@bs.module "reason/refmt"] external printML : astAndComments => string = "";

let errorCodeRegex = [%re "/^[0-9]+: /"];

let filterErrorCodeInErrorMessage = e => {
  let message = Js.Option.getWithDefault("", Js.Exn.message(e));
  Js.String.replaceByRe(errorCodeRegex, "", message);
};

let buildErrorMessage = e => {
  let parseError = toParseError(e);
  switch (Js.toOption(parseError##location)) {
  | Some(location) =>
    let sl = Js.Option.getWithDefault(0, Js.toOption(location##startLine));
    let el = Js.Option.getWithDefault(0, Js.toOption(location##endLine));
    let slsc =
      Js.Option.getWithDefault(0, Js.toOption(location##startLineStartChar));
    let elec =
      Js.Option.getWithDefault(0, Js.toOption(location##endLineEndChar));
    let locationMessage =
      if (sl == el) {
        if (slsc == elec) {
          {j|Line $sl, $slsc|j};
        } else {
          {j|Line $sl, $slsc-$elec|j};
        };
      } else {
        {j|Line $sl, $slsc-Line $el, $elec|j};
      };
    locationMessage ++ ": " ++ filterErrorCodeInErrorMessage(e);
  | None => "Syntax error"
  };
};

let refmtRE2ML = code =>
  try (
    {
      let astAndComments = parseRE(code);
      let ocamlCode = printML(astAndComments);
      {ocamlCode: Some(ocamlCode), reasonCode: None, errorMessage: None};
    }
  ) {
  | Js.Exn.Error(e) =>
    let errorMessage = buildErrorMessage(e);
    {ocamlCode: None, reasonCode: None, errorMessage: Some(errorMessage)};
  };

let refmtML2RE = code =>
  try (
    {
      let astAndComments = parseML(code);
      let reasonCode = printRE(astAndComments);
      {ocamlCode: None, reasonCode: Some(reasonCode), errorMessage: None};
    }
  ) {
  | Js.Exn.Error(e) =>
    let errorMessage = buildErrorMessage(e);
    {ocamlCode: None, reasonCode: None, errorMessage: Some(errorMessage)};
  };
