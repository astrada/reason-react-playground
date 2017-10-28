type evalResult = {. "errorMessage": Js.nullable(string)};

[@bs.val] external eval : string => unit = "";

let evalJs = (code) =>
  try {
    eval(code);
    {"errorMessage": Js.Nullable.null}
  } {
  | Js.Exn.Error(e) => {"errorMessage": Js.Nullable.from_opt(Js.Exn.message(e))}
  };
